# Pd for the Aurora

[pd2dsy](https://github.com/electro-smith/pd2dsy) is a tool that allows you to turn regular Pd patches into [Daisy-driven](https://www.electro-smith.com/daisy/daisy) firmware. Since the Aurora is driven by a Daisy seed, that means you can control it with Pd!

Once you've gone through the [process of installing pd2dsy](https://github.com/electro-smith/pd2dsy#installationsetup), copy the `aurora.json` file (in the Common folder) and the `aurora_test.pd` file over to your pd2dsy installation folder. This will simplify the command for compiling them quite a bit. You might also want to open up the `aurora_test.pd` patcher to get an idea of what the program will do once it's loaded onto the Aurora.

## Programming the Aurora

Typically, programs created with pd2dsy are flashed using the Daisy's on-board Micro USB port. Since that's at the back of the Aurora, though, that would be a bit of a pain to access. Instead, we can take advantage of the Daisy Bootloader that comes installed on every Aurora and upload Pd programs over the front USB. (Do note that the bootloader should never be overwritten, so definitely _don't_ follow the normal Pd fashing procedure.)

Once you've copied over the two files mentioned above, open up a terminal and navigate to your pd2dsy installation. From there, you can follow these steps:

1. Copy this command: `python3 pd2dsy.py -c aurora.json --rom size aurora_test.pd`, paste it into your terminal and hit enter.
2. Once it's done compiling, you should see the build folder in the same directory (`aurora_test`). Open it up in your file browser.
3. From the `build` subdirectory, copy the singular `.bin` file (`HeavyDaisy_aurora_test.bin`) onto a flash drive.
4. Take your flash drive out and plug it into the Aurora.
5. Give the Aurora a power cycle.

After about five seconds, your patch should be running on the Aurora!

## Additional Notes

Your python installation may not use `python3`. On Windows, the system-wide launcher is typically `py`, so the command above would instead be: `py pd2dsy.py -c aurora.json --rom size aurora_test.pd`. 

If you're interested in learning what each of the options in the pd2dsy command means, the pd2dsy README has a pretty detailed rundown.
