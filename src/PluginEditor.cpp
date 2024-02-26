/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

SinensisAudioProcessorEditor::SinensisAudioProcessorEditor(
    SinensisAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    const int text_box_width = 50;
    /*
    //------------------------------------------------------
    addAndMakeVisible(outputVolumeSlider);
    outputVolumeSlider.setLookAndFeel(&otherLookAndFeel);
    outputVolumeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outputVolumeAttachement.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment
        (vts, "OUTPUTVOLUME", outputVolumeSlider));
    outputVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, text_box_width, outputVolumeSlider.getTextBoxHeight());

    //FrequencyStuff.setVisible(*vts.getRawParameterValue("MIDIMODE") == 0);
    addAndMakeVisible(outputVolumeLabel);
    outputVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    outputVolumeLabel.setText("Out Volume",
        juce::dontSendNotification);

    //--------------------------------------------------------
    */

   addAndMakeVisible (checkTheTimeButton);
        checkTheTimeButton.setButtonText ("Check the time...");
        checkTheTimeButton.addListener (this); // [7]
 
        addAndMakeVisible (timeLabel);
        timeLabel.setColour (juce::Label::backgroundColourId, juce::Colours::black);
        timeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
        timeLabel.setJustificationType (juce::Justification::centred);
 
        setSize (600, 110);
        
// addAndMakeVisible(title);
// addAndMakeVisible(bandControl);
// addAndMakeVisible(Q);
// addAndMakeVisible(midiMode);

    // setSize(1200, 400);
}

SinensisAudioProcessorEditor::~SinensisAudioProcessorEditor()
{
}

//==============================================================================
// void SinensisAudioProcessorEditor::paint (juce::Graphics& g)
// {
//     // (Our component is opaque, so we must completely fill the background with a solid colour)
//    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

//     // g.fillAll(juce::Colours::grey);
//     // g.setColour (juce::Colours::black);
//     //g.setFont (15.0f);
//    //g.drawFittedText("Sinensis", 0, 0, getWidth(), 30, juce::Justification::centred, 1);

// }

void SinensisAudioProcessorEditor::resized()
{
    const int marge_haute_slider = 60;

    auto bounds = getLocalBounds();

    bounds.reduce(20, 20);

    timeLabel.setBounds(bounds.removeFromLeft(50));
    checkTheTimeButton.setBounds(bounds);

    // midiMode.setBounds(bounds.removeFromLeft(300));
    // title.setBounds(bounds.removeFromTop(100));
    // bandControl.setBounds(bounds.removeFromRight(600));
    // Q.setBounds(bounds);

}


void SinensisAudioProcessorEditor::buttonClicked (juce::Button* button)  // [2]
{
    if (button == &checkTheTimeButton)                                                      // [3]
    {
        auto currentTime = juce::Time::getCurrentTime();                                          // [4]

        auto includeDate = true;
        auto includeTime = true;
        auto currentTimeString = currentTime.toString (includeDate, includeTime);           // [5]

        timeLabel.setText (currentTimeString, juce::dontSendNotification);                        // [6]
    }
}

