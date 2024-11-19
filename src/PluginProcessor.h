#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/sdelay.hpp"
#include "dsp/curve_v2.h"
#include <random>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener,
    public mana::CurveV2::Listener
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void RandomParameter();
    void PanicFilterFb();

    SDelay delays_[2];
    std::unique_ptr<mana::CurveV2> curve_;
    juce::AudioParameterFloat* beta_{};
    juce::AudioParameterFloat* min_bw_{};
    juce::AudioParameterFloat* f_begin_{};
    juce::AudioParameterFloat* f_end_{};
    juce::AudioParameterFloat* delay_time_{};
    juce::AudioParameterBool* pitch_x_asix_{};
    juce::AudioParameterChoice* resolution_{};
    std::unique_ptr<juce::AudioProcessorValueTreeState> value_tree_;

    juce::Random random_;
    std::atomic_bool update_flag_{ true };
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)

    // ͨ�� Listener �̳�
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    void UpdateFilters();

    // ͨ�� Listener �̳�
    void OnAddPoint(mana::CurveV2* generator, mana::CurveV2::Point p, int before_idx) override;
    void OnRemovePoint(mana::CurveV2* generator, int remove_idx) override;
    void OnPointXyChanged(mana::CurveV2* generator, int changed_idx) override;
    void OnPointPowerChanged(mana::CurveV2* generator, int changed_idx) override;
    void OnReload(mana::CurveV2* generator) override;
};
