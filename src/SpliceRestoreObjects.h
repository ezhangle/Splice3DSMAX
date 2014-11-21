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
	~HoldActions() { theHold.Accept(m_msg); }
};


// This CustomKLUndoRedoCommandObject allows us to merge Max's undo system
// and KL's.  It passes the Undo/Redo commands on to Fabric to to allow it
// to undo/redo it's actions
class CustomKLUndoRedoCommandObject : public RestoreObj
{
	FabricCore::RTVal m_rtval_commands;

public:
	CustomKLUndoRedoCommandObject(FabricCore::RTVal& commands);
	~CustomKLUndoRedoCommandObject();;

	virtual void Restore( int isUndo );
	virtual void Redo();
};

// The PortChange object saves the port structure of a graph
// and can save/restore it to a previous point
class SplicePortChangeObject : public RestoreObj
{
	SpliceTranslationFPInterface* m_maxOwner;

	FabricCore::Variant m_prePortLayout;
	FabricCore::Variant m_postPortLayout;

	// Cache the outport name so we can reconnect if necessary.
	std::string m_outPortName;

public:
	SplicePortChangeObject(SpliceTranslationFPInterface* maxOwner);
	~SplicePortChangeObject();;

	virtual void EndHold();
	virtual void Restore( int isUndo );
	virtual void Redo();
};