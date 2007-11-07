/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundProbeObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkUltrasoundProbeObject.h"

namespace igstk
{ 

/** Constructor */
UltrasoundProbeObject::UltrasoundProbeObject():m_StateMachine(this)
{
  m_UltrasoundProbeSpatialObject = UltrasoundProbeSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_UltrasoundProbeSpatialObject );
} 

/** Destructor */
UltrasoundProbeObject::~UltrasoundProbeObject()  
{
}

/** Print object information */
void UltrasoundProbeObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
