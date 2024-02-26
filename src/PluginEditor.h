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
#include "BandControl.h"
#include "Title.h"
#include "MidiControl.h"
#include "Q.h"

//==============================================================================
/**
*/
class SinensisAudioProcessorEditor  :   public juce::AudioProcessorEditor, 
                                        public juce::Button::Listener
{
public:
    SinensisAudioProcessorEditor(SinensisAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    //SinensisAudioProcessorEditor (SinensisAudioProcessor&);
    ~SinensisAudioProcessorEditor() override;

    //==============================================================================
    // void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

   private:

    OtherLookAndFeel otherLookAndFeel; // [2]
    SinensisAudioProcessor& audioProcessor;

    QComponent Q { audioProcessor.parameters };
    BandControl bandControl{ audioProcessor.parameters };
    MidiControl midiMode{ audioProcessor.parameters };
    Title title;


    juce::Slider outputVolumeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>outputVolumeAttachement;
    juce::Label outputVolumeLabel;


    juce::TextButton checkTheTimeButton;
    juce::Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SinensisAudioProcessorEditor)
};


