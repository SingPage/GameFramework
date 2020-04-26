#include"stdafx.h"
#include"FuncLib.h"

tm FunctionLib::GetCurrentTm(){

	time_t rawTime;

	time(&rawTime);

	return *localtime(&rawTime);
}

UINT32 FunctionLib::GetCurThreadId(){

	return UINT32(pthread_self());
}

SOCKET FunctionLib::CreateSocket(INT32 nDomain, INT32 nType, INT32 nProtocol){

	SOCKET nSocket = socket(nDomain, nType, nProtocol);
	if ((0 == nSocket) || (-1 == nSocket)) {
		LOG_ERROR_OUTPUT("Create socket fail:", strerror(errno));

		return -1;
	}

	return nSocket;
}

BOOL FunctionLib::SetSocketAttr(SOCKET nSocket, INT32 nFlags){

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
}

BOOL FunctionLib::ConnectSocket(SOCKET nSocket, const CHAR* pIPAddr, UINT16 usPortNum){

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
}
