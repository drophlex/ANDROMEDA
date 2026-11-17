#pragma once
#include "PluginProcessor.h"

// The editor is the same page as the web version, running in the system web view and calling the native engine.
class AndromedaEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit AndromedaEditor(AndromedaProcessor&);
  ~AndromedaEditor() override;
  void resized() override;
  void paint(juce::Graphics& g) override { g.fillAll(juce::Colour(0xff0d0b0a)); }
private:
  void timerCallback() override;
  juce::WebBrowserComponent::Options makeOptions();
  std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);
  AndromedaProcessor& proc;
  std::unique_ptr<juce::FileChooser> chooser;
  std::unique_ptr<juce::WebBrowserComponent> web;
  std::vector<float> tele, scope; std::vector<int> ev;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AndromedaEditor)
};
