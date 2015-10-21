//////////////////////////////////////////////////////////////////////////
// SpliceRestoreObjects
//
// This file contains implementations of the various undo objects
// we can register with max dealing with KL actions
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// This RAII scopes calls to the undo system
class HoldActions {
	MSTR m_msg;

public:
	HoldActions(const MCHAR* msg) : m_msg(msg) { theHold.Begin(); }
	virtual ~HoldActions() { theHold.Accept(m_msg); }
};

//////////////////////////////////////////////////////////////////////////
// A specialization automatically adds a DFGCommandRestoreObj to the stack
class DFGHoldActions : public HoldActions {
public:
	DFGHoldActions(const MCHAR* msg);
};

//////////////////////////////////////////////////////////////////////////
// A specialization automatically adds a FabricCoreRestoreObj to the stack
class CoreHoldActions : public HoldActions {
public:
	CoreHoldActions(const MCHAR* msg);
};

//////////////////////////////////////////////////////////////////////////
// The DFG CommandRestoreObj synchronizes max undos with QUndoStack
// passed to the MaxDFGCmdHandler
class DFGCommandRestoreObj : public RestoreObj
{
	const int m_index;

public:
	DFGCommandRestoreObj(int id);
	~DFGCommandRestoreObj();

	virtual void Restore(int isUndo);
	virtual void Redo();
};

//////////////////////////////////////////////////////////////////////////
// The FabricCore restore obj synchronizes max undos with a single
// core operation.  These are used when we interact directly with
// the FabricCore (eg, setExecTitle, and directly setting port metadata)
class FabricCoreRestoreObj : public RestoreObj
{
public:
	FabricCoreRestoreObj();
	~FabricCoreRestoreObj();

	virtual void Restore(int isUndo);
	virtual void Redo();
};

//////////////////////////////////////////////////////////////////////////
// Allow CmdHandler to reference the shared undo stack.
extern QUndoStack* GetQtUndoStack();