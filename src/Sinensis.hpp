/*
  ==============================================================================

    Sinensis.h
    Created: 4 Mar 2023 5:01:48pm
    Author:  thoma

  ==============================================================================
*/

#pragma once
#include <vector>

#include "Filter.hpp"
#include "JuceHeader.h"
#include "Tools.hpp"

class Sinensis {
 public:
  enum BandMode { LowHigh, OddEven, Peak };
  ///Parameters of Sinensis
  /// @param BandMode band_mode_selector
  /// @param float root_frequency
  /// @param float resonance
  /// @param float band_selector
  /// @param float ratio
  /// @param bool noteLock[12]
  struct Parameters {
    BandMode band_selector_mode;
    float root_frequency;
    float resonance;
    float band_selector;
    float ratio;
    float output_volume;
    std::array<bool, 12> note_lock;
  };

  Sinensis();
  ~Sinensis();
  void setParameters(Sinensis::Parameters parameters);
  void setFrequency(float frequency){
    m_parameters.root_frequency = frequency;
  }
  float processSample(float input);
  float processSinensis(float input);
  void attenuate(float& input);
  void saturate(float& input);
  float noteLock(float frequency);

  void computeGain();
  void computeLowHigh();
  void computeOddEven();
  void computePeak();


  void computeQ();
  void prepareBpf();
  void computeFrequencies();

  void setSampleRate(float sample_rate);

  Sinensis::Parameters m_parameters;
  noi::Filter::BPF m_bpf[6];
  float m_sample_rate;
  float m_Q[6];
 
  std::array<float, 6> m_frequency;
  std::array<float, 6> m_gain;

  float m_saturation_memory;
  // int m_notes[6];
};