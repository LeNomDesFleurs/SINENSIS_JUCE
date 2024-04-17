/*
  ==============================================================================

    LookAndFeel.h
    Created: 3 Apr 2023 9:49:06am
    Author:  thoma

  ==============================================================================
*/

#pragma once
#include "CustomColors.hpp"
#include "GraphicTools.hpp"
#include "JuceHeader.h"
class OtherLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  // OtherLookAndFeel();
  //! [otherLookAndFeel]

  //! [drawRotarySlider]
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;

  void drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
};

class EmptyLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  // OtherLookAndFeel();
  //! [otherLookAndFeel]

  //! [drawRotarySlider]
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class RatioLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  // RatioLookAndFeel();
  //! [otherLookAndFeel]

  //! [drawRotarySlider]
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class ResonanceLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  // ResonanceLookAndFeel();
  //! [otherLookAndFeel]

  //! [drawRotarySlider]
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class BandSelectLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  // BandSelectLookAndFeel();
  //! [otherLookAndFeel]

  //! [drawRotarySlider]
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class AttackLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  // EnvelopeLookAndFeel();
  //! [otherLookAndFeel]

  //! [drawRotarySlider]
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class DecayLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};

class DryWetLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider& slider) override;
};