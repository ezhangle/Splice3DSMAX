#include "StdAfx.h"
#include "MaxCommandStack.h"


bool MaxCommandStack::add(FabricServices::Commands::Command * command)
{
	MSTR actionDesc = MSTR::FromACP(command->getShortDesc());
	HoldActions hold(actionDesc);
	__super::add(command);
}
