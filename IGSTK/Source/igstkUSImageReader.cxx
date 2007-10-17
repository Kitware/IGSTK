/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkUSImageReader.h"

namespace igstk
{ 

/** Constructor */
USImageReader::USImageReader():m_StateMachine(this)
{

}

/** Destructor */
USImageReader::~USImageReader()
{
}

/** Print Self function */
void USImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Check if US dicom is being read */
bool USImageReader::CheckModalityType( DICOMInformationType modality )
{
  if( modality != "US" ) 
    {
    return false;
    }
  else
    {
    return true;
    }
}

} // end namespace igstk
