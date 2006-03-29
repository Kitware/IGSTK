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

/** Read the spatialobject file */
template <class TCalibration>
bool 
ToolCalibrationReader<TCalibration>
::ParseXML()
{
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
