#pragma once
#include "cmdmode.h"
#include "SpliceMouseCallback.h"

class SpliceCommandMode :
	public CommandMode
{
private:

	SpliceMouseCallback m_mouseCallback;

public:
	SpliceCommandMode(void);
	~SpliceCommandMode(void);

	virtual int Class();

	virtual int ID();

	virtual MouseCallBack * MouseProc( int *numPoints );

	virtual ChangeForegroundCallback * ChangeFGProc();

	virtual BOOL ChangeFG( CommandMode* oldMode );

	virtual void EnterMode();

	virtual void ExitMode();

};

