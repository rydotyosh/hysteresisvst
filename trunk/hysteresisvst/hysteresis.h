//-------------------------------------------------------------------------------------------------------
// VST Plug-In
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST filter
// Filename     : hysteresis.h
// Created by   : Ryogo Yoshimura
// Description  : Hysteresis filter
//
// (c) Ryogo Yoshimura, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#pragma once

#include "vst/audioeffectx.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <cthread.h>

extern void* hInstance;

enum
{
	// Global
	kNumPrograms = 16,

	// Parameters Tags
	kGain=0,
	kNdiv,
	kParam1,

	kNumParams
};

class Hysteresis;

//------------------------------------------------------------------------
class Param
{
public:
	float fGain;
	int iNdiv;
	float fParam1;
};

//------------------------------------------------------------------------
class hys
{
private:
	int N;
	std::vector<int> multirangerelays;
	std::vector<double> mu;
	double mumax;
	double param1;
public:
	hys(int n)
	{
		N=n;multirangerelays.resize(N-1);
	}
	hys()
	{
		N=0;
	}
	void setparam1(float p){param1=p;genmu();}
	void resize(int n)
	{
		N=n;
		multirangerelays.resize(N-1);
		//mu.resize(N);
		genmu();
	}
	void genmu()
	{
		std::vector<double> mu0;
		mu0.resize(N);
		mu.resize(N);
		mumax=0;
		double sum=0;
		for(int i=0;i<N;++i)
		{
			double pi=3.141592;
			double x=(double)i/N;
			mu0[i]=exp(-(double)i/N*20*param1);
			//mu0[i]=1;
			//mu0[i]=exp((double)i/N*2);
			//mu0[i]=pow(10*(x-x*x)+0,8);
			//mu0[i]=sin(x*3.14*2)+2*pow((x-x*x),1);
			//mu0[i]=-17*atan(2*pi*(x-0.5))/pi+14.5*(x-0.5)+0.5;
			//mu0[i]=-17*atan(4*pi*(x-0.5))/pi+16*(x-0.5)+0.5+(x-x*x)*param1*10;
			sum+=mu0[i];
			mu[i]=sum;
			mumax+=mu0[i]*(N-i);
		}
	}
	void initzero()
	{
		for(int s=0;s<N-1;++s)
		{
			multirangerelays[s]=0;
		}
	}
	void inithalf()
	{
		for(int s=0;s<N/2;++s)
		{
			multirangerelays[s]=2*(N/2-s)+1;
		}
		for(int s=N/2;s<N-1;++s)
		{
			multirangerelays[s]=0;
		}
	}
	float op(float x)
	{
		int i=x*N;
		if(i<0)i=0;
		if(i>=N)i=N-1;
		
		/*double sum=0;
		for(int s=0;s<i;++s)
			sum+=multirangerelays[s]=std::max<int>(multirangerelays[s],i-s);
		for(int s=i;s<N-1;++s)
			multirangerelays[s]=0;
		
		return sum/((double)N*(N-1))*2;*/
		double sum=0;
		for(int s=0;s<i;++s)
		{
			multirangerelays[s]=std::max<int>(multirangerelays[s],i-s);
			if(multirangerelays[s]>=0&&multirangerelays[s]<mu.size())
				sum+=mu[multirangerelays[s]];
		}
		for(int s=i;s<N-1;++s)
			multirangerelays[s]=0;
		
		return sum/mumax;
	}
};


//------------------------------------------------------------------------
class HysteresisProgram
{
friend class Hysteresis;
public:
	HysteresisProgram ();
	~HysteresisProgram () {}

private:
	Param param;
	char name[24];
};

typedef std::vector<float> floatv;

//------------------------------------------------------------------------
class Hysteresis : public AudioEffectX
{
public:
	Hysteresis (audioMasterCallback audioMaster);
	~Hysteresis ();

	//---from AudioEffect-----------------------
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

	virtual void setProgram (VstInt32 program);
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);
	
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual void resume ();

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion () { return 1000; }
	
	virtual VstPlugCategory getPlugCategory () { return kPlugCategEffect; }

protected:

	HysteresisProgram* programs;

	void setGain(float g);
	float getGain();
	void setNdiv(float n);
	float getNdiv();
	void setParam1(float p);
	float getParam1();

	void setfilter();

	void resethysteresis();

	hys internalhysteresis[2];

	Param param;

	Rydot::criticalsection cs;
};


