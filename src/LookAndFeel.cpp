/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 3 Apr 2023 9:49:06am
    Author:  thoma

  ==============================================================================
*/

#include "LookAndFeel.hpp"

void EmptyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                        int width, int height, float sliderPos,
                                        const float rotaryStartAngle,
                                        const float rotaryEndAngle,
                                        juce::Slider& slider)

{
  // g.fillAll(CustomColors::red);
}

//! [drawRotarySlider]
void OtherLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                        int width, int height, float sliderPos,
                                        const float rotaryStartAngle,
                                        const float rotaryEndAngle,
                                        juce::Slider& slider)
//! [drawRotarySlider]
{
  //! [locals]
  auto outline = juce::Colours::black;
  auto fill = juce::Colours::lightgrey;

  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
  auto toAngle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  auto lineW = juce::jmin(1.0f, radius * 0.5f);
  auto arcRadius = radius - lineW * 0.5f;

  juce::Path backgroundArc;
  backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                              arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                              rotaryEndAngle, true);

  g.setColour(outline);
  g.strokePath(backgroundArc,
               juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                    juce::PathStrokeType::rounded));

  if (slider.isEnabled()) {
    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius,
                           arcRadius, 0.0f, rotaryStartAngle, toAngle, true);

    g.setColour(fill);
    g.strokePath(valueArc,
                 juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));
  }

  // auto thumbWidth = lineW * 2.0f;
  /*juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius *
     std::cos(toAngle - juce::MathConstants<float>::halfPi), bounds.getCentreY()
     + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));*/

  g.setColour(slider.findColour(juce::Slider::thumbColourId));
  // g.fillEllipse(juce::Rectangle<float>(thumbWidth,
  // thumbWidth).withCentre(thumbPoint));
}

void OtherLookAndFeel::drawToggleButton(juce::Graphics& g,
                                        juce::ToggleButton& toggleButton,
                                        bool /*shouldDrawButtonAsHighlighted*/,
                                        bool /*shouldDrawButtonAsDown*/) {
  using namespace juce;

  auto bounds = toggleButton.getLocalBounds().reduced(2);

  auto buttonIsOn = toggleButton.getToggleState();

  const int cornerSize = 4;

  g.setColour(
      buttonIsOn
          ? toggleButton.findColour(TextButton::ColourIds::buttonOnColourId)
          : toggleButton.findColour(TextButton::ColourIds::buttonColourId));
  g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

  // button color
  g.setColour(buttonIsOn ? CustomColors::yellow : CustomColors::green);
  g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);
  // text color
  g.setFont(juce::Font("Times New Roman", toggleButton.getHeight() - 10,
                       juce::Font::italic));
  g.drawFittedText(toggleButton.getName(), bounds, Justification::centred, 1);
  // }
}

//! [otherLookAndFeel]

//! [drawRotarySlider]
void RatioLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                        int width, int height, float sliderPos,
                                        const float rotaryStartAngle,
                                        const float rotaryEndAngle,
                                        juce::Slider& slider)
//! [drawRotarySlider]
{
  float fwidth = static_cast<float>(width) - 5;
  float fheight = static_cast<float>(height) - 5;
  float trackWidth = 2;
  juce::Path lines;

  float bipolar_slider_pos;
  if (sliderPos < (1. / 3.)) {
    bipolar_slider_pos = sliderPos * 3.;
    bipolar_slider_pos = 1 - bipolar_slider_pos;
  } else {
    bipolar_slider_pos = sliderPos - (1. / 3.);
    bipolar_slider_pos *= (1 + 1. / 3.);
  }

  float middle = fwidth / 2.f;
  float max_size = fwidth / 2.f;
  float max_ratio = fwidth / 6.f;
  max_ratio *= bipolar_slider_pos / 1.8;

  float ratio = sliderPos * 2;

  GraphicTools::addLine(lines, middle, 0, middle, fheight);

  float x_position = middle;
  for (int i = 0; i < 5; i++) {
    if (sliderPos > 1 / 3.) {
      x_position += max_ratio;
    } else {
      x_position -= max_ratio;
    }
    GraphicTools::addLine(lines, x_position, 10 + (i * 10), x_position,
                          fheight);
  }

  g.setColour(CustomColors::getGradient(bipolar_slider_pos));
  g.strokePath(lines,
               {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});
}

void ResonanceLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                            int width, int height,
                                            float sliderPos,
                                            const float rotaryStartAngle,
                                            const float rotaryEndAngle,
                                            juce::Slider& slider)
//! [drawRotarySlider]
{
  // Paint 2 curves that join at the peak point, the higher the value, the
  // higher the peak, the two control point level up more slowly to give a
  // narrower feel
  // added padding to avoid clipping
  float fwidth = static_cast<float>(width) - 5;
  float fheight = static_cast<float>(height) - 5;
  float trackWidth = 2;

  Point<float> start_point_left = {5, fheight};
  Point<float> start_point_right = {fwidth, fheight};
  Point<float> control_point_left = {(fwidth / 10) * 4,
                                     fheight - (fheight / 3) * sliderPos};
  Point<float> control_point_right = {(fwidth / 10) * 6,
                                      fheight - (fheight / 3) * sliderPos};
  // peak point
  Point<float> peak_point = {fwidth / 2,
                             fheight - (fheight * sliderPos * 0.8f)};

  juce::Path left_curve, right_curve;

  left_curve.startNewSubPath(start_point_left);
  left_curve.quadraticTo(control_point_left, peak_point);
  g.setColour(CustomColors::getGradient(sliderPos));
  g.strokePath(left_curve,
               {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});

  right_curve.startNewSubPath(start_point_right);
  right_curve.quadraticTo(control_point_right, peak_point);
  g.strokePath(right_curve,
               {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});
}

void AttackLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                         int width, int height, float sliderPos,
                                         const float rotaryStartAngle,
                                         const float rotaryEndAngle,
                                         juce::Slider& slider)
//! [drawRotarySlider]
{
  auto radius = (float)juce::jmin(width / 2, height / 2) - 10.0f;
  auto centreX = (float)x + (float)width * 0.5f;
  auto centreY = (float)y + (float)height * 0.5f;
  auto rx = centreX - radius;
  auto ry = centreY - radius;
  auto rw = radius * 2.0f;
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  //! [locals]

  g.setColour(CustomColors::getGradientWithoutGreen(sliderPos));
  juce::Path p;
  sliderPos /= 1.6;
  p.addRoundedRectangle(0 - width / 20, -height / 2., width / 10, height, 5);
  p.applyTransform(juce::AffineTransform::rotation(sliderPos * 2)
                       .translated(centreX, centreY));
  //! [pointer]
  // pointer

  g.fillPath(p);
  //! [pointer]
}

void DecayLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                        int width, int height, float sliderPos,
                                        const float rotaryStartAngle,
                                        const float rotaryEndAngle,
                                        juce::Slider& slider)
//! [drawRotarySlider]
{
  auto radius = (float)juce::jmin(width / 2, height / 2) - 10.0f;
  auto centreX = (float)x + (float)width * 0.5f;
  auto centreY = (float)y + (float)height * 0.5f;
  auto rx = centreX - radius;
  auto ry = centreY - radius;
  auto rw = radius * 2.0f;
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  //! [locals]

  g.setColour(CustomColors::getGradientWithoutGreen(sliderPos));

  sliderPos /= 1.6;

  juce::Path d;
  d.addRoundedRectangle(0 - width / 20, -height / 2., width / 10, height, 5);
  d.applyTransform(juce::AffineTransform::rotation(-sliderPos * 2)
                       .translated(centreX, centreY));
  //! [pointer]
  // pointer
  g.fillPath(d);
  //! [pointer]
}