////////////////////////////////////////////////////////////////////////////
//
// INI File Read/Write routines 
//
// Created by: Sohan R Ranjan, ISIS Center, Georgetown University
//
// Date: 23 Sept 2003
//
////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <windows.h> 
#include <string.h>
#include <memory.h>
#include "IniFileRW.h"


////////////////////////////////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////////////////////////////////

IniFileRW::IniFileRW( const char *fileName )
{
	m_FileName = NULL;
	SetFileName( fileName );
}
	
////////////////////////////////////////////////////////////////////////////
/// Destructor
////////////////////////////////////////////////////////////////////////////
IniFileRW::~IniFileRW()
{
	if (m_FileName!=NULL)
		delete m_FileName;
}
////////////////////////////////////////////////////////////////////////////
/// Method: SetFileName
//
/// Input: filename: Name of the ini file 
///
/// Returns: true if successfull.
///
/// To do: Add checking if the file exists or not
////////////////////////////////////////////////////////////////////////////
bool IniFileRW::SetFileName(const char *fileName)
{
	if (fileName == NULL) return false;

	FILE *fp =fopen(fileName,"r");
	
	if (fp != NULL)
	{
		fclose(fp);
		if (m_FileName) delete m_FileName;
		m_FileName = (char *) malloc(sizeof(char) * (strlen(fileName)+10));
		if (m_FileName)
		{
			sprintf(m_FileName, "%s", fileName );
			printf("Parameter file %s found.\n", m_FileName);
			return true;
		}
		else
		{
			printf("Memory allocation error!!!\n");
			return false;
		}
	}
	else
	{
		printf("Parameter file %s NOT FOUND.\n", fileName);
		return false;
	}
}
////////////////////////////////////////////////////////////////////////////
/// Method: ReadParameter
/// 
/// Input: char *sectionName = Section name
///        char *variableName = Variable Name 
///		   char *defaultValue = Default Value 
///        int size = size of the parameter  
///        char *value  = parameter value (string)
///
/// Return: char *value : as defined above 
///
/// This routine read a char value from the ini file under the specified 
/// section and the specified variable
////////////////////////////////////////////////////////////////////////////
void IniFileRW::ReadParameter(char *sectionName, char *variableName, 
		char *defaultValue, int size,  char *value )
{
	if(m_FileName!=NULL)
	{
		GetPrivateProfileString((LPCSTR)sectionName,(LPCSTR)variableName,(LPCSTR)defaultValue,
			(LPSTR)value, (DWORD)size, (LPCSTR)m_FileName);
	}
	else
	{
		sprintf(value, "%s", defaultValue);
	}
}
////////////////////////////////////////////////////////////////////////////
/// Method: ReadParameter
/// 
/// Input: char *sectionName = Section name
///        char *variableName = Variable Name 
///		   char *defaultValue = Default Value 
///        int size = size of the parameter   
///        int *value = parameter value (int) 
///
/// Return: int *value : as defined above 
///
/// This routine read a int value from the ini file under the specified 
/// section and the specified variable
////////////////////////////////////////////////////////////////////////////
void IniFileRW::ReadParameter(char *sectionName, char *variableName, 
		char *defaultValue, int size, int *value)
{
	if(m_FileName!=NULL)
	{
		GetPrivateProfileString(sectionName,variableName,defaultValue,
			m_String, size, m_FileName);
	}
	else
	{
		sprintf(m_String, "%s", defaultValue);
	}
	*value = atoi( m_String );
}
////////////////////////////////////////////////////////////////////////////
/// Method: ReadParameter
/// 
/// Input: char *sectionName = Section name
///        char *variableName = Variable Name 
///		   char *defaultValue = Default Value 
///        int size = size of the parameter   
///        bool *value = parameter value (int) 
///
/// Return: bool *value : as defined above 
///
/// This routine read a bool value from the ini file under the specified 
/// section and the specified variable
/// 
/// NOTE: REPLACE GetPrivateProfileString by GetPrivateProfileInt
/// 
////////////////////////////////////////////////////////////////////////////
void IniFileRW::ReadParameter(char *sectionName, char *variableName, 
		char *defaultValue, int size,  bool *value )
{
	if(m_FileName!=NULL)
	{
		GetPrivateProfileString(sectionName, variableName, defaultValue,
			m_String, size, m_FileName);
	}
	else
	{
		sprintf(m_String, "%s", defaultValue);
	}
	int intValue = atoi( m_String );
	*value = ( intValue == 1 ? TRUE : FALSE );
}
////////////////////////////////////////////////////////////////////////////
/// Method: WriteParameter
/// 
/// Input: char *sectionName = Section name
///        char *variableName = Variable Name 
///        int *value = parameter value (int) 
///
/// Return: None 
///
/// This routine writes an int value to the ini fileunder the specified 
/// section and the specified variable
////////////////////////////////////////////////////////////////////////////
void IniFileRW::WriteParameter(char *sectionName, char *variableName, int value)
{
	if(m_FileName!=NULL)
	{
		sprintf( m_String, "%d", value );
		WritePrivateProfileString(sectionName, variableName, m_String, m_FileName);
	}
}
////////////////////////////////////////////////////////////////////////////
/// Method: WriteParameter
/// 
/// Input: char *sectionName = Section name
///        char *variableName = Variable Name 
///        char *str = parameter value (int) 
///
/// Return: None 
///
/// This routine writes a char string to the ini file under the specified 
/// section and the specified variable
////////////////////////////////////////////////////////////////////////////
void IniFileRW::WriteParameter(char *sectionName, char *variableName, 
		char *str)
{
	if(m_FileName!=NULL)
	{
		WritePrivateProfileString(sectionName, variableName, str, m_FileName);
	}
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
