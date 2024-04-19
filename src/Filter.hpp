/*
  ==============================================================================

    Filter.h
    Created: 28 Feb 2023 11:54:07am
    Author:  thoma

  ==============================================================================
*/

#pragma once

#include <math.h>

#include <string>
#include <vector>
#include <array>
// #include "outils.hpp"

const float PI = 3.14159f;

namespace noi {

namespace Filter {

class BPF {
 private:
  // Parameters
  float m_fc = 0.0;
  float m_Q = 0.0;
  // Previous Values Memory
  std::array<float, 3> m_b;
  std::array<float, 3> m_a;
  // Biquad Coef
  std::array<float, 3> m_b_gain;
  std::array<float, 3> m_a_gain;
  // Compute Values
  float m_omega = 0.0f;
  float m_cosomega = 0.0f;
  float m_sinomega = 0.0f;
  float m_alpha = 0.0f;

  float m_sample_rate = 48000.0;

 public:
  BPF();
  BPF(float freq, float Q);
  void setSampleRate(float sample_rate);
  void computeBiquadCoef();
  float saturate(float input);
  void setParam(float frequence, float Q);

  float process(float b0);

}; /*BPF*/

}  // namespace Filter

}  // namespace noi
