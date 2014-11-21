README:
 
Introduction
=======================================================================================================
This is a port of the volume rendering code uploaded on http://www.codeproject.com/Articles/352270/Getting-started-with-Volume-Rendering?msg=4640510#xx4640510xx

The program reads in a series of images stored in a raw data format. The program reads a byte for every pixel. In the sample file, there are a 109 256x256 pixel images, resulting in a total file size of 6976 kB. The parsing code can easily be updated to read other types of images. This is in the function, initTextures3D(std::string filename), found in Main.cpp. As the sample data did not contain alpha values, they were taken as the luminosity values. Again, this can be modified easily in the function initTextures3D(std::string filename).

The user can rotate the model using either a mouse, or the W-A-S-D keys on the keyboard. The “backspace” key can be used to reset the rotation.

Configuration File:
=======================================================================================================

A configuration file is used to read in rendering properties. By default, it is called “config.txt”. To change this, modify the “CONFIG_FILE” entry in “Config.h” and recompile.

The config file provided is self-explanatory. Lines beginning with a “#” are ignored. This delimiter can be changed in “config.h”. Parameters are read in consecutive order, with each parameter on a new line. The commented lines help to describe what parameter is being entered.

In addition to the rendering options, the configuration file allows the user to change the “rotation reset key”, which must be entered in ASCII and also allows the window title and source file containing the images to be chosen. 

Compiling:
=======================================================================================================

Windows:

Open the solution in Visual Studio. All libraries required are contained in the project.

Linux:

Ensure you have the libraries freeGLUT and glew
Use the supplied makefile to compile

Instructions for Ubuntu:

sudo apt-get install freeglut3-dev
sudo apt-get install libglew-dev
make
./VolumeRenderer.exe


Known Issues:
=======================================================================================================

Running in VirtualBox:
The program was tested in Ubuntu running in VirtualBox.  To run, mesa-utils needs to be installed (sudo apt-get mesa-utils). To run the program, hardware acceleration may need to be disabled
(LIBGL_ALWAYS_INDIRECT=1 ./VolumeRenderer.exe)

Configuration files created in another OS:
Configuration files created in Windows may not be read correctly in Unix due to differences in carriage returns. This can result in the data file containing the images not being read even though it exists.

Data sets:
=======================================================================================================
Data sets can be found from here, as well as other locations linked on this page.

http://www.gris.uni-tuebingen.de/edu/areas/scivis/volren/datasets/datasets.html