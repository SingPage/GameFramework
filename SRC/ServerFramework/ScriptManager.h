#pragma once

class CScriptManager {
	CScriptManager();
	~CScriptManager();

public:
	enum ScriptType{
		LUA
	};

	static CScriptManager* GetInstancePtr();

	BOOL	InitLuaVM();

	BOOL	CloseLuaVM();
};