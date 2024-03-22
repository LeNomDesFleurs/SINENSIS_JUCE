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
  // if (!background_generated) {
  g.fillAll(CustomColors::blue);
  g.setColour(juce::Colours::black);
  // g.strokePath(generateBackground(300, 500, 10),
  //              {2, PathStrokeType::curved, PathStrokeType::rounded});
  background_generated = true;
  // }

  g.setColour(CustomColors::red);
  g.setFont(juce::Font("Times New Roman", 60.0f, juce::Font::italic));
  g.drawFittedText("Sinensis", 0, 0, getWidth(), 80,
                   juce::Justification::centred, 1);
  // Title color

  // Rounded rectangles background

  g.setColour(CustomColors::green);
  // Frequency rectangle
  g.fillRoundedRectangle(20, 90, 260, 100, 5);
  // Band Selection rectangle
  g.fillRoundedRectangle(20, 370, 260, 100, 5);

  drawBandSelectionWidget(g);

  // draw amp
}

void SinensisAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  bounds.reduce(30, 30);
  bounds.removeFromTop(80);
  cutoffFrequencySlider.setBounds(bounds.removeFromTop(80));
  bounds.removeFromTop(20);

  juce::Rectangle<int> buttonBounds = {70, 75, 170, 30};
  int temp_width = buttonBounds.getWidth() / 3;

  midiOffButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  midiMonoButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  midiPolyButton.setBounds(buttonBounds.removeFromLeft(temp_width));

  buttonBounds = {25, 355, 250, 30};
  buttonBounds = {70, 355, 170, 30};
  temp_width = buttonBounds.getWidth() / 3;

  lowHighButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  oddEvenButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  peakButton.setBounds(buttonBounds.removeFromLeft(temp_width));

  QSlider.setBounds(25, 150, 250, 83);
  BandSelectorSlider.setBounds(30, 380, 250, 83);

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
    slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0,
                            slider->getTextBoxHeight());
  }

  QSlider.setLookAndFeel(&resonance_look_and_feel);
  ratioSlider.setLookAndFeel(&ratio_look_and_feel);
  // TODO changer quand il sera temps
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
    // case 0:
    //   if (newValue) band_selector_mode = Sinensis::BandMode::LowHigh;
    //   break;
    // case 1:
    //   if (newValue) band_selector_mode = Sinensis::BandMode::OddEven;
    //   break;
    case 2:
      ratio = newValue * 10 + 4;
      break;
    case 5:
      band_selector = newValue;
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

juce::Path SinensisAudioProcessorEditor::generateBackground(int size_x,
                                                            int size_y,
                                                            int block_size) {
  juce::Path motif;
  for (int i = 0; i < size_x; i += block_size) {
    for (int j = 0; j < size_y; j += block_size) {
      if (true) {
        addLine(motif, i + block_size, j, i, j + block_size);
      } else {
        addLine(motif, i, j, i + block_size, j + block_size);
      }
    }
  }
  return motif;
}

void SinensisAudioProcessorEditor::addLine(juce::Path& path, int initial_x,
                                           int initial_y, int destination_x,
                                           int destination_y) {
  float f_initial_x, f_initial_y, f_destination_x, f_destination_y;
  f_initial_x = static_cast<float>(initial_x);
  f_initial_y = static_cast<float>(initial_y);
  f_destination_x = static_cast<float>(destination_x);
  f_destination_y = static_cast<float>(destination_y);

  juce::Point<float> begin{f_initial_x, f_initial_y};
  juce::Point<float> end{f_destination_x, f_destination_y};

  path.startNewSubPath(begin);
  path.lineTo(end);
}

void SinensisAudioProcessorEditor::drawBandSelectionWidget(Graphics& g) {
  float marge_basse = 460;
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
    float y_position = (marge_basse + 1) - ((taille * gains[i]) - 1);
    point_haut = {x_position, y_position};
    lines.startNewSubPath(point_bas);
    lines.lineTo(point_haut);
    x_position += ecart;

    g.setColour(juce::Colours::black);
    g.strokePath(lines_background,
                 {5, PathStrokeType::curved, PathStrokeType::rounded});

    g.setColour(getGradient(gains[i]));
    g.strokePath(lines, {5, PathStrokeType::curved, PathStrokeType::rounded});
  }
}

/// @brief return a gradient that goes from custom green to custom red by going
/// trought yellow and orange
/// @param value
/// @return
juce::Colour SinensisAudioProcessorEditor::getGradient(float value) {
  if (value < 0.33) {
    value *= 3;
    return CustomColors::green.interpolatedWith(CustomColors::yellow, value);
  } else if (value > 0.33 && value < 0.66) {
    value -= 0.33;
    value *= 3;
    return CustomColors::yellow.interpolatedWith(CustomColors::orange, value);
  } else if (value > 0.66) {
    value -= 0.66;
    value *= 3;
    return CustomColors::orange.interpolatedWith(CustomColors::red, value);
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