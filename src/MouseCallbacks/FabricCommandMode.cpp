#include "StdAfx.h"
#include "SpliceCommandMode.h"


SpliceCommandMode::SpliceCommandMode(void)
{
}


SpliceCommandMode::~SpliceCommandMode(void)
{
}

int SpliceCommandMode::Class()
{
	return MANIPULATE_COMMAND;
}

int SpliceCommandMode::ID()
{
	// Random number
	int id = 0x22062d9a;
	return id;
}

MouseCallBack * SpliceCommandMode::MouseProc( int *numPoints )
{
	if (numPoints != nullptr)
		*numPoints = INT_MAX;
	return &m_mouseCallback;
}

ChangeForegroundCallback * SpliceCommandMode::ChangeFGProc()
{
	return NULL;
}

BOOL SpliceCommandMode::ChangeFG( CommandMode* oldMode )
{
	return FALSE;
}

void SpliceCommandMode::EnterMode()
{
	m_mouseCallback.EnterMode();
}

void SpliceCommandMode::ExitMode()
{
	m_mouseCallback.ExitMode();
}
