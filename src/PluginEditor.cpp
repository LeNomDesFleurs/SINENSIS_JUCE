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

  editorSinensis.m_parameters.band_selector = 0.3;
  editorSinensis.computeGain();

  setSize(300, 600);
}

SinensisAudioProcessorEditor::~SinensisAudioProcessorEditor() {
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->removeListener(this);
  }
}

void SinensisAudioProcessorEditor::paintOverChildren(juce::Graphics& g) {
  // if (!background_generated) {

  editorSinensis.updateFrequencies(
      *apvts->getRawParameterValue("RATIO"),
      *apvts->getRawParameterValue("root_frequency"), note_lock);

  editorSinensis.updateBandGain(
    static_cast<Sinensis::BandMode>((int)*apvts->getRawParameterValue("BANDMODE")), 
    *apvts->getRawParameterValue("band_selector"));

  if (midi_mode) {
    attackSlider.setVisible(true);
    decaySlider.setVisible(true);
    cutoffFrequencySlider.setVisible(false);
  } else {
    drawFrequencyWidget(g);
    attackSlider.setVisible(false);
    decaySlider.setVisible(false);
    cutoffFrequencySlider.setVisible(true);
  }
  // update all Sinensis variable
  drawBandSelectionWidget(g);

  // Draw Logo
  const auto svg = Drawable::createFromImageData(BinaryData::NOI_svg,
                                                 BinaryData::NOI_svgSize);

  // juce::AffineTransform scale = Set::scale(0.2);
  juce::Rectangle<float> position = {150.f - 36 / 2, 550.f, 36.f, 36.f};
  juce::RectanglePlacement placement = (36);
  svg->setTransformToFit(position, placement);
  svg->draw(g, 1.0);

  // draw amp
}

void SinensisAudioProcessorEditor::resized() {
  background_component.setBounds(getBounds());
  auto bounds = getLocalBounds();
  bounds.reduce(30, 30);
  bounds.removeFromTop(80);
  cutoffFrequencySlider.setBounds(bounds.removeFromTop(80));
  bounds.removeFromTop(20);

  juce::Rectangle<int> buttonBounds = {122, 60, 56, 30};
  int temp_width = buttonBounds.getWidth() / 3;

  midiPolyButton.setBounds(buttonBounds);

  // buttonBounds = {25, 355, 250, 30};
  buttonBounds = {65, 505, 170, 30};
  temp_width = buttonBounds.getWidth() / 3;

  lowHighButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  oddEvenButton.setBounds(buttonBounds.removeFromLeft(temp_width));
  peakButton.setBounds(buttonBounds.removeFromLeft(temp_width));

  cutoffFrequencySlider.setBounds(20, 90, 260, 80);
  BandSelectorSlider.setBounds(20, 425, 260, 83);
  QSlider.setBounds(24, 180, 117, 80);
  ratioSlider.setBounds(25, 330, 250, 70);
  attackSlider.setBounds(45, 95, 70, 70);
  decaySlider.setBounds(180, 95, 70, 70);
  dryWetSlider.setBounds(25, 280, 260, 40);

  float button_width = 13;
  float button_height = 16;
  float x_position = 173;
  float y_position = 225;

  cButton.setBounds(x_position, y_position, button_width, button_height);
  x_position += button_width;
  cSharpButton.setBounds(x_position - (button_width / 2),
                         y_position - button_height, button_width,
                         button_height);
  dButton.setBounds(x_position, y_position, button_width, button_height);
  x_position += button_width;
  dSharpButton.setBounds(x_position - (button_width / 2),
                         y_position - button_height, button_width,
                         button_height);
  eButton.setBounds(x_position, y_position, button_width, button_height);
  x_position += button_width;
  fButton.setBounds(x_position, y_position, button_width, button_height);
  x_position += button_width;
  fSharpButton.setBounds(x_position - (button_width / 2),
                         y_position - button_height, button_width,
                         button_height);
  gButton.setBounds(x_position, y_position, button_width, button_height);
  x_position += button_width;
  gSharpButton.setBounds(x_position - (button_width / 2),
                         y_position - button_height, button_width,
                         button_height);
  aButton.setBounds(x_position, y_position, button_width, button_height);
  x_position += button_width;
  aSharpButton.setBounds(x_position - (button_width / 2),
                         y_position - button_height, button_width,
                         button_height);
  bButton.setBounds(x_position, y_position, button_width, button_height);
  // midiMode.setBounds(bounds.removeFromLeft(300));
  // title.setBounds(bounds.removeFromTop(100));
  // bandControl.setBounds(bounds.removeFromRight(600));
  // Q.setBounds(bounds);
}

std::vector<juce::Component*> SinensisAudioProcessorEditor::getComps() {
  return {&attackSlider,
          &decaySlider,
          &cutoffFrequencySlider,
          &QSlider,
          &BandSelectorSlider,
          &ratioSlider,
          &dryWetSlider,
          &lowHighButton,
          &oddEvenButton,
          &peakButton,
          &midiPolyButton,
          &cButton,
          &cSharpButton,
          &dButton,
          &dSharpButton,
          &eButton,
          &fButton,
          &fSharpButton,
          &gButton,
          &gSharpButton,
          &aButton,
          &aSharpButton,
          &bButton};
}

std::vector<juce::Slider*> SinensisAudioProcessorEditor::getSliderComps() {
  return {&attackSlider,       &decaySlider, &cutoffFrequencySlider,
          &BandSelectorSlider, &ratioSlider, &QSlider,
          &dryWetSlider};
}

std::vector<juce::Button*> SinensisAudioProcessorEditor::getButtonComps() {
  return {&lowHighButton, &oddEvenButton, &peakButton,   &midiPolyButton,
          &cButton,       &cSharpButton,  &dButton,      &dSharpButton,
          &eButton,       &fButton,       &fSharpButton, &gButton,
          &gSharpButton,  &aButton,       &aSharpButton, &bButton};
}

void SinensisAudioProcessorEditor::setButtonParameters() {
  for (auto* button : getButtonComps()) {
    button->setClickingTogglesState(true);
    button->setLookAndFeel(&otherLookAndFeel);
  }

  lowHighButton.setRadioGroupId(2);
  oddEvenButton.setRadioGroupId(2);
  peakButton.setRadioGroupId(2);

  juce::Array<juce::Button*> band_modes_buttons;
  band_modes_buttons.add(&lowHighButton);
  band_modes_buttons.add(&oddEvenButton);
  band_modes_buttons.add(&peakButton);

  midiPolyButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(
          *apvts, "MIDIMODE", midiPolyButton));

  band_modes_radio_group = std::make_unique<RadioButtonAttachment>(
      *apvts->getParameter("BANDMODE"), band_modes_buttons, "BANDMODE", 2);

  cButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "C",
                                                               cButton));
  cSharpButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "CSHARP",
                                                               cSharpButton));
  dButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "D",
                                                               dButton));
  dSharpButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "DSHARP",
                                                               dSharpButton));
  eButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "E",
                                                               eButton));
  fButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "F",
                                                               fButton));
  fSharpButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "FSHARP",
                                                               fSharpButton));
  gButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "G",
                                                               gButton));
  gSharpButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "GSHARP",
                                                               gSharpButton));
  aButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "A",
                                                               aButton));
  aSharpButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "ASHARP",
                                                               aSharpButton));
  bButtonAttachement.reset(
      new juce::AudioProcessorValueTreeState::ButtonAttachment(*apvts, "B",
                                                               bButton));
}

void SinensisAudioProcessorEditor::setSlidersParameters() {
  for (auto* slider : getSliderComps()) {
    // slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0,
                            slider->getTextBoxHeight());
  }

  QSlider.setLookAndFeel(&resonance_look_and_feel);
  QSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  ratioSlider.setLookAndFeel(&ratio_look_and_feel);
  ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
  dryWetSlider.setLookAndFeel(&dry_wet_look_and_feel);
  dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
  // TODO changer quand il sera temps
  BandSelectorSlider.setLookAndFeel(&empty_look_and_feel);
  BandSelectorSlider.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
  attackSlider.setLookAndFeel(&attack_look_and_feel);
  attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  decaySlider.setLookAndFeel(&decay_look_and_feel);
  decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
  cutoffFrequencySlider.setLookAndFeel(&empty_look_and_feel);
  cutoffFrequencySlider.setSliderStyle(juce::Slider::RotaryHorizontalDrag);

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
  dryWetAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(*apvts, "DRYWET",
                                                               dryWetSlider));
  cutoffFrequencyAttachement.reset(
      new juce::AudioProcessorValueTreeState::SliderAttachment(
          *apvts, "root_frequency", cutoffFrequencySlider));
}

void SinensisAudioProcessorEditor::parameterValueChanged(int parameterIndex,
                                                         float newValue) {
  parametersChanged.set(true);
  switch (parameterIndex) {
    case 0:
      midi_mode = newValue > 0.5;
  }
  int index_start = 9;
  for (int i = 0; i < 12; i++) {
    if (parameterIndex == i + index_start) {
      note_lock[i] = newValue > 0.5;
    }
  }
}

void SinensisAudioProcessorEditor::timerCallback() {
  if (parametersChanged.compareAndSetBool(false, true)) {
    repaint();
  }
}

void SinensisAudioProcessorEditor::drawBandSelectionWidget(Graphics& g) {
  std::array<float, 6> gains = editorSinensis.m_gain;

  float marge_basse = 500;
  float taille = 70;
  float x_position = 50;
  float ecart = 40;
  juce::Point<float> point_bas, point_haut;

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



void SinensisAudioProcessorEditor::drawFrequencyWidget(juce::Graphics& g) {
  std::array<float, 6> gains = editorSinensis.m_gain;
  std::array<float, 6> frequencies = editorSinensis.m_frequency;

  float x_position = 28;
  float y_position = 95;
  float width = 240;
  float height = 70;
  juce::Path main_line;
  g.setColour(CustomColors::getGradient(gains[0]));

  float shapedFrequency = std::pow((editorSinensis.m_frequency[0] - 20) / 14970., 0.3);

  GraphicTools::addLine(main_line, x_position + (width * shapedFrequency),
                        y_position, x_position + (width * shapedFrequency),
                        y_position + height);

  g.strokePath(main_line, {5, PathStrokeType::curved, PathStrokeType::rounded});

  for (int i = 1; i < 6; i++) {
    juce::Path other_bands;
    float shapedFrequency = std::pow((editorSinensis.m_frequency[i] - 20) / 14970., 0.3);

    GraphicTools::addLine(other_bands, x_position + (width * shapedFrequency),
                          y_position, x_position + (width * shapedFrequency),
                          y_position + height);
    g.setColour(CustomColors::getGradient(gains[i]));
    // g.setColour(CustomColors::red);
    g.strokePath(other_bands,
                 {2, PathStrokeType::curved, PathStrokeType::rounded});
  }
}
