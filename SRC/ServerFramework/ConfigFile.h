#pragma once

class CConfigFile {
	CConfigFile();
	~CConfigFile();
public:
	static CConfigFile* GetInstancePtr();

	BOOL	LoadConfigFile(CHAR* szFileName);

	BOOL	OutputConfigToFIle(CHAR* szFileName);

	BOOL	GetStringValue(std::string strKey, std::string& strValue);

	BOOL	GetIntValue(std::string strKey, INT32& nValue);

	BOOL	GetDoubleValue(std::string strKey, DOUBLE& dblValue);

	BOOL	InsertConfig(std::string strKey, std::string strValue);

private:
	std::map<std::string, std::string> m_ConfigMap;
};