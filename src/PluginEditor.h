#pragma once

#include "PluginProcessor.h"
#include "ui/LM_slider.h"
#include "ui/common_curve_editor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    LMKnob beta_;
    LMKnob f_begin_;
    LMKnob f_end_;
    LMKnob delay_time_;
    LMKnob min_bw_;
    mana::CommonCurveEditor curve_;
    juce::Label num_filter_label_;
    juce::ToggleButton x_axis_;
    std::unique_ptr<juce::ButtonParameterAttachment> x_axis_attachment_;

    juce::Label res_label_;
    juce::ComboBox reslution_;
    std::unique_ptr<juce::ComboBoxParameterAttachment> resolution_attachment_;

    juce::TextButton random_;
    juce::TextButton clear_curve_;
    juce::TextButton panic_;

    std::vector<float> group_delay_cache_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)

    // Í¨¹ý Timer ¼Ì³Ð
    void timerCallback() override;
};
