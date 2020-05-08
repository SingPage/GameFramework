#include"stdafx.h"
#include"Log.h"

CLog::CLog() {

	m_szTitle[0] = '\0';

	m_uLogLevel = LogNone;

	m_uLogCount = 0;

	m_pLogFile = NULL;
}

CLog::~CLog() {

	CloseLog();
}

CLog* CLog::GetInstancePtr(){

	static CLog pLog;

	return &pLog;
}

BOOL CLog::StartLog(CHAR* szTitle, CHAR* szLogDir)
{
	INT32 nRet = 0;
	tm CurrentTm = FunctionLib::GetCurrentTm();

	m_mutex.lock();
	if (NULL == m_pLogFile) {

		nRet = mkdir(szLogDir, S_IRWXU);

		if (0 != nRet && EEXIST != EEXIST) {
			return FALSE;
		}

		CHAR szFileName[LOG_MAX_LENGTH];
		snprintf(szFileName, LOG_MAX_LENGTH, "%s/%s_%04d-%02d-%02dT-%02d-%02d-%02d.log",
			szLogDir, szTitle, CurrentTm.tm_year + 1990,CurrentTm.tm_mon + 1, CurrentTm.tm_mday,
			CurrentTm.tm_hour, CurrentTm.tm_min, CurrentTm.tm_sec);

		strncpy(m_szTitle, szTitle, 512);

		m_pLogFile = fopen(szFileName, "a+");
		if (NULL == m_pLogFile) {
			return FALSE;
		}

		m_mutex.unlock();
		return TRUE;
	}
	
	m_mutex.unlock();
	return FALSE;
}

BOOL CLog::CloseLog(){

	m_mutex.lock();
	if (NULL != m_pLogFile) {

		fflush(m_pLogFile);

		fclose(m_pLogFile);

		m_pLogFile = NULL;

		m_mutex.unlock();

		return TRUE;
	}

	m_mutex.unlock();
	return FALSE;
}

VOID CLog::SetTitle(CHAR* szTitle){

	m_mutex.lock();
	if (NULL != szTitle) {
		strncpy(m_szTitle, szTitle, 512);
	}

	m_mutex.unlock();
	return;
}

VOID CLog::LogOutput(UINT32 uLogLevel, CHAR* szFormat, ...){

	UINT32 uLogSize = 0;
	va_list argList;
	
	if (uLogLevel <= m_uLogLevel && NULL != m_pLogFile) {

		tm CurrentTm = FunctionLib::GetCurrentTm();
		CHAR szLog[LOG_MAX_LENGTH];

		switch (uLogLevel)
		{
		case LogLevel::LogDebug:
			snprintf(szLog, LOG_MAX_LENGTH, "[%04d-%02d-%02d %02d:%02d:%02d][Thread %04x] Debug:",
				CurrentTm.tm_year + 1990, CurrentTm.tm_mon + 1, CurrentTm.tm_mday, CurrentTm.tm_hour,
				CurrentTm.tm_min, CurrentTm.tm_sec, FunctionLib::GetCurThreadId());
			break;

		case LogLevel::LogInfo:
			snprintf(szLog, LOG_MAX_LENGTH, "[%04d-%02d-%02d %02d:%02d:%02d][Thread %04x] Info:",
				CurrentTm.tm_year + 1990, CurrentTm.tm_mon + 1, CurrentTm.tm_mday, CurrentTm.tm_hour,
				CurrentTm.tm_min, CurrentTm.tm_sec, FunctionLib::GetCurThreadId());
			break;

		case LogLevel::LogWarning:
			snprintf(szLog, LOG_MAX_LENGTH, "[%04d-%02d-%02d %02d:%02d:%02d][Thread %04x] Warning:",
				CurrentTm.tm_year + 1990, CurrentTm.tm_mon + 1, CurrentTm.tm_mday, CurrentTm.tm_hour,
				CurrentTm.tm_min, CurrentTm.tm_sec, FunctionLib::GetCurThreadId());
			break;

		case LogLevel::LogError:
			snprintf(szLog, LOG_MAX_LENGTH, "[%04d-%02d-%02d %02d:%02d:%02d][Thread %04x] Error:",
				CurrentTm.tm_year + 1990, CurrentTm.tm_mon + 1, CurrentTm.tm_mday, CurrentTm.tm_hour,
				CurrentTm.tm_min, CurrentTm.tm_sec, FunctionLib::GetCurThreadId());
			break;

		default:
			break;
		}

		uLogSize = strnlen(szLog, LOG_MAX_LENGTH);
		
		va_start(argList, szFormat);
		vsnprintf(szLog + uLogSize, LOG_MAX_LENGTH - uLogSize, szFormat, argList);
		va_end(argList);

		strncat(szLog, "\n", 2);

		m_mutex.lock();
		fputs(szLog, m_pLogFile);
		printf(szLog);
		m_uLogCount++;
		m_mutex.unlock();
		
	}

	return;
}

BOOL CLog::SetLogLevel(UINT32 uLevel){

	m_uLogLevel = uLevel;

	return TRUE;
}
