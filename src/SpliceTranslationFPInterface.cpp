#include "StdAfx.h"
#include "SpliceTranslationFPInterface.h"

#include <MaxScript/MaxScript.h>
#include <maxscript\foundation\functions.h>
#include <maxscript\maxwrapper\mxsobjects.h>


SpliceTranslationFPInterface::SpliceTranslationFPInterface()
	: m_klEditor(nullptr)
{

}

SpliceTranslationFPInterface::~SpliceTranslationFPInterface()
{
	CloseKLEditor();
}

BOOL SpliceTranslationFPInterface::SetSpliceGraph(ReferenceTarget* rtarg)
{
	SpliceTranslationFPInterface* pTargetInterface = static_cast<SpliceTranslationFPInterface*>(rtarg->GetInterface(ISPLICE__INTERFACE));
	if (pTargetInterface != nullptr)
	{
		// We are going to share the graph of the target interface
		SetSpliceGraph(pTargetInterface->GetSpliceGraph(), rtarg->GetParamBlock(0));
		return TRUE;
	}
	return FALSE;
}

bool SpliceTranslationFPInterface::RemovePortMSTR(const MSTR& name) 
{
	CStr cName = name.ToCStr();
	for (int i = 0; i < GetPortCount(); i++)
	{
		if (strcmp(GetPortName(i), cName.data()) == 0)
			return RemovePort(i);
	}
	return false;
}

bool SpliceTranslationFPInterface::ConnectPortMSTR( const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, int srcPortIndex )
{
	if (pSrcContainer == NULL)
		return false;

	CStr cMyPortName = myPortName.ToCStr();
	CStr cSrcPortName = srcPortName.ToCStr();
	return ConnectPort(cMyPortName, pSrcContainer, cSrcPortName, srcPortIndex);
}

void SpliceTranslationFPInterface::ShowKLEditor()
{
	CloseKLEditor();

	//Value* valueArg = MAXClass::make_wrapper_for(this);
	Value* valueArg = FPMixinInterfaceValue::intern(this);
	m_klEditor = CallMaxScriptFunction(_M("fn_From_Cpp_ShowKLEditor"), valueArg, true);
	// MxS will always return something, but its not always a value we can use
	if (m_klEditor == &undefined)
		m_klEditor = nullptr;
}

void SpliceTranslationFPInterface::CloseKLEditor()
{
	if (m_klEditor != nullptr) 
	{
		CallMaxScriptFunction(_M("fn_From_Cpp_CloseKLEditor_Dialog"), m_klEditor, false);
		m_klEditor->make_collectable();
		m_klEditor = nullptr;
	}
}

void SpliceTranslationFPInterface::UpdateKLEditor()
{
	if (m_klEditor != nullptr)
		CallMaxScriptFunction(_M("fn_From_Cpp_UpdateKLEditor"), m_klEditor, false);
}

Value* CallMaxScriptFunction(MCHAR* function, ReferenceTarget* fnArgument, bool returnResult) 
{
	Value* valueArg = MAXClass::make_wrapper_for(fnArgument);
	return CallMaxScriptFunction(function, valueArg, returnResult);
}

Value* CallMaxScriptFunction(MCHAR* function, Value* fnArgument, bool returnResult)
{
	if (fnArgument == nullptr)
		return nullptr;

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

		// On first run, evaluate the script that defines our function
		char* mxsEditor = nullptr;
		size_t buffSize = 0;
		if (_dupenv_s(&mxsEditor, &buffSize, "SCINTILLANETDIR") == 0) {
			MSTR mxsEditorPath = MSTR::FromACP(mxsEditor, buffSize);
			mxsEditorPath = mxsEditorPath + _T("SpliceScintillaEditor.ms");
			filein_script(mxsEditorPath.data());
			free(mxsEditor);
		}
	}
	init_thread_locals();
	push_alloc_frame();
	four_value_locals(name, fn, target, result);
	save_current_frames();
	set_error_trace_back_active(FALSE);

	try	{
		// Create the name of the maxscript function we want.
		// and look it up in the global names
		vl.name = Name::intern(function);
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
			// Call the function and save the result.
			vl.result = static_cast<Primitive*>(vl.fn)->apply(&fnArgument, 1);
		}
	} catch (...) {
		clear_error_source_data();
		restore_current_frames();
		MAXScript_signals = 0;
	}

	// If we are returning this value, we need to protect it from GC
	Value* res = nullptr;
	if (returnResult)
		res = vl.result ? vl.result->make_heap_permanent() : nullptr;

	// Magic Max Script stuff to clear the frame and locals.
	pop_value_locals();
	pop_alloc_frame();
	return res;
}

SpliceTranslationFPInterface* GetSpliceInterface( ReferenceTarget* pTarg )
{
	if (pTarg)
		return static_cast<SpliceTranslationFPInterface*>(pTarg->GetInterface(ISPLICE__INTERFACE));
	return nullptr;
}
