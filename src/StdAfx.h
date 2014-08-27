//**************************************************************************/
// Copyright (c) 1998-2005 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Include this file before including any 3ds Max SDK files. It 
//              define the macros required to add linkable todo compile-time 
//              messages and lower the warning level to 3.
// AUTHOR: Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#pragma once

// useful for #pragma message
#define STRING2(x) #x
#define STRING(x) STRING2(x)
#define TODO(x) __FILE__ "(" STRING(__LINE__) "): TODO: "x
#define SAFE_DELETE(x) { if (x) { delete (x); (x) = NULL; } }
#define SAFE_FREE(x) { if (x) { free (x); (x) = NULL; } }
#define UNUSED(x)

#define FEC_STATIC 1
#define FECS_STATIC 1

#pragma warning(disable:4100) // unreferenced formal parameter
#pragma warning(disable:4800) //  forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable:4267) //  conversion from 'size_t' to 'uint32_t', possible loss of data
#pragma warning(disable:4245) //  signed/unsigned mismatch
#pragma warning(disable:4238) //  class rvalue used as lvalue

#pragma warning(push, 3)	// set to compiler warning level 3 for Max includes

// Max includes
#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#pragma warning(pop)		// revert to current build warning level

// Max 2010 changes a lot of SDK functions to be const CHAR* instead of CHAR* 
#if MAX_VERSION_MAJOR > 12
#define CONST_2010 const
#endif

#if MAX_VERSION_MAJOR < 15
#define p_end end
#endif

// Get strings from the Win32 resource table
extern HINSTANCE hInstance;
extern TCHAR *GetString(int id);

// Fabric includes
#include <FabricSplice.h>

// Local includes
#include "DynamicParamBlocks\DynamicDialog\resource.h"
#include "SpliceTranslationLayer.hpp"


