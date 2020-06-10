#pragma once
class CConnection;

class IDataBuffer{
public:
	virtual CHAR*	GetBufferPtr() = 0;

	virtual VOID	Iint() = 0;

	virtual BOOL	AddRef() = 0;

	virtual BOOL	ReduceRef() = 0;

	virtual INT32	GetRefCount() = 0;

	virtual UINT32	GetBufferLength() = 0;

	virtual CHAR*	GetDataTail() = 0;

	virtual VOID	SetDataLength(UINT32 uDataLength) = 0;

	virtual UINT32	GetDataLength() = 0;

	virtual BOOL	CopyFromBuffer(IDataBuffer& dataBuffer, UINT32 size) = 0;

	virtual VOID	LockMutex() = 0;

	virtual VOID	UnlockMutex() = 0;

	virtual BOOL	Free() = 0;
};

struct PacketHeader {
	UINT32   uMsgID;

	UINT32   uSize;

	UINT32	 uCheckCode;

	UINT32   uPacketNo;

	UINT32   ulTargetID;
};

struct NetPacket {
	PacketHeader	pHeader;

	UINT32			uConnID;

	IDataBuffer*	pBuffer;

	IDataBuffer*	pSelf;
};

class IDataHandler {
public:
	virtual BOOL	NetPacketHandler(NetPacket* pPacket) = 0;

	virtual BOOL	NewConnectionHandler(CConnection* pConn) = 0;

	virtual BOOL	CloseConnectionHandler(CConnection* pConn) = 0;
};



class INetPacketHandler {
public:
	virtual BOOL	MessageDispatch(NetPacket* pPacket) = 0;
};