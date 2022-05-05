# Oopsy for the Aurora

[Oopsy is a flexible tool](https://github.com/electro-smith/oopsy) that allows you to turn [Max/MSP Gen~](https://docs.cycling74.com/max8/vignettes/gen_topic) patches into [Daisy-driven](https://www.electro-smith.com/daisy/daisy) firmware. Since the Aurora is driven by a Daisy seed, that means you can control it with Max!

Once you've gone through the [process of installing Oopsy](https://github.com/electro-smith/DaisyWiki/wiki/1e.-Getting-Started-With-Oopsy-(Gen~-Integration)#getting-started-with-oopsy-maxmsp-gen-integration), go ahead and open up the `aurora_test.maxpat` patcher. It provides an example for using all the Aurora's I/O, so it'll be easy to tell if you've got everything set up right.

## Programming the Aurora

Typically, programs created with Oopsy are flashed using the Daisy's on-board Micro USB port. Since that's at the back of the Aurora, though, that would be a bit of a pain to access. Instead, we can take advantage of the Daisy Bootloader that comes installed on every Aurora and upload Oopsy programs over the front USB. (Do note that the bootloader should never be overwritten, so definitely _don't_ follow the normal Oopsy fashing procedure.)

Before you try to compile your patch, make sure you've selected the `aurora.json` file by pressing the `Browse` button on the Oopsy object and navigating to `Common/aurora.json`. With that done, you should see the full path to the JSON file in the dropdown to the left of `Browse`.

From this point, you can compile the program and load it onto the Aurora:
1. Press the button on the Oopsy object or save your patch. Once the compilation is done, it'll complain about `daisy not ready on USB`, but that's to be expected. 
2. Click on the `Build` button to open a file browser directly at the project's build folder. Find the file with the `.bin` extension (there should only be one) and drag this onto a flash drive.
3. Take your flash drive out and plug it into the Aurora.
4. Give the Aurora a power cycle.

After about five seconds, your patch should be running on the Aurora!