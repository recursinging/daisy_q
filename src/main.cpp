#include <daisy_seed.h>
#include <q/pitch/pitch_detector.hpp>
#include <q/support/notes.hpp>
#include <q/fx/biquad.hpp>
#include <q/synth/sin.hpp>

/**
 * This is a simple example program showing how the Q library can be used on the Daisy platform
 * 
 * In this example, we are simply taking the channel 1 input and feeding it into a BACF pitch 
 * detector that is configured to detect the fundamental frequency of monophonic signals 
 * between C2 (65Hz) and C5 (523Hz). When a frequency is detected, the phase accumulator
 * of a synthesized sine wave will be updated to reflect the detected frequency.
 * 
 * This synthesized sine wave is output on channel 1.
 * 
 * Channel 2 is a pass through.
 */

// Namespaces...
using namespace daisy;
namespace q = cycfi::q;
using namespace cycfi::q::literals;

DaisySeed hardware;

uint32_t sample_rate = 48000;

// The dectected frequencies
q::frequency detected_f0 = q::frequency(0.0f);

// The frequency detection bounds;
q::frequency lowest_frequency  = q::notes::C[2];
q::frequency highest_frequency = q::notes::C[5];

// The pitch detector pre-processor
q::pd_preprocessor::config preprocessor_config;
q::pd_preprocessor         preprocessor{preprocessor_config,
                                lowest_frequency,
                                highest_frequency,
                                sample_rate};

// The pitch detector
q::pitch_detector pd{lowest_frequency, highest_frequency, sample_rate};

// A phase accumulator for our synthesized output
q::phase phase_accumulator = q::phase();
q::phase f0_phase          = q::phase(detected_f0, sample_rate);



void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    // Iterate over the samples in our block (default block size is 48)
    for(size_t i = 0; i < size; i++)
    {
        // This is the input signal
        float sig_l = in[0][i];
        float sig_r = in[1][i];

        // Pre-process the signal for pitch detection
        float pd_sig = preprocessor(sig_l);

        // send the processed sample through the pitch detector
        if(pd(pd_sig))
        {
            detected_f0 = q::frequency{pd.get_frequency()};
            f0_phase    = q::phase(detected_f0, sample_rate);
        }

        out[0][i] = q::sin(phase_accumulator) * 0.5;
        out[1][i] = sig_r;

        // Increment our phase accumulator
        phase_accumulator += f0_phase;
    }
}

int main(void)
{
    // Initialize the hardware
    hardware.Configure();
    hardware.Init();

    // Start the audio...
    hardware.StartAudio(AudioCallback);
}
