#include "StdAfx.h"
#include "FabricCommandMode.h"


FabricCommandMode::FabricCommandMode(void)
{
}


FabricCommandMode::~FabricCommandMode(void)
{
}

int FabricCommandMode::Class()
{
	return MANIPULATE_COMMAND;
}

int FabricCommandMode::ID()
{
	// Random number
	int id = 0x22062d9a;
	return id;
}

MouseCallBack * FabricCommandMode::MouseProc( int *numPoints )
{
	if (numPoints != nullptr)
		*numPoints = INT_MAX;
	return &m_mouseCallback;
}

ChangeForegroundCallback * FabricCommandMode::ChangeFGProc()
{
	return NULL;
}

BOOL FabricCommandMode::ChangeFG( CommandMode* oldMode )
{
	return FALSE;
}

void FabricCommandMode::EnterMode()
{
	m_mouseCallback.EnterMode();
}

void FabricCommandMode::ExitMode()
{
	m_mouseCallback.ExitMode();
}
