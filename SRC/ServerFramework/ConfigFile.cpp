#include"stdafx.h"
#include"ConfigFile.h"

CConfigFile::CConfigFile() {

	FUNCTION_RUNNING

	FUNCTION_FINISHING
}

CConfigFile::~CConfigFile(){

	FUNCTION_RUNNING

	FUNCTION_FINISHING
}

CConfigFile * CConfigFile::GetInstancePtr(){

	FUNCTION_RUNNING

	static CConfigFile ConfigFile;

	return &ConfigFile;

	FUNCTION_FINISHING
}

BOOL CConfigFile::LoadConfigFile(CHAR * szFileName){

	FUNCTION_RUNNING

	if (NULL == szFileName) {
		LOG_ERROR_OUTPUT("szFileName is \"NULL\"");
		return FALSE;
	}

	FILE* ConfigFile = fopen(szFileName, "r");
	if (NULL == ConfigFile) {
		LOG_ERROR_OUTPUT("fopen(%s, \"r\") error", szFileName);
		return FALSE;
	}

	CHAR szBuff[1024];
	CHAR* szKey;
	CHAR* szValue;

	while (!feof(ConfigFile)) {

		szKey = fgets(szBuff, 1024, ConfigFile);
		szValue = strchr(szBuff, '=');
		if (NULL == szValue) {
			continue;
		}
		*szValue = '\0';
		szValue++;

		FunctionLib::StringTrim(&szKey);
		FunctionLib::StringTrim(&szValue);

		std::string strKey = szKey;
		std::string strValue = szValue;

		m_ConfigMap.insert(std::make_pair(szKey, szValue));
	}

	fclose(ConfigFile);

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CConfigFile::OutputConfigToFIle(CHAR * szFileName){

	FUNCTION_RUNNING

	if (NULL == szFileName) {
		LOG_ERROR_OUTPUT("szFileName is \"NULL\"", szFileName);
		return FALSE;
	}

	FILE* ConfigFile = fopen(szFileName, "w+");
	if (NULL == ConfigFile) {
		LOG_ERROR_OUTPUT("fopen(%s, \"w+\") error", szFileName);
		return FALSE;
	}

	for (std::map<std::string, std::string>::iterator itor = m_ConfigMap.begin(); itor != m_ConfigMap.end(); itor++) {
		fprintf(ConfigFile,"%s=%s\n", itor->first.c_str(), itor->second.c_str());
	}

	fclose(ConfigFile);
	return TRUE;

	FUNCTION_FINISHING
}

BOOL CConfigFile::GetStringValue(std::string strKey, std::string & strValue){

	auto itor =  m_ConfigMap.find(strKey);
	if (m_ConfigMap .end() == itor) {
		return FALSE;
	}

	strValue = itor->second;

	return TRUE;
}

BOOL CConfigFile::GetIntValue(std::string strKey, INT32 & nValue){

	auto itor = m_ConfigMap.find(strKey);
	if (m_ConfigMap.end() == itor) {
		return FALSE;
	}

	nValue = atoi(itor->second.c_str());

	return TRUE;
}

BOOL CConfigFile::GetDoubleValue(std::string strKey, DOUBLE & dblValue){

	auto itor = m_ConfigMap.find(strKey);
	if (m_ConfigMap.end() == itor) {
		return FALSE;
	}

	dblValue = atof(itor->second.c_str());

	return TRUE;
}

BOOL CConfigFile::InsertConfig(std::string strKey, std::string strValue){

	auto itor = m_ConfigMap.find(strKey);
	if (m_ConfigMap.end() != itor) {
		return FALSE;
	}

	m_ConfigMap.insert(std::make_pair(strKey, strValue));

	return TRUE;
}
