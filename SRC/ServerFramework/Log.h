#pragma once

class CLog {
	CLog();
	virtual ~CLog();

public:
	enum LogLevel {
		LogDebug,
		LogInfo,
		LogWarning,
		LogError,
		LogNone
	};
 
	static CLog* GetInstancePtr();

	BOOL StartLog(CHAR* szTitle, CHAR* szLogDir);

	BOOL CloseLog();

	VOID SetTitle(CHAR* szTitle);

	VOID LogOutput(UINT32 uLogLevel, CHAR* szFormat, ...);

	BOOL SetLogLevel(UINT32 uLevel);

private:
	std::mutex	m_mutex;

	CHAR		m_szTitle[512];

	UINT32		m_uLogLevel;

	UINT32		m_uLogCount;

	FILE*		m_pLogFile;
};

#define LOG_MAX_LENGTH 512

#define LOG_DEBUG_OUTPUT(Format, ...)\
CLog::GetInstancePtr()->LogOutput(CLog::LogLevel::LogDebug, Format, ##__VA_ARGS__);

#define LOG_INFO_OUTPUT(Format, ...)\
CLog::GetInstancePtr()->LogOutput(CLog::LogLevel::LogInfo, Format, ##__VA_ARGS__);

#define LOG_WARRNING_OUTPUT(Format, ...)\
CLog::GetInstancePtr()->LogOutput(CLog::LogLevel::LogInfo, Format, ##__VA_ARGS__);

#define LOG_ERROR_OUTPUT(Format, ...)\
CLog::GetInstancePtr()->LogOutput(CLog::LogLevel::LogError, Format, ##__VA_ARGS__);

#define LOG_NONE_OUTPUT(Format, ...)\
CLog::GetInstancePtr()->LogOutput(CLog::LogLevel::LogNone, Format, ##__VA_ARGS__);