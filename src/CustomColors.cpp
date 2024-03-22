#include "CustomColors.hpp"

/// @brief return a gradient that goes from custom green to custom red by going
/// trought yellow and orange
/// @param value
/// @return
juce::Colour CustomColors::getGradient(float value) {
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