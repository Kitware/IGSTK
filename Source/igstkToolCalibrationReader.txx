/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibrationReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkToolCalibrationReader_txx
#define __igstkToolCalibrationReader_txx

#include "igstkToolCalibrationReader.h"
#include "zlib/zlib.h"

namespace igstk
{ 
/** Constructor */
template <class TCalibration>
ToolCalibrationReader<TCalibration>
::ToolCalibrationReader() : m_StateMachine(this)
{
  m_Calibration = CalibrationType::New();
  m_FileName = "";
} 

/** Destructor */
template <class TCalibration>
ToolCalibrationReader<TCalibration>
::~ToolCalibrationReader()  
{
}

/** Add a parameter */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::AddParameter(const char* name,float value)
{
  std::vector<ParameterType>::const_iterator it = m_Parameters.begin();
  while(it != m_Parameters.end())
    {
    if((*it).first == name)
      {
      // we have a duplicate name
      return false;
      }
    it++;
    }

  ParameterType param;
  param.first = name;
  param.second = value;
  m_Parameters.push_back(param);
  return true;
}
 
/** Get a parameter given its name */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::GetParameter(const char* name,float * value)
{
  std::vector<ParameterType>::const_iterator it = m_Parameters.begin();
  while(it != m_Parameters.end())
    {
    if((*it).first == name)
      {
      *value = (*it).second;
      return true;
      }
    it++;
    }
  return false;
}


/** Get an error given its name */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::GetError(const char* name,float * value)
{
  std::vector<ErrorType>::const_iterator it = m_Errors.begin();
  while(it != m_Errors.end())
    {
    if((*it).first == name)
      {
      *value = (*it).second;
      return true;
      }
    it++;
    }
  return false;
}

/** Return the transform */
template <class TCalibration>
const typename ToolCalibrationReader<TCalibration>::TransformType &
ToolCalibrationReader<TCalibration>
::GetTransform() const
{
  return m_Transform;
}

/** Add an error */
template <class TCalibration>
bool ToolCalibrationReader<TCalibration>
::AddError(const char* name,float value)
{
  std::vector<ErrorType>::const_iterator it = m_Errors.begin();
  while(it != m_Errors.end())
    {
    if((*it).first == name)
      {
      // we have a duplicate name
      return false;
      }
    it++;
    }

  ErrorType error;
  error.first = name;
  error.second = value;
  m_Errors.push_back(error);
  return true;
}

/** Read the spatialobject file */
template <class TCalibration>
bool 
ToolCalibrationReader<TCalibration>
::ParseXML()
{ 
  m_Parameters.clear();
  m_Errors.clear();

  // Check the crc32
  // Read the file
  std::ifstream file;
  file.open(m_FileName.c_str(), std::ios::binary | std::ios::in);
  if(!file.is_open())
    {
    std::cout << "Cannot open file: " << m_FileName.c_str() << std::endl;
    return false;
    }
  file.seekg(0,std::ios::end);
  unsigned long fileSize = file.tellg();
  file.seekg(0,std::ios::beg);

  char* buf = new char[fileSize+1];
  file.read(buf,fileSize);
  buf[fileSize] = 0; 
  std::string buffer(buf);
  buffer.resize(fileSize);
  delete [] buf;
  file.close();

  // Find the <CRC32> tag
  long int crcpos = buffer.find("<CRC32>");
  if(crcpos == -1)
    {
    std::cout << "No CRC32 tag found in the file!" << std::endl;
    return false;
    }
 
  buffer.resize(crcpos);

  unsigned long crc = crc32(0L,(Bytef*)buffer.c_str(),crcpos);

  igstkLogMacro( DEBUG, "igstk::ToolCalibrationReader::ParseXML called...\n");
  XMLToolCalibrationReader::Pointer reader = XMLToolCalibrationReader::New();
  reader->SetFilename(m_FileName.c_str());
  if(!reader->CanReadFile(m_FileName.c_str()))
    {
    return false;
    }

  std::vector<XMLToolCalibrationReader::tag> tags = reader->GetTags();
  std::vector<XMLToolCalibrationReader::tag>::const_iterator it = tags.begin();
  while(it != tags.end())
    {
    // Creation block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Creation"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"date"))
          {
          m_Calibration->RequestSetDate((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"time"))
          {
          m_Calibration->RequestSetTime((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"method"))
          {
          if(itksys::SystemTools::Strucmp((*it2).second.c_str(),
                                  m_Calibration->GetNameOfClass()))
            {
            return false;
            }
          }  
        it2++;
        }
      }

    // Tool block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Tool"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"type"))
          {
          m_Calibration->RequestSetToolType((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"manufacturer"))
          {
          m_Calibration->RequestSetToolManufacturer((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"partNumber"))
          {
          m_Calibration->RequestSetToolPartNumber((*it2).second.c_str());
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"serialNumber"))
          {
          m_Calibration->RequestSetToolSerialNumber((*it2).second.c_str());
          }
        it2++;
        }
      }

    // Parameter block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Parameter"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
      std::string name = "";
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"name"))
          {
          name = (*it2).second;
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"value"))
          {
          if(name.size()>0)
            {
            this->AddParameter(name.c_str(),atof((*it2).second.c_str()));
            }
          }
        it2++;
        }
      }

    // Error block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"Error"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      std::vector<XMLToolCalibrationReader::PairType>::const_iterator it2 
                                                            = values.begin();
     std::string name = "";
      while(it2 != values.end())
        {
        if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"name"))
          {
          name = (*it2).second;
          }
        else if(!itksys::SystemTools::Strucmp((*it2).first.c_str(),"value"))
          {
          if(name.size()>0)
            {
            this->AddError(name.c_str(),atof((*it2).second.c_str()));
            }
          }
        it2++;
        }
      }

    // CRC32 Block
    if(!itksys::SystemTools::Strucmp((*it).name.c_str(),"CRC32"))
      {
      std::vector<XMLToolCalibrationReader::PairType> values = (*it).values;
      if(atof((*(values.begin())).first.c_str()) != crc)
        {
        std::cout << "CRC is wrong: " 
                  << (*(values.begin())).first.c_str() << " : " 
                  << atof((*(values.begin())).first.c_str())
                  << " (expected: " << crc << ")" << std::endl;
        return false;
        }
      }
    it++;
    }
  return true;
}


/** Print Self function */
template <class TCalibration>
void
ToolCalibrationReader<TCalibration>
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

template <class TCalibration>
const 
typename ToolCalibrationReader<TCalibration>::CalibrationType *
ToolCalibrationReader<TCalibration>
::GetCalibration() const
{
  return m_Calibration;
}

} // end namespace igstk

#endif
