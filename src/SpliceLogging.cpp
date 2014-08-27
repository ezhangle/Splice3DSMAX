#include "StdAfx.h"
#include "SpliceLogging.h"
#include <list>
#include <maxscript/maxscript.h>
#include <maxscript/util/listener.h>
#include "CriticalSection.h"

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

std::list<GatherCompilerResults*> s_CompilerStack;
void gatherCompilerErrorFunc(
	unsigned int row, 
	unsigned int col, 
	const char * file,
	const char * level,
	const char * desc)
{
	CStr cstr;
	cstr.printf("[MyCallback] KL Error: %s, Line %d, Col %d: %s\n", file, (int)row, (int)col, desc);
	if (!s_CompilerStack.empty() && s_CompilerStack.back() != NULL)
	{
		s_CompilerStack.back()->LogSomething(cstr.data());
	}
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
		SetDefaultLogging();
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

void SetDefaultLogging()
{
	FabricSplice::Logging::setLogFunc(myLogFunc);
	FabricSplice::Logging::setLogErrorFunc(myLogErrorFunc);
	FabricSplice::Logging::setCompilerErrorFunc(myCompilerErrorFunc);
	FabricSplice::Logging::setKLReportFunc(myKLReportFunc);
	FabricSplice::Logging::setKLStatusFunc(myKLStatusFunc);

	// cache the main thread ID (this thread)
	s_MainTreadId = GetCurrentThreadId();
#ifdef _DEBUG
	// Faster message posting on debug for a clearer pic of whats happening
	const int timeout = 100; 
#else
	// slower posting on release for better perf
	const int timeout = 1000;
#endif
	SetTimer(NULL, NULL, timeout, (TIMERPROC)&printMessages);
}