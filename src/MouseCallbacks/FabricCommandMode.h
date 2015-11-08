#pragma once
#include "cmdmode.h"
#include "FabricMouseCallback.h"

class FabricCommandMode :
	public CommandMode
{
private:

	FabricMouseCallback m_mouseCallback;

public:
	FabricCommandMode(void);
	~FabricCommandMode(void);

	virtual int Class();

	virtual int ID();

	virtual MouseCallBack * MouseProc( int *numPoints );

	virtual ChangeForegroundCallback * ChangeFGProc();

	virtual BOOL ChangeFG( CommandMode* oldMode );

	virtual void EnterMode();

	virtual void ExitMode();

};

