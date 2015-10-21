//////////////////////////////////////////////////////////////////////////
// SpliceRestoreObjects
//
// This file contains implementations of the various undo objects
// we can register with max dealing with KL actions
//
//////////////////////////////////////////////////////////////////////////

#pragma once

// This RAII scopes calls
class HoldActions {
	MSTR m_msg;

public:
	HoldActions(const MCHAR* msg) : m_msg(msg) { theHold.Begin(); }
	virtual ~HoldActions() { theHold.Accept(m_msg); }
};

//////////////////////////////////////////////////////////////////////////
// A specialization automatically adds a DFGCommandRestoreObj to the stack

class QHoldActions : public HoldActions {
public:
	QHoldActions(const MCHAR* msg);
};

//////////////////////////////////////////////////////////////////////////
// This CustomKLUndoRedoCommandObject allows us to merge Max's undo system
// and KL's.  It passes the Undo/Redo commands on to Fabric to to allow it
// to undo/redo it's actions
// All DFG Commands will add an undo object to maxes queue

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

// Allow CmdHandler to reference the shared undo stack.
extern QUndoStack* GetQtUndoStack();