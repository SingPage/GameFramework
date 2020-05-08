#include"stdafx.h"
#include"ScriptManager.h"

CScriptManager::CScriptManager() {

}

CScriptManager::~CScriptManager() {

}

CScriptManager * CScriptManager::GetInstancePtr(){

	static CScriptManager ScriptManager;

	return &ScriptManager;
}
