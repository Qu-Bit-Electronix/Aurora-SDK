/** Blink
 *
 *  Demonstrates blinking of a single LED
 * 
 *  In this example, the Freeze LED will blink on/off
 *  once every second.
 */
#include "aurora.h"

using namespace daisy;
using namespace aurora;

/** Our global hardware object */
Hardware hw;

int main(void)
{
	/** Initialize the Hardware */
	hw.Init();

	/** Create a variable to hold the state of the LED */
	bool led_state = true;

	/** Infinite Loop */
	while (1)
	{
		/** Set the State of the LED */
		hw.SetLed(Hardware::LED_FREQ, 0.f, 0.f, led_state);

		/** Toggle the state */
		if (led_state)
			led_state = false;
		else
			led_state = true;

		/** Write all LED states to the hardware */
		hw.UpdateDriverLeds();

		/** Delay 500ms */
		System::Delay(500);
	}
}
