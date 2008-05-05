/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeReader::TubeReader():m_StateMachine(this)
{ 
  m_Tube = TubeType::New();
  m_TubeSpatialObject = TubeSpatialObjectType::New();
} 

/** Destructor */
TubeReader::~TubeReader()  
{
}


// FIXME : This must be replaced with StateMachine logic
TubeReader::TubeSpatialObjectType * 
TubeReader::GetITKTubeSpatialObject() const
{
  return m_TubeSpatialObject;
}

/** Read the spatialobject file */
void TubeReader::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::TubeReader::AttemptReadObject called...\n");
  Superclass::AttemptReadObjectProcessing();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject = 
                                            m_SpatialObjectReader->GetGroup();
  GroupSpatialObjectType::ChildrenListType * children = 
                                     m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it = 
                                                            children->begin();

  while( it != children->end() )
    {
    if( !strcmp((*it)->GetTypeName(),"TubeSpatialObject") )
      {
      TubeSpatialObjectType * tube = 
              dynamic_cast< TubeSpatialObjectType * >( it->GetPointer() );
      if( tube )
        {
        m_TubeSpatialObject = tube;
        this->ConnectTube();
        break;
        }
      }
    it++;
    }
  delete children;

}

void TubeReader::ReportObjectProcessing()
{
  TubeModifiedEvent  event;
  event.Set( this->m_Tube );
  this->InvokeEvent( event );
}


void TubeReader::ConnectTube() 
{
  typedef Friends::TubeReaderToTubeSpatialObject  HelperType;
  HelperType::ConnectTube( this, m_Tube.GetPointer() );
}


/** Print Self function */
void TubeReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Tube = " << m_Tube.GetPointer() << std::endl;
  os << "TubeSpatialObject = " << m_TubeSpatialObject.GetPointer() << std::endl;
}

} // end namespace igstk
