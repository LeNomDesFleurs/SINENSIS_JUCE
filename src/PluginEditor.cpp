/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.hpp"

//==============================================================================

SinensisAudioProcessorEditor::SinensisAudioProcessorEditor(
    SinensisAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  apvts = &vts;

  for (auto* comp : getComps()) {
    addAndMakeVisible(comp);
    //   comp.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  }
  setButtonParameters();
  setSlidersParameters();
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->addListener(this);
  }
  startTimerHz(30);

  parametersChanged = false;
  ratio = 0.5;
  band_selector = 0;
  root_frequency = 100;

  setSize(300, 500);
}

SinensisAudioProcessorEditor::~SinensisAudioProcessorEditor() {
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->removeListener(this);
  }
}

void SinensisAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(57, 77, 91));

  g.setFont(juce::Font("Times New Roman", 60.0f, juce::Font::italic));
  // Title color
  g.setColour(juce::Colour(217, 74, 93));
  g.drawFittedText("Sinensis", 0, 0, getWidth(), 80,
                   juce::Justification::centred, 1);
}

void SinensisAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  bounds.reduce(30, 30);
  bounds.removeFromTop(80);
  cutoffFrequencySlider.setBounds(bounds.removeFromTop(80));
  bounds.removeFromTop(20);

  juce::Rectangle<int> buttonBounds = {100, 121, 170, 30};
  int temp_width = buttonBounds.getWidth() / 3;

  midiOffButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  midiMonoButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  midiPolyButton.setBounds(buttonBounds.removeFromLeft(temp_width));

  // midiMode.setBounds(bounds.removeFromLeft(300));
  // title.setBounds(bounds.removeFromTop(100));
  // bandControl.setBounds(bounds.removeFromRight(600));
  // Q.setBounds(bounds);
}

std::vector<juce::Component*> SinensisAudioProcessorEditor::getComps() {
  return {&attackSlider,       &decaySlider,   &cutoffFrequencySlider,
          &BandSelectorSlider, &ratioSlider,   &lowHighButton,
          &oddEvenButton,      &peakButton,    &midiOffButton,
          &midiMonoButton,     &midiPolyButton};
}

std::vector<juce::Slider*> SinensisAudioProcessorEditor::getSliderComps() {
  return {&attackSlider, &decaySlider, &cutoffFrequencySlider,
          &BandSelectorSlider, &ratioSlider};
}

std::vector<juce::Button*> SinensisAudioProcessorEditor::getButtonComps() {
  return {&lowHighButton, &oddEvenButton,  &peakButton,
          &midiOffButton, &midiMonoButton, &midiPolyButton};
}

void SinensisAudioProcessorEditor::setButtonParameters() {
  for (auto* button : getButtonComps()) {
    button->setClickingTogglesState(true);
    button->setLookAndFeel(&otherLookAndFeel);
  }
  midiOffButton.setRadioGroupId(1);
  midiMonoButton.setRadioGroupId(1);
  midiPolyButton.setRadioGroupId(1);

  lowHighButton.setRadioGroupId(2);
  oddEvenButton.setRadioGroupId(2);
  peakButton.setRadioGroupId(2);

  juce::Array<juce::Button*> midi_modes_buttons;
  midi_modes_buttons.add(&midiOffButton);
  midi_modes_buttons.add(&midiMonoButton);
  midi_modes_buttons.add(&midiPolyButton);

  juce::Array<juce::Button*> band_modes_buttons;
  band_modes_buttons.add(&lowHighButton);
  band_modes_buttons.add(&oddEvenButton);
  band_modes_buttons.add(&peakButton);

  // lowHighButtonAttachement.reset(new
  // juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, "LOWHIGH",
  // lowHighButton)); oddEvenButtonAttachement.reset(new
  // juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, "ODDEVEN",
  // oddEvenButton)); peakButtonAttachement.reset(new
  // juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, "PEAK",
  // peakButton));

  // midiOffButtonAttachement.reset(new
  // juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, "MIDIOFF",
  // midiOffButton)); midiMonoButtonAttachement.reset(new
  // juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, "MIDIMONO",
  // midiMonoButton)); midiPolyButtonAttachement.reset(new
  // juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, "MIDIPOLY",
  // midiPolyButton));

  midi_modes_radio_group = std::make_unique<RadioButtonAttachment>(
      *apvts->getParameter("MIDIMODE"), midi_modes_buttons, "MIDIMODE", 1);
  band_modes_radio_group = std::make_unique<RadioButtonAttachment>(
      *apvts->getParameter("BANDMODE"), band_modes_buttons, "BANDMODE", 2);
}

void SinensisAudioProcessorEditor::setSlidersParameters() {
  for (auto* slider : getSliderComps()) {
    slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0,
                            slider->getTextBoxHeight());
  }

  QSlider.setLookAndFeel(&resonance_look_and_feel);
  ratioSlider.setLookAndFeel(&ratio_look_and_feel);
  BandSelectorSlider.setLookAndFeel(&band_select_look_and_feel);
  attackSlider.setLookAndFeel(&envelope_look_and_feel);
  decaySlider.setLookAndFeel(&envelope_look_and_feel);
  cutoffFrequencySlider.setLookAndFeel(&envelope_look_and_feel);

  QAttachement.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
      *apvts, "RESONANCE", QSlider));
  ratioAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(*apvts, "RATIO",
                                                               ratioSlider));
  BandSelectorAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(
          *apvts, "band_selector", BandSelectorSlider));
  attackAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(*apvts, "ATTACK",
                                                               attackSlider));
  decayAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(*apvts, "DECAY",
                                                               decaySlider));
  cutoffFrequencyAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(
          *apvts, "root_frequency", cutoffFrequencySlider));
}

void SinensisAudioProcessorEditor::parameterValueChanged(int parameterIndex,
                                                         float newValue) {
  parametersChanged.set(true);
  switch (parameterIndex) {
    case 2:
      ratio = newValue * 10 + 4;
      break;
    case 1:
      band_selector = newValue * 1.5;
      break;
    case 4:
      root_frequency = newValue > 0.5;
      break;
  }
}

void SinensisAudioProcessorEditor::timerCallback() {
  if (parametersChanged.compareAndSetBool(false, true)) {
  }
  repaint();
}
