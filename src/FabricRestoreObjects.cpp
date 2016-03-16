#include "StdAfx.h"
#include "FabricRestoreObjects.h"


bool UndoOn()
{
	return theHold.Holding() && !theHold.IsSuspended();
}


//////////////////////////////////////////////////////////////////////////
DFGHoldActions::DFGHoldActions(const MCHAR* msg) 
	: HoldActions(msg)
{
	if (UndoOn())
	{
		int nextIndex = GetQtUndoStack()->count();
		theHold.Put(new DFGCommandRestoreObj(nextIndex));
	}
}

//////////////////////////////////////////////////////////////////////////
CoreHoldActions::CoreHoldActions(const MCHAR* msg) 
	: HoldActions(msg)
{
	if (UndoOn())
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
	didUndo; // No warn in release mode thanks
}

void FabricCoreRestoreObj::Redo()
{
	bool didRedo = GetHost().maybeRedo();
	DbgAssert(didRedo);
	didRedo; // No warn in release mode thanks
}

QUndoStack* GetQtUndoStack()
{
	static QUndoStack qUndoStack;
	return &qUndoStack;
}
