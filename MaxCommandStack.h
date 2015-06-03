#pragma once

#include <Commands\CommandStack.h>

class MaxCommandStack : public FabricServices::Commands::CommandStack
{
	MaxCommandStack();
	~MaxCommandStack();

	bool add(FabricServices::Commands::Command * command) override;
};