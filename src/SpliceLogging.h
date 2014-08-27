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
// capacity of Splice to the default
extern void SetDefaultLogging();

// Log messages to the maxscript window
extern void logMessage(const CStr &cstr);

extern void postLogMessages();