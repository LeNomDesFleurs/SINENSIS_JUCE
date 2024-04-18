/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Filter.hpp"
#include "Sinensis.hpp"
#include "MidiBuffer.hpp"
#include "Tools.hpp"

//==============================================================================
/**
 */
class SinensisAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                               public juce::AudioProcessorARAExtension
#endif
{
 public:
  //==============================================================================
  SinensisAudioProcessor();
  ~SinensisAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;
  void setParam();
  // float processPoly(float input, int channel);
  void processEnvelopePoly(int i);
  void computeEnvelopesStep();
  void computeFrequencyMidiPoly();
  void extractMidiPoly(juce::MidiBuffer& midi_buffer);

  juce::AudioProcessorValueTreeState parameters;

 private:
  //==============================================================================
  juce::AudioProcessorValueTreeState::ParameterLayout createParams();

  Sinensis::Parameters sinensis_parameters{
      Sinensis::BandMode::LowHigh, 218.f, 0.707f, 0.5f, 1.5f, false};

  std::array<std::array<Sinensis, 2>, 6> sinensis;

  bool MidiOn;

  float m_sample_rate;
  float attack;
  float decay;
  float m_attack_step;
  float m_decay_step;
  std::array <float, 6> m_envelope_statut;
  float drywet;

  int test_counter;
  PolyMidiBuffer m_notes{6};
  std::array<float, 6> root_frequencies;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SinensisAudioProcessor)
};

