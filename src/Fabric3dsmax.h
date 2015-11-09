#pragma once

#ifdef DEBUG
#define DBGONLYMSG(text, caption) MaxMsgBox(NULL, text, caption, MB_OK)
#else
#define DBGONLYMSG(text, caption) 
#endif

#ifndef _DEBUG

#define MAXSPLICE_CATCH_BEGIN \
	try {

#define MAXSPLICE_CATCH_END \
	} \
	catch (FabricCore::Exception e) { \
		logMessage(e.getDesc_cstr()); \
		DBGONLYMSG(MSTR::FromACP(e.getDesc_cstr()), _M("FabricException")); \
	} 

#define MAXSPLICE_CATCH_RETURN(value) \
		} \
	catch (FabricCore::Exception e) { \
		logMessage(e.getDesc_cstr()); \
		return value; \
	} 

#else
// In debug mode, we do not want exceptions to be caught.  Ever...
#define MAXSPLICE_CATCH_BEGIN
#define MAXSPLICE_CATCH_END
#define MAXSPLICE_CATCH_RETURN(value)
#endif


