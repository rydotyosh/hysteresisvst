//-------------------------------------------------------------------------------------------------------
// VST Plug-In
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST filter
// Filename     : hysteresis.cpp
// Created by   : Ryogo Yoshimura
// Description  : Hysteresis filter
//
// (c) Ryogo Yoshimura, All Rights Reserved
// MIT License
//-------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "hysteresis.h"

//


//----------------------------------------------------------------------------- 
HysteresisProgram::HysteresisProgram ()
{
	// default Program Values
	param.fInputGain=0.25;
	param.iFeedbackLoop=1;
	param.fPreGain=0.25;
	param.iBoost=0;
	param.fParam1=0.5;
	param.fPostGain=0.5;
	param.fOutputGain=0.25;

	strcpy (name, "Init");
}

//-----------------------------------------------------------------------------
Hysteresis::Hysteresis (audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
	, active(false)
{
	// init

	internalhysteresis[0].resize(100);
	internalhysteresis[0].inithalf();

	internalhysteresis[1].resize(100);
	internalhysteresis[1].inithalf();

	delayentity[0]=0;
	delayentity[1]=0;
	
	programs = new HysteresisProgram[numPrograms];

	if (programs)
		setProgram (0);

	setNumInputs (2);	// stereo input
	setNumOutputs (2);	// stereo output

	setUniqueID ('HyRy');	// this should be unique, use the Steinberg web page for plugin Id registration

	resume ();		// flush buffer

	active=true;
}

//------------------------------------------------------------------------
Hysteresis::~Hysteresis ()
{
	if (programs)
		delete[] programs;
}

//------------------------------------------------------------------------
void Hysteresis::setProgram (VstInt32 program)
{
	HysteresisProgram* ap = &programs[program];

	curProgram = program;
	setParameter (kInputGain,ap->param.fInputGain);
	setParameter (kFeedbackLoop,ap->param.iFeedbackLoop);
	setParameter (kPreGain, ap->param.fPreGain);
	setParameter (kBoost, ap->param.iBoost);
	setParameter (kParam1,ap->param.fParam1);
	setParameter (kPostGain, ap->param.fPostGain);
	setParameter (kOutputGain, ap->param.fOutputGain);
}

//------------------------------------------------------------------------
void Hysteresis::setProgramName (char *name)
{
	strcpy (programs[curProgram].name, name);
}

//------------------------------------------------------------------------
void Hysteresis::getProgramName (char *name)
{
	if (!strcmp (programs[curProgram].name, "Init"))
		sprintf (name, "%s %d", programs[curProgram].name, curProgram + 1);
	else
		strcpy (name, programs[curProgram].name);
}

//-----------------------------------------------------------------------------------------
bool Hysteresis::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text)
{
	if (index < kNumPrograms)
	{
		strcpy (text, programs[index].name);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
void Hysteresis::resume ()
{
	resethysteresis();
	AudioEffectX::resume ();
}

//------------------------------------------------------------------------
void Hysteresis::setParameter (VstInt32 index, float value)
{
	switch (index)
	{
		case kInputGain:setInputGain(value);break;
		case kFeedbackLoop:setFeedbackLoop(value);break;
		case kPreGain : setPreGain(value); break;
		case kBoost : setBoost(value); break;
		case kParam1:setParam1(value);break;
		case kPostGain : setPostGain(value); break;
		case kOutputGain:setOutputGain(value);break;
	}
}

//------------------------------------------------------------------------
float Hysteresis::getParameter (VstInt32 index)
{
	float v = 0;

	switch (index)
	{
		case kInputGain : v = getInputGain(); break;
		case kFeedbackLoop : v = getFeedbackLoop(); break;
		case kPreGain : v = getPreGain(); break;
		case kBoost : v = getBoost(); break;
		case kParam1:v = getParam1();break;
		case kPostGain : v = getPostGain(); break;
		case kOutputGain : v = getOutputGain(); break;
	}
	return v;
}

//------------------------------------------------------------------------
void Hysteresis::getParameterName (VstInt32 index, char *label)
{
	switch (index)
	{
		case kInputGain : strcpy (label, "Input Gain");	break;
		case kFeedbackLoop : strcpy (label, "Feedback Loop");	break;
		case kPreGain : strcpy (label, "Pre Gain");	break;
		case kBoost : strcpy (label, "x10");	break;
		case kParam1:strcpy (label, "Hysteresis Param");	break;
		case kPostGain : strcpy (label, "Post Gain");	break;
		case kOutputGain : strcpy (label, "Output Gain");	break;
	}
}

//------------------------------------------------------------------------
void Hysteresis::getParameterDisplay (VstInt32 index, char *text)
{
	switch (index)
	{
		case kInputGain : dB2string (param.fInputGain*4, text, kVstMaxParamStrLen);	break;
		case kFeedbackLoop : if(param.iFeedbackLoop){strcpy(text,"On");}else{strcpy(text,"Off");}break;
		case kPreGain : dB2string (param.fPreGain*4, text, kVstMaxParamStrLen);	break;
		case kBoost : if(param.iBoost){strcpy(text,"On");}else{strcpy(text,"Off");}break;
		case kParam1:float2string (param.fParam1, text, kVstMaxParamStrLen);	break;
		case kPostGain : float2string (param.fPostGain*8-4, text, kVstMaxParamStrLen);	break;
		case kOutputGain : dB2string (param.fOutputGain*4, text, kVstMaxParamStrLen);	break;
	}
}

//------------------------------------------------------------------------
void Hysteresis::getParameterLabel (VstInt32 index, char *label)
{
	switch (index)
	{
		case kInputGain : strcpy (label, "dB");	break;
		case kFeedbackLoop : strcpy (label, "");	break;
		case kPreGain : strcpy (label, "dB");	break;
		case kBoost : strcpy (label, "");	break;
		case kParam1:strcpy (label, "");	break;
		case kPostGain : strcpy (label, "");	break;
		case kOutputGain : strcpy (label, "dB");	break;
	}
}

//------------------------------------------------------------------------
bool Hysteresis::getEffectName (char* name)
{
	strcpy (name, "Hysteresis filter");
	return true;
}

//------------------------------------------------------------------------
bool Hysteresis::getProductString (char* text)
{
	strcpy (text, "Hysteresis filter");
	return true;
}

//------------------------------------------------------------------------
bool Hysteresis::getVendorString (char* text)
{
	strcpy (text, "jyoken music works");
	return true;
}

//---------------------------------------------------------------------------
void Hysteresis::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	boost::mutex::scoped_lock sync(mtx);

	if(!active)return;

	float bst=(param.iBoost)?10:1;
	if(!param.iFeedbackLoop)// simple hysteresis
	{
		for(int k=0;k<2;++k)
		{
			for(int i=0;i<sampleFrames;++i)
			{
				outputs[k][i]=
					(
						internalhysteresis[k].op
							(inputs[k][i]*param.fInputGain*4*bst*0.5+0.5)
						*2-1
					)
					*param.fOutputGain*4;
			}
		}
	}
	else// hysteresis IIR
	{
		for(int k=0;k<2;++k)
		{
			for(int i=0;i<sampleFrames;++i)
			{
				double o=
					(
						inputs[k][i]*param.fInputGain*4
						+ delayentity[k]*(param.fPostGain*8-4)
					);
				outputs[k][i]=o*param.fOutputGain*4;
				delayentity[k]=
					(
						internalhysteresis[k].op
							(o*param.fPreGain*4*bst*0.5+0.5)
						*2-1
					);
			}
		}
	}
}

void Hysteresis::setInputGain(float g)
{
	HysteresisProgram* ap = &programs[curProgram];
	boost::mutex::scoped_lock sync(mtx);
	ap->param.fInputGain=param.fInputGain=g;
}

float Hysteresis::getInputGain()
{
	return param.fInputGain;
}

void Hysteresis::setFeedbackLoop(float n)
{
	HysteresisProgram* ap = &programs[curProgram];
	boost::mutex::scoped_lock sync(mtx);
	ap->param.iFeedbackLoop=param.iFeedbackLoop=(n>0.5)?1:0;
}

float Hysteresis::getFeedbackLoop()
{
	return param.iFeedbackLoop;
}

void Hysteresis::setPreGain(float g)
{
	HysteresisProgram* ap = &programs[curProgram];
	boost::mutex::scoped_lock sync(mtx);
	ap->param.fPreGain=param.fPreGain=g;
}

float Hysteresis::getPreGain()
{
	return param.fPreGain;
}

void Hysteresis::setBoost(float n)
{
	HysteresisProgram* ap = &programs[curProgram];
	boost::mutex::scoped_lock sync(mtx);
	ap->param.iBoost=param.iBoost=(n>0.5)?1:0;
}

float Hysteresis::getBoost()
{
	return param.iBoost;
}

void Hysteresis::setParam1(float p)
{
	HysteresisProgram* ap = &programs[curProgram];
	ap->param.fParam1=param.fParam1=p;
	boost::mutex::scoped_lock sync(mtx);
	internalhysteresis[0].setparam1(param.fParam1);
	internalhysteresis[1].setparam1(param.fParam1);
}
float Hysteresis::getParam1()
{
	return param.fParam1;
}

void Hysteresis::setPostGain(float g)
{
	HysteresisProgram* ap = &programs[curProgram];
	boost::mutex::scoped_lock sync(mtx);
	ap->param.fPostGain=param.fPostGain=g;
}

float Hysteresis::getPostGain()
{
	return param.fPostGain;
}

void Hysteresis::setOutputGain(float g)
{
	HysteresisProgram* ap = &programs[curProgram];
	boost::mutex::scoped_lock sync(mtx);
	ap->param.fOutputGain=param.fOutputGain=g;
}

float Hysteresis::getOutputGain()
{
	return param.fOutputGain;
}

void Hysteresis::resethysteresis()
{
	internalhysteresis[0].inithalf();
	internalhysteresis[1].inithalf();
}
