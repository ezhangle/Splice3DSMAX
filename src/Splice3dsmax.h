#pragma once


#define MAXSPLICE_CATCH_BEGIN() \
	try {

#define MAXSPLICE_CATCH_END() \
    } \
	catch (FabricSplice::Exception e) { \
		logMessage(e.what()); \
    } \
	catch (FabricCore::Exception e) { \
		logMessage(e.getDesc_cstr()); \
    } \
	catch (...) \
	{ \
		logMessage("ERROR: Unhandled exception."); \
	}

#define MAXSPLICE_CATCH_RETURN(value) \
	    } \
	catch (FabricSplice::Exception e) { \
		logMessage(e.what()); \
		return value; \
	} \
	catch (FabricCore::Exception e) { \
		logMessage(e.getDesc_cstr()); \
		return value; \
	} \
	catch (...) \
	{ \
		logMessage("ERROR: Unhandled exception."); \
		return value; \
	}