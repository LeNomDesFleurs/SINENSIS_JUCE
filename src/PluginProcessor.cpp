/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.hpp"

#include "PluginEditor.hpp"

//==============================================================================
SinensisAudioProcessor::SinensisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      parameters(*this, nullptr, "PARAMETERS", createParams())
#endif
{
}

SinensisAudioProcessor::~SinensisAudioProcessor() {}

//==============================================================================
const juce::String SinensisAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool SinensisAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool SinensisAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool SinensisAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double SinensisAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SinensisAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int SinensisAudioProcessor::getCurrentProgram() { return 0; }

void SinensisAudioProcessor::setCurrentProgram(int index) {}

const juce::String SinensisAudioProcessor::getProgramName(int index) {
  return {};
}

void SinensisAudioProcessor::changeProgramName(int index,
                                               const juce::String& newName) {}

//==============================================================================
void SinensisAudioProcessor::prepareToPlay(double sampleRate,
                                           int samplesPerBlock) {
  for (int voice = 0; voice < 6; voice++){
    for (int channel = 0; channel < 2; channel++) {
      sinensis[voice][channel].setSampleRate(static_cast<float>(sampleRate));
    }}
  m_sample_rate = static_cast<float>(sampleRate);
  // sinensis.setSampleRate(static_cast <float> (sampleRate));
}

void SinensisAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SinensisAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void SinensisAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& midiBuffer) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  setParam();

if (MidiOn){
  extractMidiPoly(midiBuffer);
  computeFrequencyMidiPoly();
  computeEnvelopesStep();
  for (int voice = 0; voice < 6; voice++) {
    processEnvelopePoly(voice);
    // m_envelope_statut[voice];
    for (int channel = 0; channel < 2; channel++){
      sinensis[voice][channel].setFrequency(root_frequencies[voice]);
    }
  }
}

  for (auto channel = 0; channel < 2; ++channel) {
    // to access the sample in the channel as a C-style array
    auto channelSamples = buffer.getWritePointer(channel);

    // for each sample in the channel
    for (auto n = 0; n < buffer.getNumSamples(); ++n) {
      const auto input = channelSamples[n];
        float output = 0;
      if (MidiOn){
        for (int voice = 0; voice < 6; voice++) {
          output += sinensis[voice][channel].processSinensis(input)*m_envelope_statut[voice];
        }
      }
      else {
        output = sinensis[0][channel].processSinensis(input);
      }
      channelSamples[n] = Tools::equalGainCrossfade(channelSamples[n], output, drywet);
    }
  }
}

void SinensisAudioProcessor::extractMidiPoly(juce::MidiBuffer& midi_buffer) {
  for (const auto metadata : midi_buffer) {
    const auto msg = metadata.getMessage();
    if (msg.isNoteOn())
      m_notes.addNote(msg.getNoteNumber());
    else if (msg.isNoteOff())
      m_notes.removeNote(msg.getNoteNumber());
  }
}

float SinensisAudioProcessor::processEnvelopePoly(int i) {
  if (m_notes[i] == 0)
    m_envelope_statut[i] -= m_decay_step;
  else
    m_envelope_statut[i] += m_attack_step;
  if (m_envelope_statut[i] > 1) m_envelope_statut[i] = 1;
  if (m_envelope_statut[i] < 0) m_envelope_statut[i] = 0;
}

void SinensisAudioProcessor::computeFrequencyMidiPoly() {
  for (int i = 0; i < 6; i++) {
    if (m_notes[i] != 0) {
      root_frequencies[i] = juce::MidiMessage::getMidiNoteInHertz(m_notes[i]);
    }
  }
}

void SinensisAudioProcessor::computeEnvelopesStep() {
  m_attack_step =(1.f / attack) / m_sample_rate;
  m_decay_step = (1.f / decay) / m_sample_rate;
}



float processEnveloppe();

//==============================================================================
bool SinensisAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SinensisAudioProcessor::createEditor() {
  return new SinensisAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void SinensisAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void SinensisAudioProcessor::setStateInformation(const void* data,
                                                 int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
SinensisAudioProcessor::createParams() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  params.push_back(std::make_unique<juce::AudioParameterBool>(
      "MIDIMODE", "MIDIMODE", false));
  params.push_back(std::make_unique<juce::AudioParameterChoice>(
      "BANDMODE", "Band Selector Mode",
      juce::StringArray{"Low/High", "Odd/Even", "Peak"}, 0));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "root_frequency", "Root Frequency",
      juce::NormalisableRange{20.f, 15000.f, 0.1f, 0.1f, false}, 500.f)),

      params.push_back(std::make_unique<juce::AudioParameterFloat>(
          "RATIO", "Ratio", juce::NormalisableRange{0.5f, 2.0f, 0.001f}, 1.5f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "RESONANCE", "Resonance", juce::NormalisableRange{0.7f, 100.0f, 0.1f},
      20.0f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "band_selector", "Number Of Band",
      juce::NormalisableRange{0.0f, 1.0f, 0.01f}, 0.3f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "OUTPUTVOLUME", "Ouput Volume",
      juce::NormalisableRange{0.0f, 2.0f, 0.01f}, 0.6f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "ATTACK", "Attack", juce::NormalisableRange<float>{0.0001f, 3.0f, 0.0001f, 2.f, false},
      0.4f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "DECAY", "Decay", juce::NormalisableRange<float>{0.0001f, 3.0f, 0.0001f, 2.f, false},
      0.4f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "DRYWET", "DryWet", juce::NormalisableRange<float>{0.f, 1.0f, 0.0001f, 1.f, false},
      1.f));

  params.push_back(std::make_unique<juce::AudioParameterBool>("C", "C", false));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>("CSHARP", "CSHARP", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>("D", "D", false));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>("DSHARP", "DSHARP", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>("E", "E", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>("F", "F", false));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>("FSHARP", "FSHARP", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>("G", "G", false));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>("GSHARP", "GSHARP", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>("A", "A", false));
  params.push_back(
      std::make_unique<juce::AudioParameterBool>("ASHARP", "ASHARP", false));
  params.push_back(std::make_unique<juce::AudioParameterBool>("B", "B", false));

  return {params.begin(), params.end()};
}

void SinensisAudioProcessor::setParam() {
  MidiOn =
      static_cast<float>(*parameters.getRawParameterValue("MIDIMODE")) > 0.5;


  int band_mode =
      static_cast<float>(*parameters.getRawParameterValue("BANDMODE"));

  sinensis_parameters.band_selector_mode =
      static_cast<Sinensis::BandMode>(band_mode);

  // PARAM
  sinensis_parameters.root_frequency =
      *parameters.getRawParameterValue("root_frequency");

  sinensis_parameters.ratio = *parameters.getRawParameterValue("RATIO");
  sinensis_parameters.resonance = *parameters.getRawParameterValue("RESONANCE");
  sinensis_parameters.band_selector =
      *parameters.getRawParameterValue("band_selector");
  sinensis_parameters.output_volume =
      *parameters.getRawParameterValue("OUTPUTVOLUME");
  // ENVELOPPE
  attack = *parameters.getRawParameterValue("ATTACK");
  decay = *parameters.getRawParameterValue("DECAY");
  drywet = *parameters.getRawParameterValue("DRYWET");

  sinensis_parameters.note_lock[0] =
      *parameters.getRawParameterValue("C") > 0.5;
  sinensis_parameters.note_lock[1] =
      *parameters.getRawParameterValue("CSHARP") > 0.5;
  sinensis_parameters.note_lock[2] =
      *parameters.getRawParameterValue("D") > 0.5;
  sinensis_parameters.note_lock[3] =
      *parameters.getRawParameterValue("DSHARP") > 0.5;
  sinensis_parameters.note_lock[4] =
      *parameters.getRawParameterValue("E") > 0.5;
  sinensis_parameters.note_lock[5] =
      *parameters.getRawParameterValue("F") > 0.5;
  sinensis_parameters.note_lock[6] =
      *parameters.getRawParameterValue("FSHARP") > 0.5;
  sinensis_parameters.note_lock[7] =
      *parameters.getRawParameterValue("G") > 0.5;
  sinensis_parameters.note_lock[8] =
      *parameters.getRawParameterValue("GSHARP") > 0.5;
  sinensis_parameters.note_lock[9] =
      *parameters.getRawParameterValue("A") > 0.5;
  sinensis_parameters.note_lock[10] =
      *parameters.getRawParameterValue("ASHARP") > 0.5;
  sinensis_parameters.note_lock[11] =
      *parameters.getRawParameterValue("B") > 0.5;

  // SET OBJECT
  sinensis[0][0].setParameters(sinensis_parameters);
  sinensis[0][1].setParameters(sinensis_parameters);

  if (MidiOn) {
    for (int voice = 1; voice < 6; voice++) {
      for (int channel = 0; channel < 2; channel++) {
        sinensis[voice][channel].setParameters(sinensis_parameters);
      }
    }
  }
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new SinensisAudioProcessor();
}
