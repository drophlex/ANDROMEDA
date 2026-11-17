#include "PluginEditor.h"
#include "BinaryData.h"
using namespace andro;
using Completion = juce::WebBrowserComponent::NativeFunctionCompletion;

AndromedaEditor::AndromedaEditor(AndromedaProcessor& p) : AudioProcessorEditor(p), proc(p) {
  web = std::make_unique<juce::WebBrowserComponent>(makeOptions());
  addAndMakeVisible(*web);
  setResizable(true, true);
  setResizeLimits(900, 620, 2400, 1600);
  setSize(1320, 860);
  web->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
  startTimerHz(30);
}
AndromedaEditor::~AndromedaEditor() { stopTimer(); }
void AndromedaEditor::resized() { web->setBounds(getLocalBounds()); }

std::optional<juce::WebBrowserComponent::Resource> AndromedaEditor::getResource(const juce::String& url) {
  if (url == "/" || url.startsWith("/index.html")) {
    std::vector<std::byte> data((size_t)BinaryData::index_htmlSize);
    std::memcpy(data.data(), BinaryData::index_html, data.size());
    return juce::WebBrowserComponent::Resource { std::move(data), "text/html; charset=utf-8" };
  }
  return std::nullopt;
}

juce::WebBrowserComponent::Options AndromedaEditor::makeOptions() {
  auto o = juce::WebBrowserComponent::Options {}
    .withNativeIntegrationEnabled()
    .withKeepPageLoadedWhenBrowserIsHidden()
    .withResourceProvider([this](const auto& url) { return getResource(url); })
    .withNativeFunction("cmd", [this](const juce::Array<juce::var>& a, Completion done) {
      int args[16] = {}; for (int k = 0; k < 16 && k + 1 < a.size(); k++) args[k] = (int)a[k + 1];
      if (a.size() > 0) proc.queueCmd((int)a[0], args); done(juce::var()); })
    .withNativeFunction("param", [this](const juce::Array<juce::var>& a, Completion done) {
      if (a.size() >= 2) proc.setParamFromUI((int)a[0], (float)(double)a[1]); done(juce::var()); })
    .withNativeFunction("getProject", [this](const juce::Array<juce::var>&, Completion done) {
      std::vector<float> ps; auto mb = proc.getProjectBytes(ps);
      juce::DynamicObject::Ptr obj = new juce::DynamicObject();
      obj->setProperty("bytes", juce::Base64::toBase64(mb.getData(), mb.getSize()));
      juce::Array<juce::var> pv; for (float v : ps) pv.add(v); obj->setProperty("params", pv);
      done(juce::var(obj.get())); })
    .withNativeFunction("setProject", [this](const juce::Array<juce::var>& a, Completion done) {
      juce::MemoryOutputStream mo; if (a.size() > 0 && juce::Base64::convertFromBase64(mo, a[0].toString())) proc.setProjectBytes(mo.getData(), (int)mo.getDataSize());
      done(juce::var()); })
    .withNativeFunction("presets", [this](const juce::Array<juce::var>&, Completion done) {
      juce::Array<juce::var> list;
      for (int i = 0; i < proc.presetNames.size(); i++) { juce::DynamicObject::Ptr o2 = new juce::DynamicObject(); o2->setProperty("n", proc.presetNames[i]); o2->setProperty("cat", proc.presetCats[i]); list.add(juce::var(o2.get())); }
      done(list); })
    .withNativeFunction("openUrl", [](const juce::Array<juce::var>& a, Completion done) {
      // only the donation page and the project's pages open outside the plugin (the page cannot navigate the plugin's view away)
      auto url = a.size() > 0 ? a[0].toString() : juce::String();
      if (url.startsWith("https://www.paypal.com/donate/") || url.startsWith("https://github.com/drophlex/") || url.startsWith("https://drophlex.github.io/"))
        juce::URL(url).launchInDefaultBrowser();
      done(juce::var()); })
    .withNativeFunction("openAudioFile", [this](const juce::Array<juce::var>&, Completion done) {
      chooser = std::make_unique<juce::FileChooser>("Audio for ANDROMEDA", juce::File::getSpecialLocation(juce::File::userMusicDirectory), "*.wav;*.aif;*.aiff;*.flac;*.mp3;*.m4a;*.ogg");
      chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this, done](const juce::FileChooser& fc) {
        auto f = fc.getResult();
        if (f.existsAsFile() && proc.loadAudioFile(f)) done(f.getFileName()); else done(juce::var()); });
    });
  return o;
}

// telemetry → page (same shape as the web worklet: tele[48], scope[1024 interleaved], ev[4n])
void AndromedaEditor::timerCallback() {
  proc.snapshot(tele, scope, ev);
  juce::DynamicObject::Ptr o = new juce::DynamicObject();
  juce::Array<juce::var> t, s, e;
  for (float v : tele) t.add(v);
  for (float v : scope) s.add(v);
  for (int v : ev) e.add(v);
  o->setProperty("tele", t); o->setProperty("scope", s); o->setProperty("ev", e);
  web->emitEventIfBrowserIsVisible("tele", juce::var(o.get()));
}
