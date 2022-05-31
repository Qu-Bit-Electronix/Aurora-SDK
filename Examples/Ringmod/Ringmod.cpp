/** Ringmode
 *
 *  Demonstrates generating a signal, to create a basic audio effect
 *  with multiple controls.
 *
 *  A simple sine wave oscillator is used as the modulator, while the audio
 *  input is used as the carrier.
 *
 *  The following controls are used:
 *
 *  - Warp: adjusts the frequency of modulation
 *  - Blur: adjusts the depth of modulation
 *  - Mix: adjusts the blend between the dry signal, and the ring-modulated signal
 */
#include "aurora.h"
#include "daisysp.h"

using namespace daisy;
using namespace aurora;
using namespace daisysp;

Hardware hw;
Oscillator osc;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    /** This filters, and prepares all of the module's controls for us. */
    hw.ProcessAllControls();

    /** Assign our knobs to some controls */
    /** Modulator frequency in Hz */
    float freq = fmap(hw.GetKnobValue(KNOB_WARP), 10.0, 1500.0, Mapping::LOG);
    osc.SetFreq(freq);

    /** How much modulation */
    float depth = fmap(hw.GetKnobValue(KNOB_BLUR), 0.5, 1.0);

    /** Dry/Wet balance */
    float mix = fmap(hw.GetKnobValue(KNOB_MIX));

    for (size_t i = 0; i < size; i++)
    {
        /** Read our inputs */
        float dry_left = in[0][i];
        float dry_right = in[1][i];

        /** Generate the modulator signal */
        float mod_signal = osc.Process() * depth;

        /** Create our ring modulated signals by modulating the two */
        float wet_left = dry_left * mod_signal;
        float wet_right = dry_right * mod_signal;

        /** Now write the mix of the dry and wet together to the outputs */
        /** Left */
        out[0][i] = (dry_left * (1.f - mix)) + (wet_left * mix);
        /** Right */
        out[1][i] = (dry_right * (1.f - mix)) + (wet_right * mix);
    }
}

int main(void)
{
    /** Initialize the Hardware */
    hw.Init();

    /** Initialize the Oscillator we'll use to modulate our signal */
    osc.Init(hw.AudioSampleRate());
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetAmp(1.0);

    while (1)
    {
    }
}