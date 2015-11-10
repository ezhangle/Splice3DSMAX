#include "StdAfx.h"
#include "FabricLogging.h"
#include <list>
#include <maxscript/maxscript.h>
#include <maxscript/util/listener.h>
#include "CriticalSection.h"
#include "FabricStaticFPInterface.h"

//////////////////////////////////////////////////////////////////////////
#pragma region Printing to the Listener

void ScriptPrint(const MCHAR* msg)
{
	the_listener->edit_stream->printf(msg);
}

std::wstring s2ws(const std::string& s)
{
	std::wstring r(s.begin(), s.end());
	return r;
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
std::list<CStr> s_Messages;
CriticalSection s_LoggingLock;
DWORD s_MainTreadId;
#pragma region Fabric Logging fns

void postMessage(const CStr& cstr)
{
#ifdef _UNICODE
	ScriptPrint(s2ws(std::string(cstr)).data());
#else
	ScriptPrint((cstr + std::string("\n")).data());
#endif
}
void logMessage(const CStr &cstr)
{
	// If posted from the main thread, post immediately
	
	if (GetCurrentThreadId() == s_MainTreadId)
		postMessage(cstr);
	else
	{
		CSLock lock(s_LoggingLock);
		if (s_Messages.size() < 100)
			s_Messages.push_back(cstr);
	}
}

void postLogMessages()
{
	CSLock lock(s_LoggingLock);
	for (auto itr = s_Messages.begin(); itr != s_Messages.end(); itr++)
	{
		postMessage(*itr);
	}
	s_Messages.clear();
}

void myLogFunc(
	void *reportUserdata,
	FEC_ReportSource source,
	FEC_ReportLevel level,
	char const *lineCStr,
	uint32_t lineSize)
{
	CStr cstr;
	cstr.printf("[Fabric] %s\n", lineCStr);
	logMessage(cstr);
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region Initialization etc

void CALLBACK printMessages(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime)
{
	postLogMessages();
}

void InitLoggingTimer()
{
	// cache the main thread ID (this thread)
	s_MainTreadId = GetCurrentThreadId();
#ifdef _DEBUG
	// Faster message posting on debug for a clearer pic of whats happening
	const int timeout = 100; 
#else
	// slower posting on release for better performance
	const int timeout = 1000;
#endif
	SetTimer(NULL, NULL, timeout, (TIMERPROC)&printMessages);
}
