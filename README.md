<img data-image-dimensions="3840x1160" data-image-focal-point="0.5,0.5" alt="" data-load="false" data-image-id="6297f18d481b206fcb8702d3" data-type="image" style="left: 0%; top: 0%; width: 100%; height: 100.%; position: top;" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/11c77c89-b9d3-4cab-a92d-d932dc89e65b/Aurora_SDK_banner_v1.0.png?format=2500w">

[![Build All](https://github.com/Qu-Bit-Electronix/Aurora-SDK/actions/workflows/main.yml/badge.svg)](https://github.com/Qu-Bit-Electronix/Aurora-SDK/actions/workflows/main.yml)
[![Documentation](https://github.com/Qu-Bit-Electronix/Aurora-SDK/actions/workflows/docs.yml/badge.svg)](https://Qu-Bit-Electronix.github.io/Aurora-SDK)

Everything you need to start writing your own application for the Qu-Bit Aurora Hardware platform!

Check out the [reference documentation](Qu-Bit-Electronix.github.io/Aurora-SDK) for the project.

Below we'll cover some basic topics to get started writing your own code to run on the Aurora

# Table of Contents

* [Compiling the Examples](#compiling-the-examples)
  * [Installing the Toolchain](#installing-the-toolchain)
  * [Cloning the SDK](#cloning-the-sdk)
  * [Building the Libraries](#building-the-libraries)
  * [Building the Examples](#building-the-examples)
* [Reading and Running Examples](#reading-and-running-examples)
  * [Blink](#blink)
  * [Volume](#volume)
  * [Ringmod](#ringmod)
* [Going Further](#going-further)
  * [Copying and Modifying an Example](#copying-and-modifying-an-example)
  * [Creating Your Own Firmware Project](#creating-your-own-firmware-project)
  * [Sharing Firmware With the Community](#sharing-firmware-with-the-community)

# Compiling the Examples

To begin, let's get the development environment set up to start working with your Aurora!

Overall, this process is pretty simple, regardless of what operating system you're using.

## Installing the Toolchain

The "toolchain" is a bundle of tools used to take the source code, and turn it into a binary file that can load onto the Aurora (or any other Daisy) hardware.

If you've already done some development with Daisy in the past, you should already be good to go, and can skip ahead to the next section.

On any operating system, our recommended text editor is VS Code, but you can work in any environment you're comfortable with. That said, we will only cover the specifics of working with VS Code in this guide.

### Windows

<!-- **TODO**: add real link -->

1. Download, and run the [Daisy Toolchain for Windows installer]() from Qu-bit website.
2. Download, and run the [Git for Windows installer](https://git-scm.com/download/win).

This installs the required tools to get up and running with daisy.

There are some additional helper scripts that require python, but this is optional.

On Windows, you can install python by downloading the latest from [python.org](https://www.python.org/downloads/).

**Note**: The windows store version of python will not work.

### Mac OS

<!-- **TODO**: add real link -->

1. Download the [Daisy Toolchain for Mac OS installer]() from the Qu-Bit website.
2. Unzip, and Double click, the `install.command` file contained within.

There are some additional helper scripts that require python, but this is optional.

Mac OS comes with python, but the latest version can be downloaded from [python.org](https://www.python.org/downloads/), or using homebrew.

## Cloning the SDK

<img data-image-focal-point="0.5,0.5" style="width: 100%; height: 100%; object-position: 50% 50%; object-fit: cover; cursor: auto;" data-parent-ratio="3.8" class="" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/d4426128-bfe8-45b6-9876-128c43f342d6/cloning_repo_meme.png?format=2500w">

The Aurora-SDK is a github repo, and it uses a few libraries to provide access to the hardware, and bundles the [DaisySP](https://github.com/electro-smith/DaisySP) DSP library.

To download the entire SDK with all of its libraries:

First, open a terminal, and navigate to the desired location on your computer. For our purposes here we'll use the `Desktop` folder, but you may wish to use `Documents`, `Developer`, or some other folder of your choice.

Now, paste the following command into the terminal and press enter:

```shell
git clone https://github.com/qu-bit-electronix/Aurora-SDK --recurse-submodules
```

Once its done you'll have an Aurora-SDK full of everything you need to start writing your own code.

## Building the Libraries

The Aurora-SDK uses a few libraries to interface with the hardware, and provide a bunch of DSP that can be used within your projects. These need to be compiled before we can build any examples.

The only time, other than after cloning the repo, that this needs to happen is when updating the libraries to a newer version.

If you're using VS Code you can open the task menu by clicking: `Terminal->Run Task...` and selecting "Build Libraries" from the menu.

This is the equivalent of running the following command in a terminal from the Aurora-SDK:

```shell
./ci/build_libs.sh
```

## Building the Examples

<img data-image-focal-point="0.5,0.5" style="width: 100%; height: 100%; object-position: 50% 50%; object-fit: cover; cursor: auto;" data-parent-ratio="4.3" class="" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/2027f19b-a2e5-406e-8df5-419ab42b116e/Build_animation_2.gif?format=2500w">

Each example is a single C++ file, and a `Makefile`.

If you're using VS Code, you can build all examples by running the build task by clicking: `Terminal->Run Build Task...`. The shortcut for this is `CTRL+SHIFT+B` on Windows, or `CMD+SHIFT+B` on Mac OS.

Alternatively, using the same task menu as above, you can build individual examples. For example, the task, `Build Example - Blink` will build the Blink example.

This is the equivalent of running the following command in a terminal from the specific Example's folder:

```shell
make
```

# Reading and Running Examples

One of the best ways to see how stuff works is to try it out!

In the `dist/Examples` folder is a precompiled version of each of the available examples, and the `Examples/` folder has the corresponding source code.

Before we get into installing any tools or anything, let's take a moment to look over some of the basic examples, and see how to interact with the module hardware in C++ (it's easier than you might think)!

To load one of the examples just drag the .bin file of your choice onto your USB drive, and power up the Aurora with it inserted.

**Note**: In order to ensure that you're loading the desired program, you should have only one `.bin` file in the root directory of the USB drive. (you can keep as many as you want in folders, though!)

<!-- **TODO**: ADD AN ACTUAL PLACE FOR THIS!! -->

The most up to date, official Aurora firmware can always be downloaded from [the Aurora product page](https://www.qubitelectronix.com/shop/aurora)

## Blink

<img data-image-focal-point="0.5,0.5" style="width: 100%; height: 100%; object-position: 50% 50%; object-fit: cover; cursor: auto;" data-parent-ratio="3.1" class="" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/678ab0ed-cab7-44c1-a089-19c64ab68231/Aurora_illustrated_LED.gif?format=2500w">

In this example, we're just periodically changing the state of an LED. No audio, no controls.

Here, we can see a few things that will happen in every program:

We always have a `Hardware` object, that's our connection to the module itself. Typically, we'll name it `hw` to keep it nice and short.

We can use that to do things with the Hardware. For example, in this example, we change the state of an LED:

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

Check out the [full example code here](https://github.com/Qu-Bit-Electronix/Aurora-SDK/blob/main/Examples/Blink/Blink.cpp)

In more complex projects we'll want to avoid using delays, but we'll get into techniques for doing that later.

## Volume

<img data-image-focal-point="0.5,0.5" style="width: 100%; height: 100%; object-position: 50% 50%; object-fit: cover; cursor: auto;" data-parent-ratio="4.2" class="" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/d0123cf8-7f65-451a-8c5e-2cd62bc653ed/Mix_animation.gif?format=2500w">

In this example, we're going to add two new things to what our previous example did: audio, and knobs!

That's right, the most exciting eurorack concept ever invented -- a volume control.

Just like in our last example, we're still going to create our `Hardware` object, and initialize it.

However, now we're going to start up a new "callback" for handling audio.

To define the the audio callback we write a function like this:

```cpp
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
}
```

The function can have any name, but for clarity we'll simply call it `AudioCallback` within the example.

The three arguments are:

* in: real time stereo audio input from the hardware
* out: real time stereo audio output from the hardware
* size: the number of individual samples included in the buffers

The audio default audio configuration is set up with an array of samples per channel, like this:

```cpp
{
    { L0, L1, L2, . . ., LN },
    { R0, R1, R2, . . ., RN }
}
```

So using these arguments, we can loop over each sample of audio, individually.

For example, just passing the input straight through to the output:

```cpp
for (size_t i = 0; i < size; i++)
{
    /** Left channel */   
    out[0][i] = in[0][i];
    /** Right channel */
    out[1][i] = in[1][i];
}
```

We want **control**, through. So what we want to do is use the value of one of the knobs to scale that level.

Thie is pretty easy to setup. Within the callback, we'll want to process all of our controls, and then we can simply assign one of them to a variable called, "volume".

Here we'll use the mix knob to get a 0-1 value.

```cpp
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    hw.ProcessAllControls();
    float volume = hw.GetKnobValue(KNOB_MIX);
}
```

Knobs and CVs are handled separately to allow for more complex user interfaces, but there is an equivalent `GetCvValue` function that works in the same way.

Within the `for` loop, we can scale the input by this amount to control the volume of our signal.

In total, we now have the following Audio Callback:

```cpp
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    hw.ProcessAllControls();
    float volume = hw.GetKnobValue(KNOB_MIX);
    for (size_t i = 0; i < size; i++)
    {
        /** Left channel */   
        out[0][i] = in[0][i] * volume;
        /** Right channel */
        out[1][i] = in[1][i] * volume;
    }
}
```

For a bit more detail on how audio works on the Daisy, check out the [Getting Started - Audio](https://electro-smith.github.io/libDaisy/md_doc_md__a3__getting__started__audio.html) guide.

And check out the [full example code here](https://github.com/Qu-Bit-Electronix/Aurora-SDK/blob/main/Examples/Volume/Volume.cpp)


## Ringmod

<img data-image-focal-point="0.5,0.5" style="width: 100%; height: 100%; object-position: 50% 50%; object-fit: cover; cursor: auto;" data-parent-ratio="4.2" class="" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/7900e6c4-20ad-4601-b5b7-923a668bec59/ringmod_animation.gif?format=2500w">

In this example, we're going to take what we learned in the Volume example, and make something a bit more exciting happen.

A ring modulator is a form of bipolar amplitude modulation that can be easily made by multiplying one signal (like our input signal), and another signal.

In this example we're going to break out a few things from the DaisySP DSP library.

We'll also be using the `fmap` utility function to help us scale our control's 0-1 values to a more desirable range for our controls.

For example, when setting our modulator frequency we'll want to have a logarthmic curve, and a frequency range from say, 10Hz to 1.5kHz.
Using `fmap` we can save ourselves from figuring out how to do that with math.

The fmap function take up to 4 arguments:

1. The 0-1 input to be converted (here we use `hw.GetKnobValue(KNOB_WARP)`)
2. The new minimum
3. The new maximum
4. The curve for the mapping function. If this isn't specified, it defaults to a normal, linear curve.

```cpp
float freq = fmap(hw.GetKnobValue(KNOB_WARP), 10.0, 1500.0, Mapping::LOG);
```

We'll also be using an `Oscillator` to generate a sine wave for our modulator.

The `Oscillator`, like nearly every other DaisySP object, has two core functions: `Init` and `Process`.

We want to initialize the oscillator, and set any particular parameters after initializing our Hardware object:

```cpp
osc.Init(hw.AudioSampleRate());
osc.SetWaveform(Oscillator::WAVE_SIN);
osc.SetAmp(1.0);
```

And then within our per-sample loop of the audio callback, we can render the oscillator's signal by calling it's `Process` function.

```cpp
for (size_t i = 0; i < size; i++) {
    float signal = osc.Process();
}
```

To do the actual ring modulation effect, we'll multiply that signal by our input signal.

Check out the [full example code here](https://github.com/Qu-Bit-Electronix/Aurora-SDK/blob/main/Examples/Ringmod/Ringmod.cpp)

# Going Further

Now that we can compile the provided examples, let's look at where we might want to go from here.

## Copying and Modifying an Example

A great starting point is to copy one of the existing examples, and making modifications.

For example, a good first modification to any of the existing examples would be to add the corresponding CV input to one of the knob controls.

Since the examples are only a pair of files each, all you really need to do is Copy/Paste the folder and change the name.

In the `Makefile` you'll want to:

* update the `TARGET` name (on line 2)
* and the `CPP_SOURCES` filename (on line 8) if you change the cpp file name.

To build your new file you'll either need to edit the `.vscode/tasks.json` file (copying the entry for "Build Example - X" to your new folder name), or to run the `make` command directly from the terminal

## Creating Your Own Firmware Project

Once you've dabbled a bit, and want to make your own custom firmware for yourself (or to share with the community), you can set some stuff up to start making your own folders.

If you're keeping your new project in the `Examples` folder you won't have to change much other than what was mentioned above.

However, if you want to have a dedicated folder (perhaps a github repo of your own) to keep your project, you may want to have your source files _outside_ of the Aurora-SDK folder.

In this situation, the only other thing you'll want to update is the `AURORA_SDK` path (line 13) of the Makefile to point to a copy of the Aurora-SDK somewhere on your comptuer. This path can be relative, or absolute.

If you're making you're making your project a github repo. You can add the SDK to your own repo as a submodule using:

```shell
git submodule add https://github.com/qu-bit-electronix/Aurora-SDK
```

and initialize it with:

```shell
git submodule update --init
```

## Sharing Firmware With the Community

<!-- TODO Copy here kinda sucks -->

Once you've got something cool you may want to share the binary, and/or the source code with the community!

The [Qu-Bit Forum](forum.qubitelectronix.com) is a great place to share your project along with any details about how it works.

Another great place to share your project is on [patchstorage](patchstorage.com).

[!(<img data-image-focal-point="0.5,0.5" style="width: 100%; height: 100%; object-position: 50% 50%; object-fit: cover; cursor: auto;" data-parent-ratio="4.3" class="" data-image-resolution="2500w" src="https://images.squarespace-cdn.com/content/v1/56feccc7a3360c08ffa34ed4/98f5d1fd-2c74-4353-bd2e-fce2d106bd0c/Forum_button.png?format=2500w">)](https:forum.qubitelectronix.com)
