//-------------------------------------------------------------------------------------------------------
// VST Plug-In
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST filter
// Filename     : hysteresismain.cpp
// Created by   : Ryogo Yoshimura
// Description  : Hysteresis filter
//
// (c) Ryogo Yoshimura, All Rights Reserved
// MIT License
//-------------------------------------------------------------------------------------------------------

#include "hysteresis.h"

#ifdef _DEBUG
#pragma comment(lib,"vstbased.lib")
#else
#pragma comment(lib,"vstbase.lib")
#endif

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new Hysteresis (audioMaster);
}

#if WIN32
#include <windows.h>
void* hInstance;

extern "C" {
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}
} // extern "C"
#endif
