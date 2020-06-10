#include"stdafx.h"
#include"FuncLib.h"

tm FunctionLib::GetCurrentTm(){

	FUNCTION_RUNNING

	time_t rawTime;

	time(&rawTime);

	return *localtime(&rawTime);

	FUNCTION_FINISHING
}

UINT32 FunctionLib::GetCurThreadId(){

	FUNCTION_RUNNING

	return UINT32(pthread_self());

	FUNCTION_FINISHING
}

SOCKET FunctionLib::CreateSocket(INT32 nDomain, INT32 nType, INT32 nProtocol){

	FUNCTION_RUNNING

	SOCKET nSocket = socket(nDomain, nType, nProtocol);
	if ((0 == nSocket) || (-1 == nSocket)) {
		LOG_ERROR_OUTPUT("Create socket fail:", strerror(errno));

		return -1;
	}

	return nSocket;

	FUNCTION_FINISHING
}

BOOL FunctionLib::SetSocketAttr(SOCKET nSocket, INT32 nFlags){

	FUNCTION_RUNNING

	INT32 nInitFlags = fcntl(nSocket, F_GETFL, 0);
	if (-1 == nInitFlags) {
		LOG_ERROR_OUTPUT("Fcntl \"F_GETFL\" fail:%s", strerror(errno));
		return FALSE;
	}

	if (-1 == fcntl(nSocket, F_SETFL, nInitFlags | nFlags)) {
		LOG_ERROR_OUTPUT("Fcntl \"F_SETFL\" fail:%s", strerror(errno));
		return FALSE;
	}

	return TRUE;

	FUNCTION_FINISHING
}

BOOL FunctionLib::ConnectSocket(SOCKET nSocket, const CHAR* pIPAddr, UINT16 usPortNum){

	FUNCTION_RUNNING

	sockaddr_in ServiceAddr;

	if (NULL == pIPAddr) {
		LOG_ERROR_OUTPUT("pIPAddr is \"NULL\"");
		return FALSE;
	}
	ServiceAddr.sin_family = AF_INET;
	ServiceAddr.sin_port = htons(usPortNum);
	inet_pton(AF_INET, pIPAddr, &ServiceAddr.sin_addr);

	if (0 != connect(nSocket, (sockaddr*)&ServiceAddr, sizeof(ServiceAddr))) {

		if ((EWOULDBLOCK == errno) || (EINPROGRESS == errno)) {
			return TRUE;
		}

		LOG_ERROR_OUTPUT("connect error:%s", strerror(errno));
		return FALSE;
	}

	return TRUE;

	FUNCTION_FINISHING
}

BOOL FunctionLib::GetDirAllFile(const CHAR * pszDir, const CHAR * pszFileType, 
	std::list<std::string>& lsFileList, INT32 nSearchDepth){

	FUNCTION_RUNNING

	DIR* pDirInfo;
	struct dirent* pDirent;
	struct stat Stat;
	
	if (NULL == pszDir || NULL == pszFileType || nSearchDepth < -1) {

		LOG_ERROR_OUTPUT("Parameter is illegal");
		return FALSE;
	}

	std::string strTempPath = pszDir;
	if ('/' == pszDir[strlen(pszDir) - 1] || '\\' == pszDir[strlen(pszDir) - 1]) {

		strTempPath.push_back('/');
	}

	pDirInfo = opendir(pszDir);
	if (NULL == pDirInfo) {

		LOG_ERROR_OUTPUT("opendir(pszDir) return \"NULL\":%s", strerror(errno));
		return FALSE;
	}

	while(NULL != (pDirent = readdir(pDirInfo))){
		pDirent = readdir(pDirInfo);
		if (0 == strcmp(".", pDirent->d_name) || 0 == strcmp("..", pDirent->d_name)) {
			continue;
		}

		lstat(pDirent->d_name, &Stat);
		if (S_IFDIR & Stat.st_mode && nSearchDepth) {
			GetDirAllFile(pDirent->d_name, pszFileType, lsFileList, nSearchDepth - 1);
		}
		else {
			if (TRUE == FunctionLib::StringMatch(pDirent->d_name, pszFileType)) {
				lsFileList.emplace_back(strTempPath + pDirent->d_name);
			}
		}
	}

	closedir(pDirInfo);

	return TRUE;

	FUNCTION_FINISHING
}

BOOL FunctionLib::StringMatch(const CHAR * pszSourceStr, const CHAR * pszPatternStr){

	FUNCTION_RUNNING

	if (NULL == pszSourceStr || NULL == pszPatternStr) {
			return FALSE;
	}

	UINT32 uSourceSize = strlen(pszSourceStr);
	UINT32 uPatternSize = strlen(pszPatternStr);
	std::vector<std::vector<BOOL>> vtDp(uSourceSize + 1, std::vector<BOOL>(uPatternSize + 1, FALSE));

	vtDp[0][0] = TRUE;

	for (UINT32 i = 1; i <= uPatternSize; i++) {
		if ('*' == pszPatternStr[i - 1]) {
			vtDp[0][i] = vtDp[0][i - 1];
		}
	}

	for (UINT32 i = 1; i <= uSourceSize; i++) {

		for (UINT32 j = 1; j <= uPatternSize; j++) {

			if (pszPatternStr[j - 1] == pszSourceStr[i - 1] || '?' == pszPatternStr[j - 1]) {
				vtDp[i][j] = vtDp[i - 1][j - 1];
			}
			else if ('*' == pszPatternStr[j - 1]) {
				vtDp[i][j] = vtDp[i - 1][j] || vtDp[i][j - 1];
			}
		}
	}

	return vtDp[uSourceSize][uPatternSize];

	FUNCTION_FINISHING
}

BOOL FunctionLib::StringTrim(CHAR** szSourceString){

	CHAR szBlankChar[] = { '\t', '\n', '\r' };
	
	if (NULL == *szSourceString) {
		return FALSE;
	}

	CHAR* szStringTail = (*szSourceString) + strlen(*szSourceString) - 1;

	BOOL bEnd = FALSE;
	while (FALSE == bEnd) {
		for (INT32 i = 0; i < sizeof(szBlankChar); i++) {
			if (szBlankChar[i] == (**szSourceString)) {

				**szSourceString = '\0';
				(*szSourceString)++;
				break;
			}

			if (sizeof(szBlankChar) - 1 == i) {
				bEnd = TRUE;
			}
		}
	}

	bEnd = FALSE;
	while (FALSE == bEnd) {
		for (INT32 i = 0; i < sizeof(szBlankChar); i++) {
			if (szBlankChar[i] == (*szStringTail)) {

				*szStringTail = '\0';
				szStringTail--;
				break;
			}

			if (sizeof(szBlankChar) - 1 == i) {
				bEnd = TRUE;
			}
		}
	}

	return TRUE;
}
