/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#ifndef INTERFACE_H
#define INTERFACE_H

// This header defines the interface convention used in the valve engine.
// To make an interface and expose it:
//    1. Derive from IBaseInterface.
//    2. The interface must be ALL pure virtuals, and have no data members.
//    3. Define a name for it.
//    4. In its implementation file, use EXPOSE_INTERFACE or EXPOSE_SINGLE_INTERFACE.

// Versioning
// There are two versioning cases that are handled by this:
// 1. You add functions to the end of an interface, so it is binary compatible with the previous interface. In this case, 
//    you need two EXPOSE_INTERFACEs: one to expose your class as the old interface and one to expose it as the new interface.
// 2. You update an interface so it's not compatible anymore (but you still want to be able to expose the old interface 
//    for legacy code). In this case, you need to make a new version name for your new interface, and make a wrapper interface and 
//    expose it for the old interface.


// All interfaces derive from this.
class IBaseInterface
{
public:
	virtual ~IBaseInterface(void) {}
};

typedef IBaseInterface *(*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef IBaseInterface *(*InstantiateInterfaceFn)(void);

// Used internally to register classes.
class InterfaceReg
{
public:
	InterfaceReg(InstantiateInterfaceFn fn, const char *pName);

public:
	InstantiateInterfaceFn	m_CreateFn;
	const char *			m_pName;

	InterfaceReg *			m_pNext; // For the global list.
	static InterfaceReg *	s_pInterfaceRegs;
};

// Use this to expose an interface that can have multiple instances.
// e.g.:
// EXPOSE_INTERFACE( CInterfaceImp, IInterface, "MyInterface001" )
// This will expose a class called CInterfaceImp that implements IInterface (a pure class)
// clients can receive a pointer to this class by calling CreateInterface( "MyInterface001" )
//
// In practice, the shared header file defines the interface (IInterface) and version name ("MyInterface001")
// so that each component can use these names/vtables to communicate
//
// A single class can support multiple interfaces through multiple inheritance

#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
	static IBaseInterface *__Create##className##_interface(void) { return (interfaceName *)new className; } \
	static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName);

// Use this to expose a singleton interface with a global variable you've created.
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName) \
	static IBaseInterface *__Create##className##interfaceName##_interface(void) { return (interfaceName *)&globalVarName; } \
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
	static className __g_##className##_singleton; \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton);

#ifndef DLLExport
#define DLLExport __declspec(dllexport)
#endif

// This function is automatically exported and allows you to access any interfaces exposed with the above macros.
// if pReturnCode is set, it will return one of the following values
// extend this for other error conditions/code
enum
{
	IFACE_OK,
	IFACE_FAILED,
};

extern "C"
{
IBaseInterface DLLExport *CreateInterface(const char *pName, int *pReturnCode);
};

// Handle to an interface (HInterfaceModule_t* is just there for type safety).
typedef struct HInterfaceModule_t* HINTERFACEMODULE;

// Use these to load and unload a module.
extern HINTERFACEMODULE	Sys_LoadModule(const char *pModuleName);
extern void				Sys_FreeModule(HINTERFACEMODULE hModule);

// Use these to get the factory function from either a loaded module or the current module.
extern CreateInterfaceFn	Sys_GetFactory(HINTERFACEMODULE hModule);
extern CreateInterfaceFn	Sys_GetFactoryThis(void);

#endif