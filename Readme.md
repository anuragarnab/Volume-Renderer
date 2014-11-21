Introduction:
======

This is a GUI made in Qt that includes the previous Volume Renderer. This project uses the Model-View-Controller design pattern. There is a "Control Window" where input parameters are entered via the user and there is a "Rendering Window" where the output is displayed


File list:
======
The source files are:

	controlwindow.cpp / .h
	renerwindow.cpp / .h
	glWidget.cpp / .h
	TransformationManager.cpp / .h
	xmlparser.cpp / .h
	filteroption.cpp / .h
	busywindow.cpp / .h
	main.cpp
	config.h

Compiling and running:
======

A Visual Studio project and a QtCreator project is provided in the folders "Visual Studio" and "QtCreator" respectively.

The QtCreator project can be compiled by typing "qmake" into the terminal. This will create a makefile which can then be compiled by typing "make"

In order to compile on Windows, the preprocessor directive in "config.h", #DEFINE WINDOWS is required. For Linux, this line should be commented out.

Functionality:
======

There are five options in the Control Window:

	1. Upload OCT
===

		A dialog will open prompting the user to enter the first image file in the sequence. The program will then figure out the total number of images in the sequence. These images will then be displayed in the pane titled "Loaded image". Images can be changed using the slider on top or arrow keys on the keyboard

		Note, for images to be read in sequence, each image must end with a number. For example, if the input image is "test001.png", then the program will work out how many images exist in the range 001 to 999.

		The program can display all images supported by Qt

	2. Render filter
===

		Pressing this button will show a drop down menu with four selectable filters. Selecting a filter will then prompt for filter parameters. The number of filter options can easily be changed by altering "N_FILTERS" in the source file "config.h"

		Four basic filters are implemented. In order, they are Grayscale, Brightness/Darkness, Blur and Changing Saturation

		The filters will only work on images with R, G and B channels. 

		Update:
---
		The various filter options can now be specified by an external xml file. By default, this file is called "config.xml", but can be changed by changing "XML_FILE" in "config.h". 

		Refer to the sample xml file provided for more details on what the format of the xml file should be.

		To summarise, the format is as follows

		<option>
			<option id = 1>
				<name> Name of filter </name>
				<description> Description of the filter </description>
				<parameters>
					<description> Description of parameter 1 </description>
					<description> Description of parameter 2 </description>
					....
				</parameters>
			<option>
		</options>

		Parameters do not have to be specified if there are none


		Adding functionality to the filters:
---

		This does not require having to connect any signals or slots. 

		Firstly, one needs to define a processing function that has the signature:

			void functionName (QString parameters, QImage * image)

		where "parameters" is a space-separated string of the values which were entered into the various line-edit boxes of the filter.

		Then to link it up, the function pointer array needs to be populated with the new function. This is done in the constructor of the RenderWindow class. For example, 

			"processingFunctions[4] = &RenderWindow::functionName". 

		The number, 4, corresponds to the fifth filter option selected by the user. Be wary to always have functions to deal with the number of possible filter options. So when the fifth radio button is clicked, or when text is entered into the line-edit widgets corresponding to the radio buttons, this function will be invoked.

		To see how this works, follow the flow of the code from "ControlWindow::initialiseRenderOptions", in "controlwindow.cpp"


	3. Volume rendering
===

		Volume rendering will be performed based on the image slices that were selected using the 'Upload OCT' command. There are two adjustable parameters - Alpha Scaling and Alpha Threshold. For more information about this, refer to the documentation of the original volume renderer. 

		The volume renderer in this version of the program has been stripped down slightly by the fact that perspective projection is no longer enabled.

	4. Compute 2D Fingerprint
=== 

		This is just a stub. Pressing the button will print out to the console that a slot to handle this button has been called

	5. Enhance Fingerprint
===

		This is just a stub. Pressing the button will print out to the console that a slot to handle this button has been called


Additional Notes:
======

Creating the Volume Renderer:

The glWidget (which is the volume renderer), takes a long time (up to 10 seconds), to initialise as it most generate textures from image files which must then be loaded into the graphics card's memory. 

Whilst this is initialising, the GUI is in a "frozen" state and is not responsive to user interaction. A solution to this problem would be to create the widget in a separate thread so that the GUI thread is not interrupted. However, in this particular case, it is not possible with Qt. This is because Qt asserts that only the "GUI thread" is allowed to update the display - this includes creating the volume renderer widget. Trying to create the widget in a separate thread will result in an assertion failure. 

Another partial solution (which was implemented), was to display a window saying something along the lines of "Busy loading textures, please wait". 

However, Qt, draws widgets to the display asynchronously. As a result, creating the "Busy Window" just before creating the glWidget can result in the "Busy Window" not being drawn completely. This is because once Qt starts creating the glWidget, it might not come back to the other drawing events in the queue (rendering the "Busy Window". 

The solution to this problem was to call "QCoreApplication::processEvents()"; after creating the Busy Window, but before the call to the constructor of glWidget. 
". This function call ensures that all drawing events in Qt's queue are completed first. As a result, it ensures that the Busy Window is drawn completely before construction of the glWidget begins. 

Using this idea, calling "QCoreApplication::processEvents()", inside the glWidget whilst it is loading textures could be seen as a way to maintain GUI responsiveness as each time this line is called, Qt will complete all GUI events that would have filled the queue whilst the glWidget was being created. However, the problem with this is that it can cause OpenGL errors. As a result, this was not done. 


Window Size
======

The window size is fixed. The total size of the window is defined in "config.h" as WINDOW_WDITH and WINDOW_HEIGHT. The height of the volume rendering widget is defined as VOL_REND_HEIGHT. The difference between the window height and volume rendering height is used for the image views.