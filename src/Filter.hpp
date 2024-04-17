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
// #include "outils.hpp"

const float PI = 3.14159f;

namespace noi {

namespace Filter {

class BPF {
 private:
  // Parameters
  float m_fc{0.0};
  float m_Q{0.0};
  // Previous Values Memory
  float m_b[3]{0.0f, 0.0f, 0.0f};
  float m_a[3]{0.0f, 0.0f, 0.0f};
  // Biquad Coef
  float m_b_gain[3]{0.0f, 0.0f, 0.0f};
  float m_a_gain[3]{0.0f, 0.0f, 0.0f};
  // Compute Values
  float m_omega{0.0f};
  float m_cosomega{0.0f};
  float m_sinomega{0.0f};
  float m_alpha{0.0f};

  float m_sampling_frequency{48000.0};

 public:
  BPF();
  BPF(float freq, float Q);
  // inline BPF() {}
  void setSampleRate(float sampling_frequency);
  void computeBiquadCoef();

  void setParam(float frequence, float Q);

  float process(float b0);

}; /*BPF*/

}  // namespace Filter

}  // namespace noi
