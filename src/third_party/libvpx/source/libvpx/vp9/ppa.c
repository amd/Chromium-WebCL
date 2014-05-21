/*
 *	File			: ppa.c
 *					  ( C source file )
 *
 *	Description		: Parallel Path Analyzer
*					  ( This is a source of the PPA SDK. )
 *					  Add this file in your application.
 *
 *	Copyright		: MulticoreWare Inc.
 *					  ( http://www.multicorewareinc.com )
 *					  Copyright (c) 2013 MulticoreWare Inc. All rights reserved
 *
 *
 *	License			: PPA Software License v0.1
 *
 *
 *	This software is governed by MulticoreWare Inc. Permission to use, reproduce, 
 *	copy, modify, display, distribute, execute and transmit this software 
 *	for any purpose and any use of its accompanying documentation is hereby granted. 
 *	Provided that the above copyright notice appear in all copies, and that  
 *	this software is not used in advertising or publicity without specific, written
 *	prior permission by MulticoreWare Inc, and that the following disclaimer is 
 *	included.
 *	
 *
 *	THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 *	INCLUDING, WITHOUT LIMITATION, ANY WARRANTY OF DESIGN, MERCHANTABILITY 
 *	OR FITNESS FOR A PARTICULAR PURPOSE, AND WITHOUT WARRANTY AS TO NON-INFRINGEMENT 
 *	OR THE PERFORMANCE OR RESULTS YOU MAY OBTAIN BY USING THE SOFTWARE.
 */
#include "ppa.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#ifndef PPA_DISABLE
#ifdef PPA_REGISTER_CPU_EVENT
#undef PPA_REGISTER_CPU_EVENT
#endif

#ifdef PPA_REGISTER_CPU_EVENT2GROUP
#undef PPA_REGISTER_CPU_EVENT2GROUP
#endif

#define PPA_REGISTER_CPU_EVENT2GROUP(x,y) #x,#y,
#define PPA_REGISTER_CPU_EVENT(x) PPA_REGISTER_CPU_EVENT2GROUP(x,NoGroup)
char* PPACpuAndGroup[] = {
#include "ppaCPUEvents.h"
	""
};
#undef PPA_REGISTER_CPU_EVENT
#undef PPA_REGISTER_CPU_EVENT2GROUP


#ifdef __cplusplus
extern "C"{
#endif

static unsigned int init_count = 0;

#ifdef _WIN32
HMODULE ppaDllHandle = NULL;
#else
void * ppaSoHandle = NULL;
#endif

FUNC_PPAInit *PPAInitFunc;
FUNC_PPADel*PPADelFunc;
FUNC_PPAStartCpuEvent*PPAStartCpuEvent;
FUNC_PPAStopCpuEvent *PPAStopCpuEvent;
FUNC_PPAIsEventEnable *PPAIsEventEnable;
FUNC_PPARegisterCpuEvent *PPARegisterCpuEvent;
FUNC_PPARegisterGrpName *PPARegisterGrpName;
FUNC_PPATIDCpuEvent *PPATIDCpuEvent;
FUNC_PPADebugCpuEvent *PPADebugCpuEvent;
FUNC_PPARegisterCpuEventExGrpID *PPARegisterCpuEventExGrpID;
FUNC_PPASetGrpCpuEventEnDis *PPASetGrpCpuEventEnDis;
FUNC_PPASetSingleCpuEventEnDis *PPASetSingleCpuEventEnDis;

/************************************************************************/
/*                                                                      */
/************************************************************************/

void ErrorManage()
{

}

void initializePPA()
{
#ifdef _WIN32
	if(ppaDllHandle)
#else
	if(ppaSoHandle)
#endif
	{
		++init_count;
		return;
	}

#ifdef _WIN32
	ppaDllHandle = LoadLibrary(PPA_LIB_NAME);
	if(!ppaDllHandle)
#else
	ppaSoHandle = dlopen(PPA_LIB_NAME,RTLD_LAZY);
	if(!ppaSoHandle)
#endif
	{
		printf("Failed to load PPA library:%s!\n",PPA_LIB_NAME);
		return;
	}

/** get the function pointers to the event src api */
#ifdef _WIN32
	PPAInitFunc                   = (FUNC_PPAInit*)GetProcAddress(ppaDllHandle, "InitPpaUtil");
	PPADelFunc                    = (FUNC_PPADel*)GetProcAddress(ppaDllHandle,"DeletePpa");
	PPAStartCpuEvent              = (FUNC_PPAStartCpuEvent *)GetProcAddress(ppaDllHandle,"mcw_ppaStartCpuEvent");
	PPAStopCpuEvent               = (FUNC_PPAStopCpuEvent *)GetProcAddress(ppaDllHandle,"mcw_ppaStopCpuEvent");
	PPAIsEventEnable              = (FUNC_PPAIsEventEnable *)GetProcAddress(ppaDllHandle,"mcw_ppaIsEventEnable");
	PPARegisterCpuEvent           = (FUNC_PPARegisterCpuEvent *)GetProcAddress(ppaDllHandle,"mcw_ppaRegisterCpuEvent");
	PPARegisterGrpName            = (FUNC_PPARegisterGrpName *)GetProcAddress(ppaDllHandle,"mcw_ppaRegisterGrpName");
	PPATIDCpuEvent                = (FUNC_PPATIDCpuEvent *)GetProcAddress(ppaDllHandle,"mcw_ppaTIDCpuEvent");
	PPADebugCpuEvent              = (FUNC_PPADebugCpuEvent *)GetProcAddress(ppaDllHandle,"mcw_ppaDebugCpuEvent");
	PPARegisterCpuEventExGrpID    = (FUNC_PPARegisterCpuEventExGrpID *)GetProcAddress(ppaDllHandle,"mcw_ppaRegisterCpuEventExGrpID");
	PPASetGrpCpuEventEnDis        = (FUNC_PPASetGrpCpuEventEnDis *)GetProcAddress(ppaDllHandle,"mcw_ppaSetGrpCpuEventEnDis");
	PPASetSingleCpuEventEnDis     = (FUNC_PPASetSingleCpuEventEnDis *)GetProcAddress(ppaDllHandle,"mcw_ppaSetSingleCpuEventEnDis");
#else
	PPAInitFunc                   = (FUNC_PPAInit*)dlsym(ppaSoHandle, "InitPpaUtil");
	PPADelFunc                    = (FUNC_PPADel*)dlsym(ppaSoHandle,"DeletePpa");
	PPAStartCpuEvent              = (FUNC_PPAStartCpuEvent *)dlsym(ppaSoHandle,"mcw_ppaStartCpuEvent");
	PPAStopCpuEvent               = (FUNC_PPAStopCpuEvent *)dlsym(ppaSoHandle,"mcw_ppaStopCpuEvent");
	PPAIsEventEnable              = (FUNC_PPAIsEventEnable *)dlsym(ppaSoHandle,"mcw_ppaIsEventEnable");
	PPARegisterCpuEvent           = (FUNC_PPARegisterCpuEvent *)dlsym(ppaSoHandle,"mcw_ppaRegisterCpuEvent");
	PPARegisterGrpName            = (FUNC_PPARegisterGrpName *)dlsym(ppaSoHandle,"mcw_ppaRegisterGrpName");
	PPATIDCpuEvent                = (FUNC_PPATIDCpuEvent *)dlsym(ppaSoHandle,"mcw_ppaTIDCpuEvent");
	PPADebugCpuEvent              = (FUNC_PPADebugCpuEvent *)dlsym(ppaSoHandle,"mcw_ppaDebugCpuEvent");
	PPARegisterCpuEventExGrpID    = (FUNC_PPARegisterCpuEventExGrpID *)dlsym(ppaSoHandle,"mcw_ppaRegisterCpuEventExGrpID");
	PPASetGrpCpuEventEnDis        = (FUNC_PPASetGrpCpuEventEnDis *)dlsym(ppaSoHandle,"mcw_ppaSetGrpCpuEventEnDis");
	PPASetSingleCpuEventEnDis     = (FUNC_PPASetSingleCpuEventEnDis *)dlsym(ppaSoHandle,"mcw_ppaSetSingleCpuEventEnDis");
#endif
	if(!PPAInitFunc || !PPADelFunc || !PPAStartCpuEvent || !PPAStopCpuEvent
		 || !PPAIsEventEnable || !PPASetGrpCpuEventEnDis|| !PPARegisterCpuEvent 
		|| !PPARegisterGrpName || !PPATIDCpuEvent || !PPADebugCpuEvent
		|| !PPARegisterCpuEventExGrpID || !PPASetSingleCpuEventEnDis)
	{
#ifdef _WIN32
		FreeLibrary(ppaDllHandle);
		ppaDllHandle = NULL;
#else
		dlclose(ppaSoHandle);
		ppaSoHandle = NULL;
#endif
		printf("Load PPA function fails\n");
		return;
	}
	PPAInitFunc((char**)PPACpuAndGroup, PPACpuGroupNums);
	++init_count;
}

void releasePPA()
{
	if(--init_count == 0)
	{
		PPADelFunc();
		PPAInitFunc                = NULL;
		PPADelFunc                 = NULL;
		PPAStartCpuEvent           = NULL;
		PPAStopCpuEvent            = NULL;
		PPAIsEventEnable           = NULL;
		PPARegisterCpuEvent        = NULL;
		PPARegisterGrpName         = NULL;
		PPATIDCpuEvent             = NULL;
		PPADebugCpuEvent           = NULL;
		PPARegisterCpuEventExGrpID = NULL;
		PPASetGrpCpuEventEnDis     = NULL;
		PPASetSingleCpuEventEnDis  = NULL;
#ifdef _WIN32
		ppaDllHandle = NULL;
#else
		ppaSoHandle = NULL;
#endif
	}
}

#ifdef __cplusplus
}
#endif

#endif/* !defined(PPA_DISABLE) */
