#include "StdAfx.h"
#include "SpliceLogging.h"
#include <list>
#include <maxscript/maxscript.h>
#include <maxscript/util/listener.h>
#include "CriticalSection.h"
#include "SpliceStaticFPInterface.h"

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
#pragma region GatherCompilerResults

bool WasLogging()
{
	return (SpliceStaticFPInterface::GetInstance()->EnableLogging(0)&SpliceStaticFPInterface::LOG_COMPILER_ERROR) != 0;
}

std::list<GatherCompilerResults*> s_CompilerStack;
void gatherCompilerErrorFunc(
	unsigned int row, 
	unsigned int col, 
	const char * file,
	const char * level,
	const char * desc)
{
	CStr cstr;
	cstr.printf("[CompilerError] KL Error: %s, Line %d, Col %d: %s\n", file, (int)row, (int)col, desc);
	if (!s_CompilerStack.empty() && s_CompilerStack.back() != NULL)
	{
		s_CompilerStack.back()->LogSomething(cstr.data());
	}
	if (WasLogging())
		logMessage(cstr);
}

GatherCompilerResults::GatherCompilerResults()
{
	s_CompilerStack.push_back(this);
	FabricSplice::Logging::setCompilerErrorFunc(gatherCompilerErrorFunc);
}

GatherCompilerResults::~GatherCompilerResults()
{
	s_CompilerStack.pop_back();
	if (s_CompilerStack.empty())
	{
		// Reset our logging output to default channels.
		if (WasLogging())
			SpliceStaticFPInterface::GetInstance()->EnableLogging(SpliceStaticFPInterface::LOG_COMPILER_ERROR);
	}
}

void GatherCompilerResults::LogSomething(const char* sSomething)
{
	m_results = m_results + sSomething;
}

const std::string& GatherCompilerResults::GetGatheredResults()
{
	return m_results;
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
std::list<CStr> s_Messages;
CriticalSection s_LoggingLock;
DWORD s_MainTreadId;
#pragma region Splice Logging fns

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

void myLogFunc(const char * message, unsigned int length)
{
	CStr cstr;
	cstr.printf("[Splice] %s\n", message);
	logMessage(cstr);
}

void myLogErrorFunc(const char * message, unsigned int length)
{
	CStr cstr;
	cstr.printf("[Splice] Error: %s\n", message);
	logMessage(cstr);
}

void myCompilerErrorFunc(
	unsigned int row, 
	unsigned int col, 
	const char * file,
	const char * level,
	const char * desc
	) {
	CStr cstr;
	cstr.printf("[Splice] KL Error: %s, Line %d, Col %d: %s\n", file, (int)row, (int)col, desc);
	logMessage(cstr);
}

void myKLReportFunc(const char * message, unsigned int length)
{
	CStr cstr;
	cstr.printf("[Splice] KL Reports: %s\n", message);
	logMessage(cstr);
}

void myKLStatusFunc(const char * topic, unsigned int topicLength,  const char * message, unsigned int messageLength)
{
	CStr cstr;
	cstr.printf("[Splice] KL Status for '%s': %s\n", topic, message);
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

extern void SetGenericLoggerEnabled( bool enable )
{
	FabricSplice::Logging::setLogFunc((enable) ? myLogFunc : NULL);
}

extern void SetErrorLoggerEnabled( bool enable )
{
	FabricSplice::Logging::setLogErrorFunc((enable) ? myLogErrorFunc : NULL);
}

extern void SetCompilerLoggerEnabled( bool enable )
{
	FabricSplice::Logging::setCompilerErrorFunc((enable) ? myCompilerErrorFunc : NULL);
}

extern void SetKLReportLoggerEnabled( bool enable )
{
	FabricSplice::Logging::setKLReportFunc((enable) ? myKLReportFunc : NULL);
}

extern void SetKLStatusLoggerEnabled( bool enable )
{
	FabricSplice::Logging::setKLStatusFunc((enable) ? myKLStatusFunc : NULL);
}
