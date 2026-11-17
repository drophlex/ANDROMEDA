#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace andro;

static juce::String pid(int i) { return "p" + juce::String(i).paddedLeft('0', 2); }

AndromedaProcessor::AndromedaProcessor()
  : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     .withInput("Sidechain", juce::AudioChannelSet::stereo(), false)) {
  eng = std::make_unique<Engine>();
  eng->init(48000.f); sr = 48000;
  // 74 automatable parameters, in the 8-per-bank order the KeyLab / Push / any controller sees.
  for (int i = 0; i < P_COUNT; i++) {
    const ParamMeta& m = PARAMS[i];
    juce::RangedAudioParameter* p;
    if (m.steps > 1)
      p = new juce::AudioParameterFloat(juce::ParameterID { pid(i), 1 }, m.name, juce::NormalisableRange<float>(0.f, 1.f, 1.f / (m.steps - 1)), m.def,
                                        juce::AudioParameterFloatAttributes().withStringFromValueFunction([i](float v, int) {
                                          int s = (int)std::lround(v * (PARAMS[i].steps - 1));
                                          if (i == P_FX_TYPE) return juce::String(FX_NAMES[s]);
                                          if (i == P_MOOD) return juce::String(MOODS[s].name);
                                          if (i == P_TRACK) return juce::String(TRACKS[s].name);
                                          if (i == P_SCENE) return juce::String::charToString((juce::juce_wchar)('A' + s));
                                          if (i == P_KEYMODE) return juce::String(s == 0 ? "RAW" : s == 1 ? "SNAP" : "SHIFT");
                                          if (i == P_ST_MODE) return juce::String(s + 1);
                                          if (PARAMS[i].steps == 2) return juce::String(s ? "ON" : "OFF");
                                          return juce::String(s); }));
    else
      p = new juce::AudioParameterFloat(juce::ParameterID { pid(i), 1 }, m.name, juce::NormalisableRange<float>(0.f, 1.f), m.def,
                                        juce::AudioParameterFloatAttributes().withStringFromValueFunction([](float v, int) { return juce::String(juce::roundToInt(v * 100)) + "%"; }));
    addParameter(p); params.push_back(p);
    lastHost[i] = m.def; engOut[i] = eng->params[i];
  }
  const PresetBank& B = presets();
  for (int i = 0; i < B.n; i++) { presetNames.add(B.p[i].name); presetCats.add(B.p[i].cat); }
  formats.registerBasicFormats();
  startTimerHz(15);
}
AndromedaProcessor::~AndromedaProcessor() { stopTimer(); }

bool AndromedaProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
  if (l.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
  auto in = l.getChannelSet(true, 0);
  return in.isDisabled() || in == juce::AudioChannelSet::stereo() || in == juce::AudioChannelSet::mono();
}

void AndromedaProcessor::prepareToPlay(double rate, int block) {
  int r = 1; while (rate / r > 50000.0) r *= 2;
  const double er = rate / r;
  if (std::abs(er - sr) < 1 && r == ratio) return;
  const juce::SpinLock::ScopedLockType sl(lock);
  ratio = r; ph = 0; prevL = prevR = curL = curR = 0;
  std::vector<uint8_t> buf(1 << 16); int n = writeProject(eng->PJ, eng->params, buf.data(), (int)buf.size());
  eng->init((float)er); sr = er;
  Project P; if (readProject(P, eng->params, buf.data(), n)) { eng->PJ = P; eng->snapshotSeeds(); eng->syncSoundParams(); eng->projVersion++; }
  if (!fileL.empty()) { eng->fileL = fileL.data(); eng->fileR = fileR.empty() ? nullptr : fileR.data(); eng->fileLen = (int)fileL.size(); }
}

void AndromedaProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  juce::ScopedNoDenormals nd;
  const int n = buffer.getNumSamples();
  const juce::SpinLock::ScopedLockType sl(lock);
  Engine& E = *eng;
  // UI commands
  { int s1, n1, s2, n2; fifo.prepareToRead(fifo.getNumReady(), s1, n1, s2, n2);
    for (int k = 0; k < n1; k++) command(E, cmds[s1 + k].op, cmds[s1 + k].a);
    for (int k = 0; k < n2; k++) command(E, cmds[s2 + k].op, cmds[s2 + k].a);
    fifo.finishedRead(n1 + n2); }
  // host automation → engine (only what the host changed)
  for (int i = 0; i < P_COUNT; i++) { float v = params[(size_t)i]->getValue(); if (v != lastHost[i]) { lastHost[i] = v; E.setParam(i, v); } }
  // host transport: when the DAW plays, ANDROMEDA follows its tempo and bar position
  bool hp = false; double bpm = 120, ppq = 0;
  if (auto* ph = getPlayHead()) if (auto pos = ph->getPosition()) {
    hp = pos->getIsPlaying(); if (auto b = pos->getBpm()) bpm = *b; if (auto q = pos->getPpqPosition()) ppq = *q; }
  if (hp || hostWasPlaying) { E.hostSync = true; E.hostPlaying = hp; E.hostBpm = (float)bpm; if (hp && !hostWasPlaying) E.hostPpq = ppq; }
  else E.hostSync = false;
  if (hp) E.hostBpm = (float)bpm;
  hostWasPlaying = hp;

  // the engine runs at 44.1/48 kHz; at 88.2/96/176.4/192 kHz it runs at a half or a quarter and is interpolated up
  const bool hasIn = getBus(true, 0) && getBus(true, 0)->isEnabled() && getTotalNumInputChannels() > 0;
  const int ic = juce::jmax(1, juce::jmin(2, getTotalNumInputChannels()));
  float* L = buffer.getWritePointer(0); float* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;
  auto mit = midi.begin();
  for (int base = 0; base < n; base += CH) {
    const int len = juce::jmin(CH, n - base);
    // which output samples start a new engine sample
    int k = 0; { int p = ph; for (int j = 0; j < len; j++) { if (p == 0) k++; p = (p + 1) % ratio; } }
    if (hasIn) for (int q = 0, p = ph, j = 0; j < len; j++) { if (p == 0) { float sl = 0, sr2 = 0; int c = 0; for (int t = j; t < juce::jmin(len, j + ratio); t++, c++) { sl += buffer.getSample(0, base + t); sr2 += buffer.getSample(ic - 1, base + t); } lowInL[(size_t)q] = sl / (float)juce::jmax(1, c); lowInR[(size_t)q] = sr2 / (float)juce::jmax(1, c); q++; } p = (p + 1) % ratio; }
    // render k engine samples, splitting at MIDI events (sample accurate at the engine rate)
    int done = 0;
    auto run = [&](int upto) { if (upto > done) { E.process(lowL.data() + done, lowR.data() + done, hasIn ? lowInL.data() + done : nullptr, hasIn ? lowInR.data() + done : nullptr, upto - done); done = upto; } };
    for (; mit != midi.end(); ++mit) {
      const auto meta = *mit; const int sp = meta.samplePosition - base; if (sp >= len) break;
      int lowPos = 0; { int p = ph; for (int j = 0; j < juce::jmax(0, sp); j++) { if (p == 0) lowPos++; p = (p + 1) % ratio; } }
      run(juce::jmin(k, lowPos));
      const auto msg = meta.getMessage(); const uint8_t* d = msg.getRawData();
      if (msg.getRawDataSize() >= 2 && (d[0] & 0xF0) != 0xF0) E.midi(d[0], d[1], msg.getRawDataSize() > 2 ? d[2] : 0);
    }
    run(k);
    // write out (linear interpolation when ratio > 1)
    for (int j = 0, q = 0; j < len; j++) {
      if (ph == 0) { prevL = curL; prevR = curR; curL = lowL[(size_t)q]; curR = lowR[(size_t)q]; q++; }
      const float f = ratio == 1 ? 1.f : (float)(ph + 1) / (float)ratio;
      L[base + j] = prevL + (curL - prevL) * f; if (R) R[base + j] = prevR + (curR - prevR) * f;
      ph = (ph + 1) % ratio;
    }
  }
  for (int ch = 2; ch < buffer.getNumChannels(); ch++) buffer.clear(ch, 0, n);
  midi.clear();
  for (int i = 0; i < P_COUNT; i++) engOut[i].store(E.params[i], std::memory_order_relaxed);
}

// engine-side changes (moods, track select, pads, MIDI learn) show up in the DAW and on the KeyLab screen
void AndromedaProcessor::timerCallback() {
  for (int i = 0; i < P_COUNT; i++) {
    float e = engOut[i].load(std::memory_order_relaxed), h = params[(size_t)i]->getValue();
    if (std::abs(e - h) > 1e-4f && std::abs(e - lastHost[i]) > 1e-4f) {
      params[(size_t)i]->beginChangeGesture(); params[(size_t)i]->setValueNotifyingHost(e); params[(size_t)i]->endChangeGesture(); }
  }
}

void AndromedaProcessor::queueCmd(int op, const int* a) {
  int s1, n1, s2, n2; fifo.prepareToWrite(1, s1, n1, s2, n2);
  if (n1 + n2 == 0) return;
  Cmd& c = cmds[n1 ? s1 : s2]; c.op = op; for (int k = 0; k < 16; k++) c.a[k] = a[k];
  fifo.finishedWrite(1);
}
void AndromedaProcessor::setParamFromUI(int id, float v) {
  if (id < 0 || id >= P_COUNT) return;
  auto* p = params[(size_t)id]; p->beginChangeGesture(); p->setValueNotifyingHost(v); p->endChangeGesture();
}
juce::MemoryBlock AndromedaProcessor::getProjectBytes(std::vector<float>& outParams) {
  std::vector<uint8_t> buf(1 << 16); int n;
  { const juce::SpinLock::ScopedLockType sl(lock);
    // anything the host set since the last audio block goes into the engine first (track first: it decides where sound params land)
    auto apply = [&](int i) { float v = params[(size_t)i]->getValue(); if (v != lastHost[i]) { lastHost[i] = v; eng->setParam(i, v); } };
    apply(P_TRACK); for (int i = 0; i < P_COUNT; i++) if (i != P_TRACK) apply(i);
    n = writeProject(eng->PJ, eng->params, buf.data(), (int)buf.size()); outParams.assign(eng->params, eng->params + P_COUNT); }
  return juce::MemoryBlock(buf.data(), (size_t)n);
}
void AndromedaProcessor::setProjectBytes(const void* data, int len) {
  auto P = std::make_unique<Project>(); float tmp[P_COUNT];
  { const juce::SpinLock::ScopedLockType sl(lock); std::copy(eng->params, eng->params + P_COUNT, tmp); }
  if (!readProject(*P, tmp, (const uint8_t*)data, len)) return;
  { const juce::SpinLock::ScopedLockType sl(lock);
    eng->PJ = *P; std::copy(tmp, tmp + P_COUNT, eng->params);
    for (int i = 0; i < P_COUNT; i++) if (i != P_TRACK) eng->setParam(i, tmp[i]);
    for (int i = 0; i < P_COUNT; i++) { lastHost[i] = tmp[i]; engOut[i] = tmp[i]; }
    eng->snapshotSeeds(); eng->projVersion++; }
  for (int i = 0; i < P_COUNT; i++) if (std::abs(params[(size_t)i]->getValue() - tmp[i]) > 1e-5f) params[(size_t)i]->setValueNotifyingHost(tmp[i]);
}
void AndromedaProcessor::getStateInformation(juce::MemoryBlock& dest) { std::vector<float> p; dest = getProjectBytes(p); }
void AndromedaProcessor::setStateInformation(const void* data, int size) { setProjectBytes(data, size); }

bool AndromedaProcessor::loadAudioFile(const juce::File& f) {
  std::unique_ptr<juce::AudioFormatReader> r(formats.createReaderFor(f));
  if (!r) return false;
  const double rr = r->sampleRate / sr;
  const int srcLen = (int)juce::jmin<juce::int64>(r->lengthInSamples, (juce::int64)(r->sampleRate * 90));
  juce::AudioBuffer<float> src((int)juce::jmax(1u, r->numChannels > 1 ? 2u : 1u), srcLen);
  r->read(&src, 0, srcLen, 0, true, true);
  const int outLen = (int)(srcLen / rr);
  std::vector<float> nl((size_t)outLen), nr;
  { juce::LagrangeInterpolator li; li.process(rr, src.getReadPointer(0), nl.data(), outLen); }
  if (src.getNumChannels() > 1) { nr.resize((size_t)outLen); juce::LagrangeInterpolator li; li.process(rr, src.getReadPointer(1), nr.data(), outLen); }
  { const juce::SpinLock::ScopedLockType sl(lock);
    fileL.swap(nl); fileR.swap(nr);
    eng->fileL = fileL.data(); eng->fileR = fileR.empty() ? nullptr : fileR.data(); eng->fileLen = (int)fileL.size(); eng->filePos = 0; eng->filePlay = true; eng->fileGain = 1.f; }
  return true;   // old buffers (now in nl/nr) are freed here, outside the lock
}

void AndromedaProcessor::snapshot(std::vector<float>& tele, std::vector<float>& scope, std::vector<int>& ev) {
  const juce::SpinLock::ScopedLockType sl(lock);
  tele.assign(eng->tele, eng->tele + 48);
  scope.resize(1024); int w = eng->scopeW;
  for (int k = 0; k < 512; k++) { int i = (w - 512 + k + 1024) & 1023; scope[(size_t)k * 2] = eng->scope[i * 2]; scope[(size_t)k * 2 + 1] = eng->scope[i * 2 + 1]; }
  ev.clear(); int uw = eng->uiW; if (uw - uiRead > NUI) uiRead = uw - NUI;
  for (; uiRead < uw; uiRead++) { auto& e = eng->ui[uiRead % NUI]; ev.push_back(e.type); ev.push_back(e.a); ev.push_back(e.b); ev.push_back(e.c); }
}

juce::AudioProcessorEditor* AndromedaProcessor::createEditor() { return new AndromedaEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new AndromedaProcessor(); }
