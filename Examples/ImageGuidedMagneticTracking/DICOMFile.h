//
//			DICOMFile.h: interface for the CDICOMFile class.
//
//				Copyright by ZH, 2000/3/1
//
//////////////////////////////////////////////////////////////////////

#ifndef _DICOMFILE_H_
#define _DICOMFILE_H_

#include "stdio.h"

typedef struct
{
	unsigned short	mGroupLength;
	unsigned short	unGroupID ;
	unsigned short	unElementName ;
	long	ulValueLength ;
	char	sValue[10*1024] ;
	short	sValueLength ;
} T_DATAELEMENT;

class CDICOMFile  
{
public:
	void DeleteBuffer();
	void NewBuffer();
	int m_CX, m_CY;
	int m_WinCenter, m_WinWidth;
	short* m_pImage;
	void TranslateGroup0x08(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x09(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x10(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x18(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x19(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x20(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x21(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x28(int unElementName , int ulValueLength , char* psValue);
	void TranslateGroup0x33(int unElementName , int ulValueLength , char* psValue);
	void ReadStringValue(T_DATAELEMENT* pElement, FILE* pFile);
	void TransElement(int unGroupID, int unElementName, int ulValueLength, char* psValue);
	void IgnoreGroup(int unGroupID, int unElementName, char* psValue, FILE* pFile);
	bool ReadElement(T_DATAELEMENT* pElement, FILE* pFile);
	bool Open(FILE* pFile);
	T_DATAELEMENT m_Element;
	CDICOMFile();
	virtual ~CDICOMFile();

  void Swap2(void* val);
  bool m_Swap;
  void SetSwap(bool b);
  void SwapImage();

  double m_SliceLocation;
  float m_Spacing[2];
  float m_Position[3];
};

#endif 
