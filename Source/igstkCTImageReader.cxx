/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCTImageReader.h"

namespace igstk
{ 

/** Constructor */
CTImageReader::CTImageReader():m_StateMachine(this)
{

} 

/** Check if CT dicom is being read */
bool CTImageReader::CheckModalityType( DICOMInformationType modality )
{
  if( modality != "CT" )
    {
    return false;
    }
  else
    {
    return true;
    }
}


/** Print Self function */
void CTImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
