#pragma once

namespace FunctionLib{
	tm			GetCurrentTm();

	UINT32		GetCurThreadId();

	SOCKET		CreateSocket(INT32 nDomain, INT32 nType, INT32 nProtocol = 0);

	BOOL		SetSocketAttr(SOCKET nSocket, INT32 nFlags);

	BOOL		ConnectSocket(SOCKET nSocket, const CHAR* pIPAddr, UINT16 usPortNum);
}