/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMRImageReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkMRImageReader.h"

namespace igstk
{ 

/** Constructor */
MRImageReader::MRImageReader():m_StateMachine(this)
{

} 

/** Check if MRI dicom is being read */
bool MRImageReader::CheckModalityType( DICOMInformationType modaltiy )
{
  if( modaltiy != "MR" )
    {
    return false;
    }
  else
    {
    return true;
    }
}


/** Print Self function */
void MRImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
