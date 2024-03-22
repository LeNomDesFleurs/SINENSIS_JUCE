#pragma once

#include <JuceHeader.h>

namespace CustomColors {
const juce::Colour red = juce::Colour::fromString("FFef3d59");
const juce::Colour orange = juce::Colour::fromString("FFe17a47");
const juce::Colour yellow = juce::Colour::fromString("FFEFC958");
const juce::Colour green = juce::Colour::fromString("FF4AB19D");
const juce::Colour blue = juce::Colour::fromString("FF344E5C");

juce::Colour getGradient(float value);
}  // namespace CustomColors
