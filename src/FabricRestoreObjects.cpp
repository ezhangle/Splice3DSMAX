#include "StdAfx.h"
#include "SpliceRestoreObjects.h"


//////////////////////////////////////////////////////////////////////////
DFGHoldActions::DFGHoldActions(const MCHAR* msg) 
	: HoldActions(msg)
{
	if (theHold.Holding())
	{
		int nextIndex = GetQtUndoStack()->count();
		theHold.Put(new DFGCommandRestoreObj(nextIndex));
	}
}

//////////////////////////////////////////////////////////////////////////
CoreHoldActions::CoreHoldActions(const MCHAR* msg) 
	: HoldActions(msg)
{
	if (theHold.Holding())
	{
		theHold.Put(new FabricCoreRestoreObj());
	}
}


//////////////////////////////////////////////////////////////////////////
DFGCommandRestoreObj::DFGCommandRestoreObj(int id)
	: m_index(id)
{
}

DFGCommandRestoreObj::~DFGCommandRestoreObj()
{
}

void DFGCommandRestoreObj::Restore(int isUndo)
{
	GetQtUndoStack()->undo();
}

void DFGCommandRestoreObj::Redo()
{
	GetQtUndoStack()->redo();
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FabricCoreRestoreObj::FabricCoreRestoreObj()
{
}

FabricCoreRestoreObj::~FabricCoreRestoreObj()
{
}

void FabricCoreRestoreObj::Restore(int isUndo)
{
	bool didUndo = GetHost().maybeUndo();
	DbgAssert(didUndo);
}

void FabricCoreRestoreObj::Redo()
{
	bool didRedo = GetHost().maybeRedo();
	DbgAssert(didRedo);
}

QUndoStack* GetQtUndoStack()
{
	static QUndoStack qUndoStack;
	return &qUndoStack;
}