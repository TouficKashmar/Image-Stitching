The script main.cpp was written using OpenCV version 4.2.0 in C++.
To run the program, open Code:Blocks, load the file (main.cpp) then click on build. Once the build is complete, click on run to run the program.

I have included 3 sets of input photos, in the "InputImages" folder, where each set is named accordingly (i.e. the first set is named Set1). 
To try out different set, simply change the variable "setNumber" in the beginning of the code. The code should run without any modification

Similarly, all the output images are stored in the folder "OutputImages", where the output of each pair of images is stored under its own folder, named accordingly.

In this project, we implemented Agast feature detector, FREAK descriptor and BruteForce matcher for the matching algorithm. We also used warpPerspective to stitch images together.sssssssss
Note that in the include part of the code, two "includes" had to be modified by listing the absolute path instead of their regular names.
