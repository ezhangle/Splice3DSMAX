#include "StdAfx.h"
#include "MaxCommandStack.h"
#include "SpliceRestoreObjects.h"

MaxCommandStack::MaxCommandStack()
{

}

MaxCommandStack::~MaxCommandStack()
{

}

bool MaxCommandStack::add(FabricServices::Commands::Command * command)
{
	MSTR actionDesc = MSTR::FromACP(command->getShortDesc());
	HoldActions hold(actionDesc);

	// For every command added to the command stack, add a matching
	// 3ds Max undo object.  The Max undo object will trigger the undo
	if (theHold.Holding())
		theHold.Put(new DFGCommandRestoreObj(command->getID()));

	return __super::add(command);
}

FabricServices::Commands::CommandStack* GetCommandStack()
{
	static MaxCommandStack stack;
	return &stack;
}
