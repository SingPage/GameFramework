#pragma once

class CLuaManager {
	CLuaManager();
	virtual ~CLuaManager();
public:
	static CLuaManager*	GetInstancePtr();

	BOOL				InitLuaState();

	lua_State*			GetLuaState();

	BOOL				SetLuaState(lua_State* pLuaState);

	BOOL				CloseLua();

	BOOL				LoadDirLuaFile(const CHAR* pszDir);

	BOOL				LoadLuaFile(const CHAR* pszLuaFile);

	BOOL				CallLuaFunction(CHAR * szLuaFunName, CHAR* szParmSign, ...);

	BOOL				RegisterFunction(const CHAR* szFunName, lua_CFunction Fun);

	BOOL				GetStackData(INT32 nStackIndex, CHAR cDataSig, VOID* DataVar);

private:
	lua_State*			m_pLuaState;

};