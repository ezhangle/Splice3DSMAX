//**************************************************************************/
// DESCRIPTION: Basic compiler setup, pre-compiled header to import most-used headers
// AUTHOR: Stephen Taylor
//***************************************************************************/

#pragma once

// useful for #pragma message
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
#pragma warning(disable:4481) // nonstandard extenion used: override specifier override

// Max includes
#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "macrorec.h"

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

#pragma warning(push, 0)
// Fabric includes
#include <FabricCore.h>

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>

#include <FTL/CStrRef.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONValue.h>

#include <qwidget.h>

#pragma warning(pop)

// Local includes
#include "DynamicParamBlocks\DynamicDialog\resource.h"
#include "FabricTranslationLayer.hpp"
#include "Fabric3dsmax.h"

extern const MSTR* EmptyStr();

extern MSTR ToMstr( const std::string& str );
extern MSTR ToMstr( const QString& qs );
extern MSTR ToMstr( const char* str );
extern CStr ToCStr( const MSTR& ms );
extern QString ToQStr( const MSTR& ms );


