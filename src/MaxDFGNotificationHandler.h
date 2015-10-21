///////////////////////////////////////////////////////
//
// This class header has been almost completely copy-pasted
// DFGNotificationRouter.  We want to respond to notifications
// but there appears to be no built-in general-purpose class
// that does the job of converting text notify messages to
// compiler-friendly function calls etc.
//
#pragma once;

class SpliceTranslationFPInterface;

class MaxDFGNotificationHandler
{
public:

	MaxDFGNotificationHandler(SpliceTranslationFPInterface* pInterface);
	virtual ~MaxDFGNotificationHandler();

	// Must call when owning class changes/updates it's bindings
	void updateBinding(FabricCore::DFGBinding& binding);

	// DFG Change notifications
	void callback(FTL::CStrRef json);

#pragma region notification methods

	void onArgInserted(int index, FTL::CStrRef name, FTL::CStrRef type);

	void onArgTypeChanged(int index, FTL::CStrRef name, FTL::CStrRef newType);

	void onArgRenamed(FTL::CStrRef oldName, FTL::CStrRef newName);
	
	void onArgRemoved(FTL::CStrRef name);

	void onArgChanged(FTL::CStrRef name);

	void onDirty();

#pragma endregion

	// Members
	SpliceTranslationFPInterface* m_pInterface;

	// Our parent only returns us the Exec pointer in const form, but
	// for a lot of our functions we need to call functions that
	// have not yet been labelled as const.  Const-cast until FE cleans up their const-ness
	FabricCore::DFGExec GetExec();
	FabricCore::DFGBinding& GetBinding();
};