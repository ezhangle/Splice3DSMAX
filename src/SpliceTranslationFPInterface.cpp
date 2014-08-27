#include "StdAfx.h"
#include "SpliceTranslationFPInterface.h"

#include <MaxScript/MaxScript.h>
#include <maxscript\foundation\functions.h>
#include <maxscript\maxwrapper\mxsobjects.h>


BOOL SpliceTranslationFPInterface::SetSpliceGraph(ReferenceTarget* rtarg)
{
	SpliceTranslationFPInterface* pTargetInterface = static_cast<SpliceTranslationFPInterface*>(rtarg->GetInterface(ISPLICE__INTERFACE));
	if (pTargetInterface != nullptr)
	{
		// We are going to share the graph of the target interface
		SetSpliceGraph(pTargetInterface->GetSpliceGraph());
		return TRUE;
	}
	return FALSE;
}

void DoShowKLEditor(ReferenceTarget* pTarget)
{
	if (pTarget == NULL)
		return;

	// We are going to called a function defined entirely in MaxScript
	// to pop the editor dialog.  This MxS argument is required to take
	// a single argument called interface which is a pointer to the class
	// whose KL we are going to edit.  This genius section of code was
	// lifted from:
	// <http://sourceforge.net/p/niftools/max-nif-plugin/ci/master/tree/NifCommon/nimorph.cpp#l88>, 
	// pretty damn snazzy stuff

	// Magic initialization stuff for maxscript.
	static bool script_initialized = false;
	if (!script_initialized) {
		init_MAXScript();
		script_initialized = TRUE;
	}
	init_thread_locals();
	push_alloc_frame();
	six_value_locals(name, fn, mod, index, target, result);
	save_current_frames();
	set_error_trace_back_active(FALSE);

	try	{
		// Create the name of the maxscript function we want.
		// and look it up in the global names
		vl.name = Name::intern(_T("fn_From_Cpp_ShowKLEditor"));
		vl.fn = globals->get(vl.name);

		// For some reason we get a global thunk back, so lets
		// check the cell which should point to the function.
		// Just in case if it points to another global thunk
		// try it again.
		while (vl.fn != NULL && is_globalthunk(vl.fn))
			vl.fn = static_cast<GlobalThunk*>(vl.fn)->cell;
		while (vl.fn != NULL && is_constglobalthunk(vl.fn))
			vl.fn = static_cast<ConstGlobalThunk*>(vl.fn)->cell;

		// Now we should have a MAXScriptFunction, which we can
		// call to do the actual conversion. If we didn't
		// get a MAXScriptFunction, we can't convert.
		if (vl.fn != NULL) {

			// Ok. our KL editor takes takes 1 parameter
			Value* arg = MAXClass::make_wrapper_for(pTarget);
			if (arg != NULL)
			{
				// Call the function and save the result.
				vl.result = static_cast<Primitive*>(vl.fn)->apply(&arg, 1);
			}
		}
	} catch (...) {
		clear_error_source_data();
		restore_current_frames();
		MAXScript_signals = 0;
	}

	// Magic Max Script stuff to clear the frame and locals.
	pop_value_locals();
	pop_alloc_frame();
}