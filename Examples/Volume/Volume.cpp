/** Volume
 *
 *  Demonstrates reading a knob to adjust the level of audio 
 *  passing through the module.
 * 
 *  The Mix knob is used to adjust the volume of the signal.
 */
#include "aurora.h"

using namespace daisy;
using namespace aurora;

/** Our global hardware object */
Hardware hw;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	/** This filters, and prepares all of the module's controls for us. */
	hw.ProcessAllControls();

	/** Read the Mix knob value into a variable called "volume". */
	float volume = hw.GetKnobValue(KNOB_MIX);

	/** Loop through each sample of audio */
	for (size_t i = 0; i < size; i++)
	{
		/** Now for both left and right channels of audio, we'll multiply the input
		 *  by the volume to turn down the output signal.
		 *
		 *  With Mix all the way down, this will result in silence.
		 *  With Mix all the way up, the audio should pass through at the normal level.
		 */

		/** Left Channel */
		out[0][i] = in[0][i] * volume;

		/** Right Channel */
		out[1][i] = in[1][i] * volume;
	}
}

int main(void)
{
	/** Initialize the Hardware */
	hw.Init();

	/** Start the audio engine calling the function defined above periodically */
	hw.StartAudio(AudioCallback);

	/** Infinite Loop */
	while (1)
	{
	}
}
