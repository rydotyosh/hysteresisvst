Thank you for download!

This VST plug-in is an effector using the Preisach model of hysteresis. This provides distortion based on the hysteresis effect.

To build this, you need VST SDK Version 2.4 from Steinberg. Build the SDK files to make libs, and put libs in our root directory in our project. Put include files in SDK to the vst directory in our project. You might get the directory tree like below:

hysteresisvst
	vst
		+aeffect.h
		+aeffectx.h
		+aeffeditor.h
		...
		
	+hysteresis.cpp
	+hysteresis.h
	+hysteresismain.cpp
	...
	+vstbase.lib
	+vstbased.lib

Ryogo Yoshimura (ryogo.yoshimura@gmail.com)
