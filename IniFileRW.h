/////////////////////////////////////////////////////////////////////////////////
//
// Utility routines to 
//	1. Read/Write data from/to an ini file (IniFileRW.h/IniFileRW.cxx)
//	2. Read error message from a message error list file
//
// Uses following Windows support routines
// GetPrivateProfileString(char *psSectionName, char *psVariableName, 
//	char *psDefaultValue, char *psStringValue, int nSize, char *psFileName);
// WritePrivateProfileString(char *psSectionName, char *psVariableName, 
//	char *psValue,  char *psFileName);
//
//
/////////////////////////////////////////////////////////////////////////////////
#ifndef __IniFileRW_h__
#define __IniFileRW_h__


#include <stdio.h>

class IniFileRW
{
public:
	
	IniFileRW(const char *fileName = NULL);
	
	~IniFileRW();
	
	bool SetFileName(const char *fileName);

	void ReadParameter(char *sectionName, char *variableName, 
		char *defaultValue, int size,  char *str );

	void ReadParameter(char *sectionName, char *variableName, 
		char *defaultValue, int size, int *value);

	void ReadParameter(char * sectionName, char *variableName, 
		char *defaultValue, int size,  bool *value );

	void WriteParameter(char *sectionName, char *variableName, int value);

	void WriteParameter(char *sectionName, char *variableName, 
		char *value);
	
private:
	
	char	m_String[256];

	char	*m_FileName;
};

#endif
