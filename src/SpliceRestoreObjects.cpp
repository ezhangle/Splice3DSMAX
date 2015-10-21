#include "StdAfx.h"
#include "SpliceRestoreObjects.h"


//////////////////////////////////////////////////////////////////////////
QHoldActions::QHoldActions(const MCHAR* msg) : HoldActions(msg)
{
	if (theHold.Holding())
	{
		int nextIndex = GetQtUndoStack()->count();
		theHold.Put(new DFGCommandRestoreObj(nextIndex));
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

QUndoStack* GetQtUndoStack()
{
	static QUndoStack qUndoStack;
	return &qUndoStack;
}