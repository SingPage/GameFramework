#pragma once

namespace FunctionLib{
	tm			GetCurrentTm();

	UINT32		GetCurThreadId();

	SOCKET		CreateSocket(INT32 nDomain, INT32 nType, INT32 nProtocol = 0);

	BOOL		SetSocketAttr(SOCKET nSocket, INT32 nFlags);

	BOOL		ConnectSocket(SOCKET nSocket, const CHAR* pIPAddr, UINT16 usPortNum);

	//nSearchDepth: -1: No limit 0: Root dir >0: Search dir depth
	BOOL		GetDirAllFile(const CHAR* pszDir, const CHAR* pszFileType, std::list<std::string>& lsFileList, INT32 nSearchDepth);

	BOOL		StringMatch(const CHAR* pszSourceString, const CHAR* pszPatternString);

	BOOL		StringTrim(CHAR** szSourceString);
}