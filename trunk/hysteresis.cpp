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
	param.fGain=0.25;
	param.iNdiv=1000;
	param.fParam1=0.5;

	strcpy (name, "Init");
}

//-----------------------------------------------------------------------------
Hysteresis::Hysteresis (audioMasterCallback audioMaster)
	: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
	// init

	internalhysteresis[0].resize(1000);
	internalhysteresis[0].inithalf();
	//internalhysteresis[0].genmu();

	internalhysteresis[1].resize(1000);
	internalhysteresis[1].inithalf();
	//internalhysteresis[1].genmu();
	
	programs = new HysteresisProgram[numPrograms];

	if (programs)
		setProgram (0);

	setNumInputs (2);	// stereo input
	setNumOutputs (2);	// stereo output

	setUniqueID ('HyRy');	// this should be unique, use the Steinberg web page for plugin Id registration

	resume ();		// flush buffer
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
	setParameter (kGain, ap->param.fGain);
	setParameter (kNdiv, ap->param.iNdiv);
	setParameter (kParam1,ap->param.fParam1);
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
	//HRTFProgram* ap = &programs[curProgram];

	switch (index)
	{
		case kGain : setGain(value); break;
		case kNdiv : setNdiv(value); break;
		case kParam1:setParam1(value);break;
	}
}

//------------------------------------------------------------------------
float Hysteresis::getParameter (VstInt32 index)
{
	float v = 0;

	switch (index)
	{
		case kGain : v = getGain(); break;
		case kNdiv : v = getNdiv(); break;
		case kParam1:v = getParam1();break;
	}
	return v;
}

//------------------------------------------------------------------------
void Hysteresis::getParameterName (VstInt32 index, char *label)
{
	switch (index)
	{
		case kGain : strcpy (label, "Gain");	break;
		case kNdiv : strcpy (label, "Ndiv");	break;
		case kParam1:strcpy (label, "Param1");	break;
	}
}

//------------------------------------------------------------------------
void Hysteresis::getParameterDisplay (VstInt32 index, char *text)
{
	switch (index)
	{
		case kGain : dB2string (param.fGain*4, text, kVstMaxParamStrLen);	break;
		case kNdiv : int2string (param.iNdiv, text, kVstMaxParamStrLen);	break;
		case kParam1:float2string (param.fParam1, text, kVstMaxParamStrLen);	break;
	}
}

//------------------------------------------------------------------------
void Hysteresis::getParameterLabel (VstInt32 index, char *label)
{
	switch (index)
	{
		case kGain : strcpy (label, "dB");	break;
		case kNdiv : strcpy (label, "");	break;
		case kParam1:strcpy (label, "");	break;
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
	Rydot::synchronized sync(cs);
	for(int k=0;k<2;++k)
	{
		for(int i=0;i<sampleFrames;++i)
		{
			outputs[k][i]=internalhysteresis[k].op(inputs[k][i]*param.fGain*4*0.5+0.5)*2-1;
		}
	}
}

void Hysteresis::setGain(float g)
{
	HysteresisProgram* ap = &programs[curProgram];
	ap->param.fGain=param.fGain=g;
}

float Hysteresis::getGain()
{
	return param.fGain;
}

void Hysteresis::setNdiv(float n)
{
	HysteresisProgram* ap = &programs[curProgram];
	n*=2000;
	if(n>2000)n=2000;
	if(n<3)n=3;
	ap->param.iNdiv=param.iNdiv=n;
	Rydot::synchronized sync(cs);
	internalhysteresis[0].resize(param.iNdiv);
	internalhysteresis[1].resize(param.iNdiv);
}

float Hysteresis::getNdiv()
{
	return param.iNdiv/2000.0;
}

void Hysteresis::setParam1(float p)
{
	HysteresisProgram* ap = &programs[curProgram];
	ap->param.fParam1=param.fParam1=p;
	Rydot::synchronized sync(cs);
	internalhysteresis[0].setparam1(param.fParam1);
	internalhysteresis[1].setparam1(param.fParam1);
}
float Hysteresis::getParam1()
{
	return param.fParam1;
}


void Hysteresis::resethysteresis()
{
	internalhysteresis[0].inithalf();
	internalhysteresis[1].inithalf();
}