/*
  ==============================================================================

    Sinensis.cpp
    Created: 4 Mar 2023 5:01:37pm
    Author:  thoma

  ==============================================================================
*/
#include "Sinensis.hpp"

Sinensis::Sinensis() {
  m_parameters = {Sinensis::BandMode::LowHigh, 218.f, 0.707f, 0.f, 1.5f, false};
  m_sample_rate = 48000;
  computeFrequencies();
  computeGain();
  computeQ();
  prepareBpf();
}

Sinensis::~Sinensis() {}

void Sinensis::setParameters(Sinensis::Parameters parameters) {
  m_parameters = parameters;
}

float Sinensis::processSample(float input) {
  float output = 0;
  for (int i = 0; i < 6; i++) {
    output += m_bpf[i].process(input) * m_gain[i];
  }
  attenuate(output);
  // saturate(output);
  return output;
}

float Sinensis::processSinensis(float input) {

  computeFrequencies();
  computeGain();
  computeQ();
  prepareBpf();

  return processSample(input);
}

void Sinensis::attenuate(float& input) {
  input *= m_parameters.output_volume;
  input *= 0.9;
  input *= 1 - (pow(m_parameters.resonance / 100, 0.5) / 1.5);
}

void Sinensis::saturate(float& input) {
  float max = 1.f;

  input = pow(input, 1. / 2.);
  // additionnal clipping
  if (input < -max) input = -max;
  if (input > max) input = max;

  m_saturation_memory = input;
}

void Sinensis::computeFrequencies() {
  std::array<float, 6> temp_frequency;
  for (int i = 0; i < 6; i++) {
    // multiply frequence by ratio
    float thisBandFreq = m_parameters.root_frequency;
    for (int j = 1; j <= i; j++) {
      thisBandFreq *= m_parameters.ratio;
    }
    thisBandFreq = truncf(thisBandFreq);
    // frequency folding
    while (thisBandFreq > 15000.f || thisBandFreq < 30.f) {
      if (thisBandFreq > 15000.F) {
        thisBandFreq = 15000.f - (thisBandFreq - 15000.f);
      }
      if (thisBandFreq < 30.f) {
        thisBandFreq = 30.f + (30.f - thisBandFreq);
      }
    }

    bool note_lock_on = false;
    // check for note lock
    int j = 0;
    while (j < 12 && !note_lock_on) {
      note_lock_on = m_parameters.note_lock[j] ? true : false;
      j++;
    }
    // if not lock, skim through all frequencies until one is superior to actual
    // frequency
    if (note_lock_on) {
      m_frequency[i] = noteLock(thisBandFreq);
      // else, keep the calculated band
    } else {
      m_frequency[i] = thisBandFreq;
    }
  }
}

float Sinensis::noteLock(float frequency) {
  for (int j = 0; j < 120; j++) {
    int note = j % 12;
    if (m_parameters.note_lock[note]) {
      if (Tools::equal_temperament_frequencies[j] > frequency) {
        return Tools::equal_temperament_frequencies[j];
      }
    }
  }
  for (int j = 120; j > 0; j--) {
    int note = j % 12;
    if (m_parameters.note_lock[note]) {
      if (Tools::equal_temperament_frequencies[j] < frequency) {
        return Tools::equal_temperament_frequencies[j];
      }
    }
  }
  return 30.f;
}

void Sinensis::computeGain() {
  switch (m_parameters.band_selector_mode) {
    case LowHigh:
      computeLowHigh();
      break;
    case OddEven:
      computeOddEven();
      break;
    case Peak:
      computePeak();
      break;
  }
}

void Sinensis::computeLowHigh() {
  float band_selector = m_parameters.band_selector;
  float alpha = band_selector * 4.0f - 2.0f;
  float beta = -2.0f * (band_selector * band_selector) + 1.0f;

  for (int i = 0; i < 6; i++) {
    float this_band_index = static_cast<float>(i);
    this_band_index /= 5.0f;
    float this_band_gain = this_band_index * alpha + beta;
    if (this_band_gain < 0.0f) {
      this_band_gain = 0.0f;
    }
    m_gain[i] = this_band_gain;
  }
}

void Sinensis::computeOddEven() {
  for (int i = 0; i < 6; i++) {
    float this_band_gain = 0;
    // odd case
    if (i == 0 || i == 2 || i == 4) {
      this_band_gain = m_parameters.band_selector;
    }
    // even case
    if (i == 1 || i == 3 || i == 5) {
      this_band_gain = 1.0f - m_parameters.band_selector;
    }
    m_gain[i] = this_band_gain;
  }
}

void Sinensis::computePeak() {
  float cursor = m_parameters.band_selector;
  cursor *= 5.;
  for (int i = 0; i < 6; i++) {
    float band_index = static_cast<float>(i);
    float band_gain = 0.0f;
    if (cursor < band_index) {
      band_gain = band_index - cursor;
    } else {
      band_gain = cursor - band_index;
    }

    band_gain = 1 - band_gain;
    if (band_gain > 1.) {
      band_gain = 1.;
    }
    if (band_gain < 0.) {
      band_gain = 0.;
    }
    m_gain[i] = band_gain;
  }
}

void Sinensis::computeQ() {
  for (int i = 0; i < 6; i++) {
    float Q = (m_parameters.resonance * m_gain[i]) + 0.707f;
    if (Q > 32.) Q = 32.0f;
    m_Q[i] = Q;
  }
}

void Sinensis::prepareBpf() {
  for (int i = 0; i < 6; i++) {
    m_bpf[i].setParam(m_frequency[i], m_Q[i]);
  }
}

void Sinensis::setSampleRate(float sample_rate) {
  for (auto bpf : m_bpf) bpf.setSampleRate(sample_rate);
  m_sample_rate = sample_rate;
}
