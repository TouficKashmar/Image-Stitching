
The script main.cpp was written using OpenCV version 4.2.0 in C++.

To run the program, open Code:Blocks, load the file (main.cpp) then click on build. Once the build is complete, click on run to run the program.

I have included 3 sets of input photos, in the "InputImages" folder, where each set is named accordingly (i.e. the first set is named Set1). To try out different set, simply change the variable "setNumber" in the beginning of the code.

The code can be run without any modification, but I thought adding the "setNumber" variable would make it easier to try out different pairs of images.

Similarly, all the output images are stored in the folder "OutputImages", where the output of each pair of images is stored under its own folder, named accordingly.

In this project, we implemented Agast feature detector, LUCID descriptor and BruteForce matcher for the matching algorithm. We also used warpPerspective to stitch images together.

Note that in the include part of the code, two "includes" had to be modified by listing the absolute path instead of their regular names due to some errors in codeblocks. 

This project was developped and tested on a HP-Laptop 15-ce0xx, System Type	x64-based PC, Processor Intel(R) Core(TM) i7-7700HQ CPU @ 2.80GHz, 2808 Mhz, 4 Core(s), 8 Logical Processor(s), OS Name: Microsoft Windows 10 Home. We used CodeBlocks IDE to write the code.
