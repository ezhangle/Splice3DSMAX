#include "StdAfx.h"
#include "SpliceRestoreObjects.h"
#include "Commands\CommandStack.h"
#include "MaxCommandStack.h"

#pragma region CustomKLUndoRedoCommandObject

CustomKLUndoRedoCommandObject::CustomKLUndoRedoCommandObject( FabricCore::RTVal& commands )
{
	m_rtval_commands = commands;
}

CustomKLUndoRedoCommandObject::~CustomKLUndoRedoCommandObject()
{
	m_rtval_commands.invalidate();
}

void CustomKLUndoRedoCommandObject::Restore( int isUndo )
{
	if(m_rtval_commands.isValid())
	{
		for(size_t i=0; i< m_rtval_commands.getArraySize(); i++)
		{
			m_rtval_commands.getArrayElement(i).callMethod("", "undoAction", 0, 0);
		}
	}
}

void CustomKLUndoRedoCommandObject::Redo()
{
	if(m_rtval_commands.isValid())
	{
		for(size_t i = 0; i < m_rtval_commands.getArraySize(); i++)
		{
			m_rtval_commands.getArrayElement(i).callMethod("", "doAction", 0, 0);
		}
	}
}

#pragma endregion

#pragma region SplicePortChangeObject

SplicePortChangeObject::SplicePortChangeObject(SpliceTranslationFPInterface* maxOwner)
	: m_maxOwner(maxOwner)
{
	m_outPrePortName = maxOwner->GetOutPortName();
}

SplicePortChangeObject::~SplicePortChangeObject()
{

}

void SplicePortChangeObject::EndHold()
{
	m_outPostPortName = m_maxOwner->GetOutPortName();
}

void SplicePortChangeObject::Restore( int isUndo )
{
	GetCommandStack()->undo();
	//FabricCore::DFGBinding graph = m_maxOwner->GetSpliceGraph();
	//graph.setFromPersistenceDataDict(m_prePortLayout);
	//m_maxOwner->UpdateKLEditor();
	//// Re-connect the outport (TODO: Parent ports?)
	//m_maxOwner->ResetPorts();
	m_maxOwner->SetOutPort(m_outPrePortName.data());
}

void SplicePortChangeObject::Redo()
{
	GetCommandStack()->redo();
	//FabricCore::DFGBinding graph = m_maxOwner->GetSpliceGraph();
	//graph.setFromPersistenceDataDict(m_postPortLayout);
	//m_maxOwner->UpdateKLEditor();
	//// Re-connect the outport (TODO: Parent ports?)
	//m_maxOwner->ResetPorts();
	; m_maxOwner->SetOutPort(m_outPostPortName.data());
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
DFGCommandRestoreObj::DFGCommandRestoreObj(int id)
	: m_commandId(id)
{
}

DFGCommandRestoreObj::~DFGCommandRestoreObj()
{
}

void DFGCommandRestoreObj::Restore(int isUndo)
{
	GetCommandStack()->undo(m_commandId);
}

void DFGCommandRestoreObj::Redo()
{
	GetCommandStack()->redo(m_commandId);
}

