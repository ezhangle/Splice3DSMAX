#include "StdAfx.h"
#include "SpliceRestoreObjects.h"

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
	//DFGWrapper::Binding graph = maxOwner->GetSpliceGraph();
	//m_prePortLayout = graph.getPersistenceDataDict();
	//m_outPortName = maxOwner->GetOutPortName();
}

SplicePortChangeObject::~SplicePortChangeObject()
{

}

void SplicePortChangeObject::EndHold()
{
	//DFGWrapper::Binding graph = m_maxOwner->GetSpliceGraph();
	//m_postPortLayout = graph.getPersistenceDataDict();
}

void SplicePortChangeObject::Restore( int isUndo )
{
	//DFGWrapper::Binding graph = m_maxOwner->GetSpliceGraph();
	//graph.setFromPersistenceDataDict(m_prePortLayout);
	//m_maxOwner->UpdateKLEditor();
	//// Re-connect the outport (TODO: Parent ports?)
	//m_maxOwner->ResetPorts();
	//m_maxOwner->SetOutPortName(m_outPortName.data());
}

void SplicePortChangeObject::Redo()
{
	//DFGWrapper::Binding graph = m_maxOwner->GetSpliceGraph();
	//graph.setFromPersistenceDataDict(m_postPortLayout);
	//m_maxOwner->UpdateKLEditor();
	//// Re-connect the outport (TODO: Parent ports?)
	//m_maxOwner->ResetPorts();
	//m_maxOwner->SetOutPortName(m_outPortName.data());
}

#pragma endregion