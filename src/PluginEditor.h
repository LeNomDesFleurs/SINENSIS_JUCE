/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "PlaceHolder.h"
#include "RadioButtonAttachment.hpp"

//==============================================================================
/**
*/
class SinensisAudioProcessorEditor  :   public juce::AudioProcessorEditor, 
                                        public juce::AudioProcessorParameter::Listener,
                                        public juce::Timer
{
public:
    SinensisAudioProcessorEditor(SinensisAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    //SinensisAudioProcessorEditor (SinensisAudioProcessor&);
    ~SinensisAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
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

   private:

    OtherLookAndFeel otherLookAndFeel; // [2]
    RatioLookAndFeel ratio_look_and_feel;
    ResonanceLookAndFeel resonance_look_and_feel;
    BandSelectLookAndFeel band_select_look_and_feel;
    EnvelopeLookAndFeel envelope_look_and_feel;

    SinensisAudioProcessor& audioProcessor;

    AudioProcessorValueTreeState* apvts;

    //SLIDER
 
    juce::Slider BandSelectorSlider, ratioSlider, QSlider, cutoffFrequencySlider, attackSlider, decaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        BandSelectorAttachement, ratioAttachement, cutoffFrequencyAttachement, attackAttachement, decayAttachement, QAttachement;

    //BUTTON
    juce::TextButton lowHighButton{ "Low | High" }, oddEvenButton{ "Odd | Even" }, peakButton{ "Peak" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        lowHighButtonAttachement, oddEvenButtonAttachement, peakButtonAttachement;
    
    //BUTTON
    juce::TextButton midiOffButton{ "Off" }, midiMonoButton{ "Mono" }, midiPolyButton{ "Poly" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        midiOffButtonAttachement, midiMonoButtonAttachement, midiPolyButtonAttachement;

    juce::Slider outputVolumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>outputVolumeAttachement;



  std::unique_ptr<RadioButtonAttachment> midi_modes_radio_group;
  std::unique_ptr<RadioButtonAttachment> band_modes_radio_group;

  juce::Atomic<bool> parametersChanged;  
  juce::Atomic<float> ratio;
  juce::Atomic<float> band_selector;
  juce::Atomic<float> root_frequency;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SinensisAudioProcessorEditor)
};


