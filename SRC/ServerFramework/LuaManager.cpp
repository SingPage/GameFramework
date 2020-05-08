#include<lua.hpp>
#include"stdafx.h"
#include"LuaManager.h"

CLuaManager::CLuaManager() {
	
	FUNCTION_RUNNING

	m_pLuaState = NULL;

	FUNCTION_FINISHING
}

CLuaManager::~CLuaManager() {

	FUNCTION_RUNNING

	FUNCTION_FINISHING
}

CLuaManager * CLuaManager::GetInstancePtr(){

	FUNCTION_RUNNING

	CLuaManager LuaManager;

	return &LuaManager;

	FUNCTION_FINISHING
}

lua_State * CLuaManager::GetLuaState(){

	FUNCTION_RUNNING

	return m_pLuaState;

	FUNCTION_FINISHING
}

BOOL CLuaManager::SetLuaState(lua_State * pLuaState){

	FUNCTION_RUNNING

	if (NULL == pLuaState || NULL != m_pLuaState) {
		return FALSE;
	}

	m_pLuaState = pLuaState;

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CLuaManager::CloseLua(){

	FUNCTION_RUNNING

	if (NULL == m_pLuaState) {

		lua_close(m_pLuaState);
		m_pLuaState = NULL;

		return TRUE;
	}

	return FALSE;

	FUNCTION_FINISHING
}

BOOL CLuaManager::LoadDirLuaFile(const CHAR * pszDir){

	FUNCTION_RUNNING

	if (NULL == pszDir) {
		return FALSE;
	}

	std::list<std::string> lsLuaFileList;
	
	if (FunctionLib::GetDirAllFile(pszDir, "*.lua", lsLuaFileList, -1)) {
		LOG_ERROR_OUTPUT("GetDirAllFile(%s, \"*.lua\", lsLuaFileList, -1) fail", pszDir);
		return FALSE;
	};

	for (std::list<std::string>::iterator itor = lsLuaFileList.begin(); itor != lsLuaFileList.end(); itor++) {

		if (LoadLuaFile(itor->c_str())) {
			LOG_ERROR_OUTPUT("LoadLuaFile(%s) fail", itor->c_str());
		}

	}

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CLuaManager::LoadLuaFile(const CHAR * pszLuaFile){

	FUNCTION_RUNNING

	if (NULL == m_pLuaState) {
		return FALSE;
	}

	if (0 != luaL_dofile(m_pLuaState, pszLuaFile)) {
		LOG_ERROR_OUTPUT("luaL_dofile(%p, %s) fail", m_pLuaState, pszLuaFile);
		return FALSE;
	}

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CLuaManager::CallLuaFunction(CHAR * szLuaFunName, CHAR* szParmSign, ...){

	FUNCTION_RUNNING

	if (NULL == szLuaFunName || NULL == szParmSign) {
		LOG_ERROR_OUTPUT("szLuaFunName or szParmSign is \"NULL\"");
		return FALSE;
	}

	UINT32 uInParmNum = 0;
	UINT32 uOutParamNUm = 0;
	CHAR* pInParm = szParmSign;
	CHAR* pOutParm = strchr(szParmSign, '=');
	INT32 nStackTop = lua_gettop(m_pLuaState);
	

	if (NULL == pOutParm) {
		LOG_ERROR_OUTPUT("szParmSign:%s error", szParmSign);
		return FALSE;
	}
	pOutParm++;
	uOutParamNUm = strlen(pOutParm);

	va_list VaList;
	va_start(VaList, szParmSign);
	
	lua_getglobal(m_pLuaState, szLuaFunName);
	if (!lua_isfunction(m_pLuaState, -1)) {
		LOG_ERROR_OUTPUT("lua_getglobal(%p, %s)", m_pLuaState, szLuaFunName);
		lua_settop(m_pLuaState, nStackTop);
		return FALSE;
	}

	BOOL bEnd = FALSE;
	while (FALSE == bEnd) {

		luaL_checkstack(m_pLuaState, 1, "Too many parm");
		switch(*pInParm){

		case 'i':
			lua_pushinteger(m_pLuaState, va_arg(VaList, int));
			uInParmNum++;
			break;
		case 'd':
			lua_pushnumber(m_pLuaState, va_arg(VaList, double));
			uInParmNum++;
			break;
		case 'f':
			lua_pushnumber(m_pLuaState, va_arg(VaList, float));
			uInParmNum++;
			break;
		case 'l':
			lua_pushinteger(m_pLuaState, va_arg(VaList, long long));
			uInParmNum++;
			break;
		case 's':
			lua_pushstring(m_pLuaState, va_arg(VaList, char*));
			uInParmNum++;
			break;
		case 'p':
			lua_pushlightuserdata(m_pLuaState, va_arg(VaList, void*));
			uInParmNum++;
			break;
		case 'b':
			lua_pushboolean(m_pLuaState, va_arg(VaList, bool));
			uInParmNum++;
			break;
		case '=':
			bEnd = TRUE;
			uInParmNum++;
			break;
		default:
			LOG_ERROR_OUTPUT("InParm type error");
			lua_settop(m_pLuaState, nStackTop);
			va_end(VaList);
			return FALSE;
		}

		pInParm++;
	}

	if (lua_pcall(m_pLuaState, uInParmNum, uOutParamNUm, 0)){
		LOG_ERROR_OUTPUT("lua_pcall error:%s", lua_tostring(m_pLuaState, -1));
		lua_settop(m_pLuaState, nStackTop);
		va_end(VaList);
		return FALSE;
	}
	
	bEnd = FALSE;
	INT32 nIndex= -uOutParamNUm;
	while (FALSE == bEnd) {

		switch(*pOutParm){
		case 'i':
			if (!lua_isinteger(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isinteger(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, int*) = lua_tointeger(m_pLuaState, nIndex);
			break;
		case 'd':
			if (!lua_isnumber(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isnumber(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, double*) = lua_tonumber(m_pLuaState, nIndex);
			break;
		case 'f':
			if (!lua_isnumber(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isnumber(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, float*) = lua_tonumber(m_pLuaState, nIndex);
			break;
		case 'l':
			if (!lua_isinteger(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isinteger(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, long long*) = lua_tointeger(m_pLuaState, nIndex);
			break;
		case 's':
			if (!lua_isstring(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isstring(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, const char**) = lua_tostring(m_pLuaState, nIndex);
			break;
		case 'p':
			if (!lua_isuserdata(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isuserdata(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, void**) = lua_touserdata(m_pLuaState, nIndex);
			break;
		case 'b':
			if (!lua_isboolean(m_pLuaState, nIndex)) {
				LOG_ERROR_OUTPUT("lua_isboolean(%p, %d)", m_pLuaState, nIndex);
				lua_settop(m_pLuaState, nStackTop);
				va_end(VaList);
				return FALSE;
			}
			*va_arg(VaList, bool*) = lua_toboolean(m_pLuaState, nIndex);
			break;
		case 0:
			bEnd = TRUE;
			break;
		default:
			LOG_ERROR_OUTPUT("OutParm type error");
			lua_settop(m_pLuaState, nStackTop);
			va_end(VaList);
			return FALSE;
		}

		nIndex++;
	}

	va_end(VaList);
	lua_settop(m_pLuaState, nStackTop);

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CLuaManager::RegisterFunction(const CHAR * szFunName, lua_CFunction Fun){

	FUNCTION_RUNNING

	lua_register(m_pLuaState, szFunName, Fun);

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CLuaManager::GetStackData(INT32 nStackIndex, CHAR cDataSig, VOID * DataVar){

	FUNCTION_RUNNING

	if (NULL == cDataSig) {

		LOG_ERROR_OUTPUT("cDataSig is \"NULL\"");
		return FALSE;
	}

	switch (cDataSig) {
	case 'i':
		if (!lua_isinteger(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isinteger(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(INT32*)DataVar = lua_tointeger(m_pLuaState, nStackIndex);
		break;

	case 'd':
		if (!lua_isnumber(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isnumber(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(double*)DataVar = lua_tonumber(m_pLuaState, nStackIndex);
		break;

	case 'f':
		if (!lua_isnumber(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isnumber(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(float*)DataVar = lua_tonumber(m_pLuaState, nStackIndex);
		break;

	case 'l':
		if (!lua_isinteger(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isinteger(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(long long*)DataVar = lua_tointeger(m_pLuaState, nStackIndex);
		break;

	case 's':
		if (!lua_isstring(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isstring(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(const char**)DataVar = lua_tostring(m_pLuaState, nStackIndex);
		break;

	case 'p':
		if (!lua_isuserdata(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isuserdata(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(void**)DataVar = lua_touserdata(m_pLuaState, nStackIndex);
		break;
	case 'b':
		if (!lua_isboolean(m_pLuaState, nStackIndex)) {
			LOG_ERROR_OUTPUT("lua_isboolean(%p, %d)", m_pLuaState, nStackIndex);
			return FALSE;
		}
		*(bool*)DataVar = lua_toboolean(m_pLuaState, nStackIndex);
		break;
	default:
		LOG_ERROR_OUTPUT("cDataSig type error");
		return FALSE;
	}

	

	return TRUE;

	FUNCTION_FINISHING
}
