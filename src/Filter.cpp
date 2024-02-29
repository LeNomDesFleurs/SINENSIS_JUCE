/*
  ==============================================================================

    Filter.h
    Created: 28 Feb 2023 11:54:07am
    Author:  thoma

  ==============================================================================
*/

#include "Filter.hpp"
// #include "outils.hpp"

namespace noi {

namespace Filter {

BPF::BPF() {}
BPF::BPF(float freq, float Q) { setParam(freq, Q); }
//  BPF() {}
void BPF::setSamplingFrequency(float sampling_frequency) {
  m_sampling_frequency = sampling_frequency;
}
void BPF::computeBiquadCoef() {
  m_a_gain[0] = 1 + m_alpha;

  m_b_gain[0] = m_alpha * m_Q;
  m_b_gain[0] /= m_a_gain[0];

  m_b_gain[1] = 0;
  m_b_gain[1] /= m_a_gain[0];

  m_b_gain[2] = -m_Q * m_alpha;
  m_b_gain[2] /= m_a_gain[0];

  m_a_gain[1] = -2 * m_cosomega;
  m_a_gain[1] /= m_a_gain[0];

  m_a_gain[2] = 1 - m_alpha;
  m_a_gain[2] /= m_a_gain[0];
}

void BPF::setParam(float frequence, float Q) {
  if (m_fc == frequence && m_Q == Q) {
    return;
  }
  m_fc = frequence;
  m_Q = Q;
  m_omega = 2.f * PI * (m_fc / 48000.f);
  m_cosomega = cosf(m_omega);
  m_sinomega = sinf(m_omega);
  m_alpha = m_sinomega / (2 * m_Q);
  computeBiquadCoef();
}

float BPF::process(float b0) {
  // feedback & clipping
  float feedback = m_a[0];
  // 1500 choosed by experimentation w/ sinensis, self osc around Q = 38
  feedback *= (m_Q / 1500.F);
  if (feedback < -0.95) {
    float overtaking = feedback + 0.95f;
    feedback = -0.95f - (overtaking / 5.f);
  }
  if (feedback > 0.95f) {
    float overtaking = feedback - 0.95f;
    feedback = 0.95f + (overtaking / 5.f);
  }
  if (feedback > 1.0f) {
    feedback = 1.0f;
  }
  if (feedback < -1.0f) {
    feedback = -1.0f;
  }
  b0 += feedback;
  // shift new value in
  m_b[2] = m_b[1];
  m_b[1] = m_b[0];
  m_b[0] = b0;
  m_a[2] = m_a[1];
  m_a[1] = m_a[0];

  m_a[0] = m_b[0] * m_b_gain[0] + m_b[1] * m_b_gain[1] + m_b[2] * m_b_gain[2] -
           m_a[1] * m_a_gain[1] - m_a[2] * m_a_gain[2];

  return m_a[0];
}

};  // namespace Filter

}  // namespace noi
