/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CustomColors.hpp"
#include "GraphicTools.hpp"
#include "LookAndFeel.hpp"
#include "PlaceHolder.hpp"
#include "PluginProcessor.hpp"
#include "RadioButtonAttachment.hpp"

//==============================================================================
/**
 */

class SinensisAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      public juce::AudioProcessorParameter::Listener,
      public juce::Timer {
 public:
  SinensisAudioProcessorEditor(SinensisAudioProcessor&,
                               juce::AudioProcessorValueTreeState& vts);
  // SinensisAudioProcessorEditor (SinensisAudioProcessor&);
  ~SinensisAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;
  void parameterValueChanged(int parameterIndex, float newvalue) override;
  void parameterGestureChanged(int parameterIndex,
                               bool gestureIsStarting) override {}
  void timerCallback() override;
  std::vector<juce::Component*> getComps();
  std::vector<juce::Button*> getButtonComps();
  std::vector<juce::Slider*> getSliderComps();
  void setButtonParameters();
  void setSlidersParameters();
  juce::Path generateBackground(int size_x, int size_y, int block_size);
  void addLine(juce::Path& path, int initial_x, int initial_y,
               int destination_x, int destination_y);
  void drawBandSelectionWidget(Graphics& g);
  void drawFrequencyWidget(juce::Graphics& g);
  juce::Colour getGradient(float value);
  void updateGains();
  void computeLowHigh();
  void computeOddEven();
  void computePeak();

 private:
  OtherLookAndFeel otherLookAndFeel;  // [2]
  RatioLookAndFeel ratio_look_and_feel;
  ResonanceLookAndFeel resonance_look_and_feel;
  BandSelectLookAndFeel band_select_look_and_feel;
  EnvelopeLookAndFeel envelope_look_and_feel;

  SinensisAudioProcessor& audioProcessor;

  AudioProcessorValueTreeState* apvts;

  // Background
  //   juce::Component background;

  // SLIDER

  juce::Slider BandSelectorSlider, ratioSlider, QSlider, cutoffFrequencySlider,
      attackSlider, decaySlider;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      BandSelectorAttachement, ratioAttachement, cutoffFrequencyAttachement,
      attackAttachement, decayAttachement, QAttachement;

  // BUTTON
  juce::ToggleButton lowHighButton{"L | H"}, oddEvenButton{"O | E"},
      peakButton{"Peak"};
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      lowHighButtonAttachement, oddEvenButtonAttachement, peakButtonAttachement;

  // BUTTON
  juce::ToggleButton midiOffButton{"Off"}, midiMonoButton{"Mono"},
      midiPolyButton{"Poly"};
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      midiOffButtonAttachement, midiMonoButtonAttachement,
      midiPolyButtonAttachement;

  juce::Slider outputVolumeSlider;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      outputVolumeAttachement;

  std::unique_ptr<RadioButtonAttachment> midi_modes_radio_group;
  std::unique_ptr<RadioButtonAttachment> band_modes_radio_group;

  juce::Atomic<bool> parametersChanged;
  float ratio;
  float band_selector;
  float root_frequency;
  bool background_generated = false;
  std::array<float, 6> gains;
  Sinensis::BandMode band_selector_mode;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SinensisAudioProcessorEditor)
  };
