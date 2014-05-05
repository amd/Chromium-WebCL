/*
 *	File			: ppa.h
 *					  ( C header file )
 *
 *	Description		: Parallel Path Analyzer
 *					  ( This is a header of the PPA SDK. )
 *					  Include this file in your application's source code,
 *					  and you can manually use the PPA interfaces in your own program.
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
#ifndef _PPA_H_
#define _PPA_H_

#ifdef PPA_REGISTER_CPU_EVENT
#undef PPA_REGISTER_CPU_EVENT
#endif

#include <stdio.h>
#include <stdlib.h>

#ifndef __cplusplus
typedef enum {false = 0,true = 1} bool;
#endif

//#define PPA_CODELINK
#undef PPA_CLEANUP

#ifdef PPA_DISABLE
	#define UNDEF_PARAM(x) 
	#define PPA_CLEANUP(x) UNDEF_PARAM(#x)
inline bool dumbrt() {return 0;}
#else
	#ifdef PPA_CODELINK
		#define P2STRING(x) #x
		#define PSTRING(x) P2STRING(x)
		#define PRAGMA_MES __pragma("message(__FILE__)")
	#if defined(WINDOWS)||defined(_WIN32)
		#define PPA_APICLEANUP(x) PRAGMA_MES x
		#define PPA_CLEANUP(x) x
	#endif /* _WIN32 */

	#else
		#define	PPA_CLEANUP(x) x
		#define	PPA_APICLEANUP(x) x
	#endif /* PPA_CODLELINK */

#endif /* !defined(PPA_DISABLE) */


#ifdef _WIN32
	#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
	#ifdef UNICODE
		#define PPA_LIB_NAME L"ppa64.dll"
	#else
		#define PPA_LIB_NAME "ppa64.dll"
	#endif
	#else 
	#ifdef UNICODE
		#define PPA_LIB_NAME L"ppa.dll"
	#else
		#define PPA_LIB_NAME "ppa.dll"
	#endif
	#endif /* 32bit */

#elif defined(__linux__)
	#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
		#define PPA_LIB_NAME "libppa64.so"
	#else
		#define PPA_LIB_NAME "libppa.so"
	#endif

#elif defined(__APPLE__) || defined(__MACH__)
	#ifdef __LP64__
		#define PPA_LIB_NAME "libppa64.dylib"
	#else
		#define PPA_LIB_NAME "libppa.dylib"
	#endif

#endif /* !WINDOWS */

/**
 * Register PPA event in a group
 * @param x event name
 * @param y group name
 */
#if !defined(PPA_DISABLE)
#define PPA_REGISTER_CPU_EVENT2GROUP(x,y) x,
#define PPA_REGISTER_CPU_EVENT(x) PPA_REGISTER_CPU_EVENT2GROUP(x,NoGroup)
enum PPACpuEventEnum {
	#include "ppaCPUEvents.h"
	PPACpuGroupNums
};

#undef PPA_REGISTER_CPU_EVENT
#undef PPA_REGISTER_CPU_EVENT2GROUP
#endif

#ifdef __cplusplus
extern "C"{
#endif

typedef unsigned short  EventID;
typedef unsigned char GrpID;

typedef void (FUNC_PPAInit)(char**, int);
typedef void (FUNC_PPADel)();
typedef void (FUNC_PPAStartCpuEvent)(EventID);
typedef void (FUNC_PPAStopCpuEvent)(EventID );
typedef bool (FUNC_PPAIsEventEnable)(EventID );
typedef EventID (FUNC_PPARegisterCpuEvent)(const char* );
typedef GrpID (FUNC_PPARegisterGrpName)(const char* );
typedef void (FUNC_PPATIDCpuEvent)(EventID ,unsigned int );
typedef void (FUNC_PPADebugCpuEvent)(EventID ,unsigned int ,unsigned int );
typedef EventID (FUNC_PPARegisterCpuEventExGrpID)(const char* ,GrpID );
typedef int (FUNC_PPASetGrpCpuEventEnDis)(bool ,GrpID );
typedef bool (FUNC_PPASetSingleCpuEventEnDis)(bool ,EventID );

#ifndef PPA_DISABLE
extern FUNC_PPAInit* PPAInitFunc;
extern FUNC_PPADel* PPADelFunc;
extern FUNC_PPAStartCpuEvent* PPAStartCpuEvent;
extern FUNC_PPAStopCpuEvent* PPAStopCpuEvent;
extern FUNC_PPAIsEventEnable* PPAIsEventEnable;
extern FUNC_PPARegisterCpuEvent* PPARegisterCpuEvent;
extern FUNC_PPARegisterGrpName* PPARegisterGrpName;
extern FUNC_PPATIDCpuEvent* PPATIDCpuEvent;
extern FUNC_PPADebugCpuEvent* PPADebugCpuEvent;
extern FUNC_PPARegisterCpuEventExGrpID* PPARegisterCpuEventExGrpID;
extern FUNC_PPASetGrpCpuEventEnDis* PPASetGrpCpuEventEnDis;
extern FUNC_PPASetSingleCpuEventEnDis* PPASetSingleCpuEventEnDis;
#endif

/**
 * Macro for ppaStartCpuEvent
 * @param e CPU event id
 * @see ppaStartCpuEvent
 */
#define PPAStartCpuEventFunc(e)	     \
	if(PPAStartCpuEvent)                 \
		PPA_CLEANUP((PPAStartCpuEvent(e)))

/**
 * Macro for ppaStopCpuEvent
 * @param e CPU event id
 * @see ppaStopCpuEvent
 */
#define PPAStopCpuEventFunc(e)	           \
	if(PPAStopCpuEvent)					   \
		PPA_CLEANUP((PPAStopCpuEvent( (e) )))

/**
 * Macro for ppaIsEventEnabled
 * @param e CPU event id
 * @see ppaIsEventEnabled
 */
#ifdef PPA_DISABLE
#define PPAIsEventEnabledFunc(e) dumbrt()
#else
#define PPAIsEventEnabledFunc(e)           \
	(PPAIsEventEnable ? PPAIsEventEnable( (e) ) : false)
#endif

/**
* Macro for ppaRegisterCpuEvent
* @param s CPU event name
* @see ppaRegisterCpuEvent
*/
#ifdef PPA_DISABLE
#define PPARegisterCpuEventFunc(s) dumbrt()
#else
#define PPARegisterCpuEventFunc(s)           \
	(PPARegisterCpuEvent ? PPARegisterCpuEvent( (s) ) : 0xFFFF)
#endif

/**
* Macro for ppaRegisterGrpName
* @param s group name
* @see ppaRegisterGrpName
*/
#ifdef PPA_DISABLE
#define PPARegisterGrpNameFunc(s) dumbrt()
#else
#define PPARegisterGrpNameFunc(s)             \
	(PPARegisterGrpName ? PPARegisterGrpName( (s) ) : 0xFF) 
#endif

/**
 * Macro for ppaTIDCpuEvent
 * @param e CPU event id
 * @param data additional data
 * @see ppaTIDCpuEvent
 */
#define PPATIDCpuEventFunc(e,data)             \
	if(PPATIDCpuEvent)						   \
		PPA_CLEANUP((PPATIDCpuEvent( (e) ,(data) )))

/**
 * Macro for ppaStartCpuEvent
 * @param e CPU event id
 * @param data0 additional data
 * @param data1 additional data
 * @see ppaDebugCpuEvent
 */
#define PPADebugCpuEventFunc(e,data0,data1)     \
	if(PPADebugCpuEvent)						 \
	PPA_CLEANUP((PPADebugCpuEvent((e),(data0),(data1))))

/**
 * Macro for ppaRegisterCpuEventExGrpID
 * @param s CPU event name
 * @param e group id
 * @see ppaRegisterCpuEventExGrpID
 */
#ifdef PPA_DISABLE
#define PPARegisterCpuEventExGrpIDFunc(s,e) dumbrt()
#else
#define PPARegisterCpuEventExGrpIDFunc(s,e)     \
	(PPARegisterCpuEventExGrpID ? PPARegisterCpuEventExGrpID((s),(e)) : 0xFFFF)
#endif

/**
 * Macro for ppaSetGrpCpuEventsEnDis
 * @param en_dis enable or disable, TRUE or 1 is enable, FALSE or 0 is disable
 * @param e group id
 * @see ppaSetGrpCpuEventsEnDis
 */
#ifdef PPA_DISABLE
#define PPASetGrpCpuEventsEnDisFunc(en_dis,e) dumbrt()
#else
#define PPASetGrpCpuEventsEnDisFunc(en_dis,e)   \
	(PPASetGrpCpuEventEnDis ? PPASetGrpCpuEventEnDis((en_dis),(e)) : 0)
#endif

/**
 * Macro for ppaSetSingleCpuEventEnDis
 * @param en_dis enable or disable, TRUE or 1 is enable, FALSE or 0 is disable
 * @param e CPU event id
 * @see ppaSetSingleCpuEventEnDis
 */
#ifdef PPA_DISABLE
 #define PPASetSingleCpuEventEnDisFunc(en_dis,e) dumbrt()
#else
 #define PPASetSingleCpuEventEnDisFunc(en_dis,e) \
 (PPASetSingleCpuEventEnDis ? PPASetSingleCpuEventEnDis((en_dis),(e)) : false)
#endif

/**
 * Initialize PPA
 * @return a pointer to the PpaBase class
 */
void initializePPA();

/**
 *	Release PPA
 *  @return void 
 */
void releasePPA();

/**
 * PPA_INIT  macro to initialize PPA
 */
#define PPA_INIT()   PPA_CLEANUP(initializePPA();)

/**
 * PPA_END macro to shut down PPA
 */
#define PPA_END()  PPA_CLEANUP(releasePPA();) 

// for compatibility to early version
#define PPAEnableGroup                   PPASetGrpCpuEventsEnDisFunc
#define PPAEnableEvent                   PPASetSingleCpuEventEnDisFunc
#define PPARegisterEventInGroup          PPARegisterCpuEventExGrpIDFunc
#define PPADispatchStartEvent            PPAStartCpuEventFunc
#define PPADispatchEndEvent              PPAStopCpuEventFunc
#define PPADispatchTidEvent              PPATIDCpuEventFunc
#define PPADispatchDebugEvent            PPADebugCpuEventFunc
#define PPARegisterGroup                 PPARegisterGrpNameFunc
#define PPARegisterEvent                 PPARegisterCpuEventFunc
#define PPAIsEventFiltered               PPAIsEventFilteredFunc
#define PPAIsEventFilteredFunc           PPAIsEventEnabledFunc

#ifdef __cplusplus
}
#endif

#endif /* _PPA_H_ */
