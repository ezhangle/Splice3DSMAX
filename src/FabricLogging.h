//////////////////////////////////////////////////////////////////////////
// This file is a catch-all for logging functionality.

#pragma once

// Struct will collect compiler results once created
struct GatherCompilerResults
{
private:
	std::string m_results;
public:
	GatherCompilerResults();
	~GatherCompilerResults();

	void LogSomething(const char* sSomething);
	const std::string& GetGatheredResults();
};

// Call this function to set the logging
// capacity of Fabric to the default
extern void InitLoggingTimer();

extern void myLogFunc(
	void *reportUserdata,
	FEC_ReportSource source,
	FEC_ReportLevel level,
	char const *lineCStr,
	uint32_t lineSize);

extern void SetGenericLoggerEnabled(bool enable);
extern void SetErrorLoggerEnabled(bool enable);
extern void SetCompilerLoggerEnabled(bool enable);
extern void SetKLReportLoggerEnabled(bool enable);
extern void SetKLStatusLoggerEnabled(bool enable);

// Log messages to the maxscript window
extern void logMessage(const CStr &cstr);

extern void postLogMessages();
