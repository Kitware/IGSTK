//
//			DICOMFile.cpp: implementation of the CDICOMFile class.
//
//				Copyright by ZH, 2000/3/1
//
//////////////////////////////////////////////////////////////////////

#include "DICOMFile.h"
#include "string.h"
#include "stdlib.h"


CDICOMFile::CDICOMFile()
{
	m_pImage=NULL;
  m_Swap = false;
}

CDICOMFile::~CDICOMFile()
{
	DeleteBuffer();
}

bool CDICOMFile::Open(FILE *pFile)
{
	unsigned short temp;
	bool ret;
	char pTemp[4];
//	pFile->Read(&temp, sizeof(unsigned short));
  fread(&temp, sizeof(unsigned short), 1, pFile);
	if (temp == 0 ) 
	{
//		pFile->Seek(0x80, CFile::begin);
    fseek(pFile, 0x80, SEEK_SET);
//		pFile->Read(pTemp, 4);
    fread(pTemp, 4, 1, pFile);
		if (strncmp(pTemp, "DICM", 4))
		{
			return false;
		}
	}
	else
	{
//		pFile->SeekToBegin();
    fseek(pFile, 0, SEEK_SET);
	}
	while (true)
	{
		ret=ReadElement(&m_Element, pFile); 
    if (!ret)
		{
			return false;
		}
		if(m_Element.unGroupID == 0x7fe0 && m_Element.unElementName == 0x10)
		{
			break;
		}
   	    if (m_Element.unElementName == 0x0000)
		{
			// All following groups are which we needed
			switch (m_Element.unGroupID)		
			{
//			case 0x0002:
			case 0x0008:
//			case 0x0009:
			case 0x0010:
			case 0x0018:
//			case 0x0019:
//			case 0x0020:
//			case 0x0021:
			case 0x0028:
//			case 0x0033:
//			case 0x0054:
				TransElement(m_Element.unGroupID, m_Element.unElementName, m_Element.ulValueLength , m_Element.sValue);
				break;
			default:
				IgnoreGroup(m_Element.unGroupID, m_Element.unElementName, m_Element.sValue, pFile);
				break;
			}
		}
		else
		{
			TransElement(m_Element.unGroupID, m_Element.unElementName, m_Element.ulValueLength , m_Element.sValue);
		}
	}
	DeleteBuffer();
	NewBuffer();
//	pFile->Read(m_pImage, m_CX*m_CY*sizeof(short));
  fread(m_pImage, m_CX * m_CY * sizeof(short), 1, pFile);
	return true;
}

bool CDICOMFile::ReadElement(T_DATAELEMENT *pElement, FILE *pFile)
{
//  pFile->Read(&(pElement->unGroupID), sizeof(unsigned short));
  fread(&pElement->unGroupID, sizeof(unsigned short), 1, pFile);
//	pFile->Read(&(pElement->unElementName), sizeof(unsigned short));
  fread(&pElement->unElementName, sizeof(unsigned short), 1, pFile);
  if (m_Swap && pElement->unGroupID != 0x02)
  {
    Swap2(&pElement->unGroupID);
    Swap2(&pElement->unElementName);
  }
	// if reach data buffer	
	if (pElement->unGroupID == 0x7fe0 && pElement->unElementName == 0x0010)
	{
		return true;		
	}
	switch (pElement->unGroupID)
	{
	// group 0x0002, may have 2-ascii char in the value buffer
	case 0x0002:
		switch (pElement->unElementName)
		{
		case 0x0001:
//			pFile->Read(&(pElement->ulValueLength), sizeof(unsigned long));
      fread(&pElement->ulValueLength, sizeof(unsigned long), 1, pFile);
			if (pElement->ulValueLength >= 0x4141)
			{
//				pFile->Read(&(pElement->ulValueLength), sizeof(unsigned long));
        fread(&pElement->ulValueLength, sizeof(unsigned long), 1, pFile);
			}
			break;
		default:
//			pFile->Read(&(pElement->ulValueLength), sizeof(unsigned long));
      fread(&pElement->ulValueLength, sizeof(unsigned long), 1, pFile);
			if (pElement->ulValueLength >= 0x4141)
			{
				pElement->ulValueLength/=0xffff;        
			}
		}
		break;
	// group 0x0033, may have unmeaning string between elements
	case 0xffff:
		switch (pElement->unElementName)
		{
		case 0xffff:
			pElement->ulValueLength=8;
			break;
		case 0xe00d:
			pElement->ulValueLength=12;
			break;
		}
		break;
	default:
//		pFile->Read(&(pElement->ulValueLength), sizeof(unsigned long));
    fread(&pElement->ulValueLength, sizeof(unsigned long), 1, pFile);
		// may have 2-ascii char in the value buffer
		// not compatible with all dicom3 file
		if (pElement->ulValueLength >= 0x4141)
		{
			pElement->ulValueLength/=0xffff;
      
		}
    if (m_Swap)
    {
      Swap2(&pElement->ulValueLength);
    }
	}
	ReadStringValue(pElement, pFile); 
	return true;
}

void CDICOMFile::TransElement(int unGroupID, int unElementName, int ulValueLength, char *psValue)
{
	switch (unGroupID)
	{
	case 0x0008:
		TranslateGroup0x08(unElementName ,  ulValueLength ,  psValue) ;
		break;
	case 0x0009:
        TranslateGroup0x09(unElementName ,  ulValueLength ,  psValue) ;
		break;
 	case 0x0010:
		TranslateGroup0x10(unElementName ,  ulValueLength ,  psValue) ;
		break;
    case 0x0018:
        TranslateGroup0x18(unElementName ,  ulValueLength ,  psValue) ;
		break;
	case 0x0019:
        TranslateGroup0x19(unElementName ,  ulValueLength ,  psValue) ;
		break;
	case 0x0020:
        TranslateGroup0x20(unElementName ,  ulValueLength ,  psValue) ;
		break;
	case 0x0021:
        TranslateGroup0x21(unElementName ,  ulValueLength ,  psValue) ;
		break;
	case 0x0028:
        TranslateGroup0x28(unElementName ,  ulValueLength ,  psValue) ;
		break;
	case 0x0033:
        TranslateGroup0x33(unElementName ,  ulValueLength ,  psValue) ;
		break;
	}
}

void CDICOMFile::IgnoreGroup(int unGroupID, int unElementName, char *psValue, FILE* pFile)
{
//	ASSERT (unElementName == 0x0000);
	unsigned long temp=atoi(psValue);
//	pFile->Seek(temp, CFile::current);
  fseek(pFile, temp, SEEK_CUR);
}

void CDICOMFile::ReadStringValue(T_DATAELEMENT *pElement, FILE *pFile)
{
	unsigned short temp;
	switch (pElement->unGroupID)
	{
	case 0x0028:
		switch (pElement->unElementName)
		{
		case 0x0010:
		case 0x0011:
		case 0x0100:
		case 0x0101:
		case 0x0102:
		case 0x0103:
//			pFile->Read(&temp, sizeof(unsigned short));
      fread(&temp, sizeof(unsigned short), 1, pFile);
      if (m_Swap)
      {
        Swap2(&temp);
      }
			sprintf(pElement->sValue, "%d", temp);
			break;
		default:
//			pFile->Read(pElement->sValue, pElement->ulValueLength);
      fread(pElement->sValue, pElement->ulValueLength, 1, pFile);
			pElement->sValue[pElement->ulValueLength]='\0';
			break;
		}
		break;
	default:
//		pFile->Read(pElement->sValue, pElement->ulValueLength);
    fread(pElement->sValue, pElement->ulValueLength, 1, pFile);
		pElement->sValue[pElement->ulValueLength]='\0';
		break;
	}	
}

void CDICOMFile::TranslateGroup0x08(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::TranslateGroup0x09(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::TranslateGroup0x10(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::TranslateGroup0x18(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::TranslateGroup0x19(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::TranslateGroup0x20(int unElementName, int ulValueLength, char* psValue)
{
  switch(unElementName)
	{
  case 0x32:
    sscanf(psValue, "%f\\%f\\%f", &m_Position[0], &m_Position[1], &m_Position[2]);
    break;
	case 0x1041:
    m_SliceLocation = atof(psValue);
    break;
  }
}

void CDICOMFile::TranslateGroup0x21(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::TranslateGroup0x28(int unElementName, int ulValueLength, char* psValue)
{
  switch(unElementName)
	{
	case 0x0010:
    m_CX = atoi(psValue);    
		break;
	case 0x0011:
    m_CY = atoi(psValue);
		break;
  case 0x0030:
    sscanf(psValue, "%f\\%f", &(m_Spacing[0]), &(m_Spacing[1]));
    break;
	case 0x1050:
    m_WinCenter = atoi(psValue);
		break;
	case 0x1051:
    m_WinWidth = atoi(psValue);
		break;
	}
}

void CDICOMFile::TranslateGroup0x33(int unElementName, int ulValueLength, char* psValue)
{

}

void CDICOMFile::NewBuffer()
{
	m_pImage=new short[m_CX*m_CY];
}

void CDICOMFile::DeleteBuffer()
{
	if (m_pImage)
		delete m_pImage;
	m_pImage=NULL;
}

void CDICOMFile::Swap2(void* val)
{
  char* p = (char*)val, temp;
  
  temp = p[0];
  p[0] = p[1];
  p[1] = temp;  
}

void CDICOMFile::SetSwap(bool b)
{
  m_Swap = b;
}

void CDICOMFile::SwapImage()
{
  int i;
  char* p = (char*)m_pImage, temp;
  for (i = 0; i < m_CX * m_CY * 2; i += 2)
  {
    temp = p[i];
    p[i] = p[i + 1];
    p[i + 1] = temp;
  }
}