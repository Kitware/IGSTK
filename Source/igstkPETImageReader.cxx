/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPETImageReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPETImageReader.h"

namespace igstk
{ 

/** Constructor */
PETImageReader::PETImageReader():m_StateMachine(this)
{

} 

/** Check if PET dicom is being read */
bool PETImageReader::CheckModalityType( DICOMInformationType modality )
{
  if( modality != "PT" )
    {
    return false;
    }
  else
    {
    return true;
    }
}


/** Print Self function */
void PETImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
