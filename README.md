# Aurora SDK

Everything you need to start writing your own application for the Qu-Bit Aurora Hardware platform!

Below we'll cover some basic topics to get started writing your own code to run on the Aurora

. . .

TODO: Add some info!

* [ ] Run the examples and overviews
* [ ] Flash the original firmware back onto the module
* [ ] Install the Toolchain
* [ ] Compile one of the examples
* [ ] How to interact with the UI
* [ ] Create your own example
* [ ] Create your own project to share on Github
* [ ] Share your firmware with the community!
* [ ] Debugging with an ST-Link (optional)

## Reading and Running Examples

One of the best ways to see how stuff works is to try it out!

In the `dist/Examples` folder is a precompiled version of each of the available examples, and the `Examples/` folder has the corresponding source code.

Before we get into installing any tools or anything, let's take a moment to look over some of the basic examples, and see how to interact with the module hardware in C++ (it's easier than you might think)!

To load one of the examples just drag the .bin file of your choice onto your USB drive, and power up the Aurora with it inserted.

**Note**: In order to ensure that you're loading the desired program, you should have only one `.bin` file in the root directory of the USB drive. (you can keep as many as you want in folders, though!)

### Blink

In this example, we're just periodically changing the state of an LED. No audio, no controls.

Here, we can see a few things that will happen in every program:

We always have a `Hardware` object, that's our connection to the module itself. Typically, we'll name it `hw` to keep it nice and short.

We can use that to do things, for example, in this example, we change the state of an LED:

```cpp
hw.SetLed(LED_FREEZE, 0.f, 0.f, led_state);
```

where the arguments are the LED we want to change followed by values for the red, green, and blue components of the light.

Next, we toggle that state by changing the variable.

Then we tell the hardware to write all changes to the LED values to the actual hardware:

```cpp
hw.WriteLeds();
```

Finally, we tell tell everything to wait 500 milliseconds (or 0.5 seconds) before looping back to the top of the infinite loop:

```cpp
System::Delay(500);
```

In more complex projects we'll want to avoid using delays, but we'll get into techniques for doing that later.

### Volume

In this example, we're going to add two new things to what our previous example did: audio, and knobs!

That's right, the most exciting eurorack concept ever invented -- a volume control.

TODO: Finish this (once we've added that example).
