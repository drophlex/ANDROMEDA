#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "api.h"

// ANDROMEDA plugin: wraps the same header-only engine the web version runs as WebAssembly.
class AndromedaProcessor : public juce::AudioProcessor, private juce::Timer {
public:
  AndromedaProcessor();
  ~AndromedaProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout&) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  using AudioProcessor::processBlock;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }
  const juce::String getName() const override { return "ANDROMEDA"; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 12.0; }
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return "ANDROMEDA"; }
  void changeProgramName(int, const juce::String&) override {}
  void getStateInformation(juce::MemoryBlock&) override;
  void setStateInformation(const void*, int) override;

  // ---- called from the editor (message thread)
  void queueCmd(int op, const int* a);
  void setParamFromUI(int id, float v);
  juce::MemoryBlock getProjectBytes(std::vector<float>& params);
  void setProjectBytes(const void* data, int len);
  juce::StringArray presetNames, presetCats;
  bool loadAudioFile(const juce::File&);
  juce::File renderLoop();                       // message thread: the current scene as a 4-bar WAV in a temporary folder
  // telemetry snapshot for the UI
  void snapshot(std::vector<float>& tele, std::vector<float>& scope, std::vector<int>& ev);

  std::vector<juce::RangedAudioParameter*> params;

private:
  void timerCallback() override;
  std::unique_ptr<andro::Engine> eng;
  juce::SpinLock lock;            // guards engine state between audio and message threads (held briefly)
  double sr = 0;
  float lastHost[andro::P_COUNT];
  std::atomic<float> engOut[andro::P_COUNT];
  struct Cmd { int op; int a[16]; };
  juce::AbstractFifo fifo { 256 }; Cmd cmds[256];
  bool hostWasPlaying = false;
  int uiRead = 0;
  std::vector<float> fileL, fileR, pendL, pendR;
  juce::AudioFormatManager formats;
  static constexpr int CH = 2048;            // output samples per inner chunk
  int ratio = 1, ph = 0; float prevL = 0, prevR = 0, curL = 0, curR = 0;
  std::vector<float> lowL = std::vector<float>(CH + 4), lowR = std::vector<float>(CH + 4), lowInL = std::vector<float>(CH + 4), lowInR = std::vector<float>(CH + 4);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AndromedaProcessor)
};
