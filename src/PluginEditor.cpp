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

  addAndMakeVisible(background_component);
  background_component.setBufferedToImage(true);

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
  root_frequency = 0.5;

  setSize(300, 540);
}

SinensisAudioProcessorEditor::~SinensisAudioProcessorEditor() {
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->removeListener(this);
  }
}

void SinensisAudioProcessorEditor::paintOverChildren(juce::Graphics& g) {
  // if (!background_generated) {
  if (midi_mode == Sinensis::MidiMode::Off) {
    drawFrequencyWidget(g);
  }
  drawBandSelectionWidget(g);

  const auto svg = Drawable::createFromImageData(BinaryData::NOI_svg,
                                                 BinaryData::NOI_svgSize);

  // juce::AffineTransform scale = Set::scale(0.2);
  juce::Rectangle<float> position = {140.f, 495.f, 35.f, 35.f};
  juce::RectanglePlacement placement = (36);
  svg->setTransformToFit(position, placement);
  svg->draw(g, 1.0);

  // draw amp
}

void SinensisAudioProcessorEditor::resized() {
  background_component.setBounds(0, 0, 300, 540);
  auto bounds = getLocalBounds();
  bounds.reduce(30, 30);
  bounds.removeFromTop(80);
  cutoffFrequencySlider.setBounds(bounds.removeFromTop(80));
  bounds.removeFromTop(20);

  juce::Rectangle<int> buttonBounds = {70, 60, 170, 30};
  int temp_width = buttonBounds.getWidth() / 3;

  midiOffButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  midiMonoButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  midiPolyButton.setBounds(buttonBounds.removeFromLeft(temp_width));

  // buttonBounds = {25, 355, 250, 30};
  buttonBounds = {70, 455, 170, 30};
  temp_width = buttonBounds.getWidth() / 3;

  lowHighButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  oddEvenButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  peakButton.setBounds(buttonBounds.removeFromLeft(temp_width));

  cutoffFrequencySlider.setBounds(20, 90, 260, 80);
  BandSelectorSlider.setBounds(25, 375, 250, 83);
  QSlider.setBounds(25, 180, 250, 80);
  ratioSlider.setBounds(25, 290, 250, 60);

  attackSlider.setBounds(45, 95, 70, 70);
  decaySlider.setBounds(180, 95, 70, 70);
  // midiMode.setBounds(bounds.removeFromLeft(300));
  // title.setBounds(bounds.removeFromTop(100));
  // bandControl.setBounds(bounds.removeFromRight(600));
  // Q.setBounds(bounds);
}

std::vector<juce::Component*> SinensisAudioProcessorEditor::getComps() {
  return {&attackSlider,  &decaySlider,        &cutoffFrequencySlider,
          &QSlider,       &BandSelectorSlider, &ratioSlider,
          &lowHighButton, &oddEvenButton,      &peakButton,
          &midiOffButton, &midiMonoButton,     &midiPolyButton};
}

std::vector<juce::Slider*> SinensisAudioProcessorEditor::getSliderComps() {
  return {&attackSlider,       &decaySlider, &cutoffFrequencySlider,
          &BandSelectorSlider, &ratioSlider, &QSlider};
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
  band_modes_buttons.add(&oddEvenButton);
  band_modes_buttons.add(&lowHighButton);
  band_modes_buttons.add(&peakButton);

  midi_modes_radio_group = std::make_unique<RadioButtonAttachment>(
      *apvts->getParameter("MIDIMODE"), midi_modes_buttons, "MIDIMODE", 1);
  band_modes_radio_group = std::make_unique<RadioButtonAttachment>(
      *apvts->getParameter("BANDMODE"), band_modes_buttons, "BANDMODE", 2);
}

void SinensisAudioProcessorEditor::setSlidersParameters() {
  for (auto* slider : getSliderComps()) {
    slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0,
                            slider->getTextBoxHeight());
  }

  QSlider.setLookAndFeel(&resonance_look_and_feel);
  ratioSlider.setLookAndFeel(&ratio_look_and_feel);
  // TODO changer quand il sera temps
  BandSelectorSlider.setLookAndFeel(&empty_look_and_feel);
  attackSlider.setLookAndFeel(&attack_look_and_feel);
  decaySlider.setLookAndFeel(&decay_look_and_feel);
  cutoffFrequencySlider.setLookAndFeel(&empty_look_and_feel);

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
    case 0:
      attackSlider.setVisible(*apvts->getRawParameterValue("BANDMODE"));
      if (newValue) band_selector_mode = Sinensis::BandMode::LowHigh;
      break;
    // case 1:
    //   if (newValue) band_selector_mode = Sinensis::BandMode::OddEven;
    //   break;
    case 2:
      root_frequency = newValue;
      break;
    case 5:
      band_selector = newValue;
      break;
    case 4:
      // root_frequency = newValue;
      break;
  }
}

void SinensisAudioProcessorEditor::timerCallback() {
  if (parametersChanged.compareAndSetBool(false, true)) {
    repaint();
  }
}

void SinensisAudioProcessorEditor::drawBandSelectionWidget(Graphics& g) {
  float marge_basse = 450;
  float taille = 70;
  float x_position = 50;
  float ecart = 40;
  juce::Point<float> point_bas, point_haut;
  updateGains();
  for (int i = 0; i < 6; i++) {
    juce::Path lines;
    juce::Path lines_background;
    // draw slider background
    point_bas = {x_position, marge_basse};
    point_haut = {x_position, marge_basse - (taille)};
    lines_background.startNewSubPath(point_bas);
    lines_background.lineTo(point_haut);

    // draw slider value
    // float y_position = (marge_basse + 1) - ((taille * gains[i]) - 1);
    float y_position = (marge_basse) - ((taille * gains[i]));
    point_haut = {x_position, y_position};
    lines.startNewSubPath(point_bas);
    lines.lineTo(point_haut);
    x_position += ecart;

    g.setColour(CustomColors::black);
    g.strokePath(lines_background,
                 {5, PathStrokeType::curved, PathStrokeType::rounded});

    g.setColour(CustomColors::getGradient(gains[i]));
    g.strokePath(lines, {5, PathStrokeType::curved, PathStrokeType::rounded});
  }
}

void SinensisAudioProcessorEditor::updateGains() {
  int band_mode = static_cast<int>(*apvts->getRawParameterValue("BANDMODE"));

  switch (band_mode) {
    case 0:
      band_selector_mode = Sinensis::BandMode::OddEven;
      break;
    case 1:
      band_selector_mode = Sinensis::BandMode::LowHigh;
      break;
    case 2:
      band_selector_mode = Sinensis::BandMode::Peak;
      break;
  }

  int i_midi_mode = static_cast<int>(*apvts->getRawParameterValue("MIDIMODE"));

  switch (i_midi_mode) {
    case 0:
      midi_mode = Sinensis::MidiMode::Off;
      break;
    case 1:
      midi_mode = Sinensis::MidiMode::Mono;
      break;
    case 2:
      midi_mode = Sinensis::MidiMode::Poly;
      break;
  }

  if (midi_mode == Sinensis::MidiMode::Off) {
    attackSlider.setVisible(false);
    decaySlider.setVisible(false);
    cutoffFrequencySlider.setVisible(true);
  } else {
    attackSlider.setVisible(true);
    decaySlider.setVisible(true);
    cutoffFrequencySlider.setVisible(false);
  }

  switch (band_selector_mode) {
    case Sinensis::LowHigh:
      computeLowHigh();
      break;
    case Sinensis::OddEven:
      computeOddEven();
      break;
    case Sinensis::Peak:
      computePeak();
      break;
  }
}

void SinensisAudioProcessorEditor::computeLowHigh() {
  float alpha = band_selector * 4.0f - 2.0f;
  float beta = -2.0f * (band_selector * band_selector) + 1.0f;

  for (int i = 0; i < 6; i++) {
    float this_band_index = static_cast<float>(i);
    this_band_index /= 5.0f;
    float this_band_gain = this_band_index * alpha + beta;
    if (this_band_gain < 0.0f) {
      this_band_gain = 0.0f;
    }
    gains[i] = this_band_gain;
  }
}

void SinensisAudioProcessorEditor::computeOddEven() {
  for (int i = 0; i < 6; i++) {
    float this_band_gain = 0;
    // odd case
    if (i == 0 || i == 2 || i == 4) {
      this_band_gain = band_selector;
    }
    // even case
    if (i == 1 || i == 3 || i == 5) {
      this_band_gain = 1.0f - band_selector;
    }
    gains[i] = this_band_gain;
  }
}

void SinensisAudioProcessorEditor::computePeak() {
  float cursor = band_selector;
  cursor *= 5.;
  for (int i = 0; i < 6; i++) {
    float band_index = static_cast<float>(i);
    float band_gain = 0.0f;
    if (cursor < band_index) {
      band_gain = band_index - cursor;
    } else {
      band_gain = cursor - band_index;
    }

    band_gain = 1 - band_gain;
    if (band_gain > 1.) {
      band_gain = 1.;
    }
    if (band_gain < 0.) {
      band_gain = 0.;
    }
    gains[i] = band_gain;
  }
}

void SinensisAudioProcessorEditor::drawFrequencyWidget(juce::Graphics& g) {
  float x_position = 28;
  float y_position = 95;
  float width = 240;
  float height = 70;
  juce::Path lines;

  GraphicTools::addLine(lines, x_position + (width * root_frequency),
                        y_position, x_position + (width * root_frequency),
                        y_position + height);

  g.setColour(CustomColors::getGradientWithoutGreen(root_frequency));
  // g.setColour(CustomColors::red);
  g.strokePath(lines, {5, PathStrokeType::curved, PathStrokeType::rounded});
}