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
void BPF::setSampleRate(float sample_rate) {
  m_sample_rate = sample_rate;
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
  m_omega = 2.f * PI * (m_fc / m_sample_rate);
  m_cosomega = cosf(m_omega);
  m_sinomega = sinf(m_omega);
  m_alpha = m_sinomega / (2 * m_Q);
  computeBiquadCoef();
}

float BPF::saturate(float input) {
  float max = 1.f;

  // input = pow(input, 1. / 2.);
  // additionnal clipping
  if (input < -max) {input = -max;}
  if (input > max) {input = max;}

  return input;
  // m_saturation_memory = input;
}

float BPF::process(float b0) {
  // feedback & clipping
  float feedback = m_a[0];
  // 1050 choosed by experimentation, get close to selfosc
  feedback *= (m_Q / 1050.F);

  feedback = saturate(feedback);

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
