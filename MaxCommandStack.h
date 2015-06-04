#pragma once

#include <Commands\CommandStack.h>

class MaxCommandStack : public FabricServices::Commands::CommandStack
{
public:

	MaxCommandStack();
	~MaxCommandStack();

	bool add(FabricServices::Commands::Command * command) override;
};

FabricServices::Commands::CommandStack* GetCommandStack();