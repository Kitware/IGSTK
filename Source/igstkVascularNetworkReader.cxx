/*=========================================================================

  Program:   SpatialObject Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVascularNetworkReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
VascularNetworkReader::VascularNetworkReader():m_StateMachine(this)
{ 
  m_VascularNetwork = VascularNetworkType::New();
} 

/** Destructor */
VascularNetworkReader::~VascularNetworkReader()  
{
}

/** Read the spatialobject file */
void VascularNetworkReader::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VascularNetworkReader::AttemptReadObject called...\n");
  Superclass::AttemptReadObjectProcessing();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject = m_SpatialObjectReader->GetGroup();
  GroupSpatialObjectType::ChildrenListType * children = m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it = children->begin();

  while(it != children->end())
    {
    if(!strcmp((*it)->GetTypeName(),"VesselTubeSpatialObject"))
      {
      VesselTubeSpatialObjectType * vesselTube = 
          dynamic_cast< VesselTubeSpatialObjectType * >( it->GetPointer() );
      
      if(vesselTube)
        {
        // Create a new VascularNetworkObject
        VesselObjectType::Pointer vessel = VesselObjectType::New();
        vessel->SetVesselSpatialObject(vesselTube);

        // For the moment no hierarchy
        m_VascularNetwork->RequestAddObject(vessel);
        }
      
      // Get the points from the vesseltube spatialobject
      /*typedef VesselTubeSpatialObjectType::PointListType    PointListType;
      typedef PointListType::const_iterator           PointIterator;

      PointIterator pointItr = vesselTube->GetPoints().begin();
      PointIterator pointEnd = vesselTube->GetPoints().end();

      while( pointItr != pointEnd )
        {
        VesselObjectType::PointType point;
        point.SetPosition( pointItr->GetPosition() );
        point.SetRadius( pointItr->GetRadius() );
        vessel->AddPoint( point );
        pointItr++;
        }*/
      }
    it++;
    }
  delete children;

  // Provide an identity transform with a long validity time.
 /* const double validityTimeInMilliseconds = 1e30;
  
  igstk::Transform transform;

  igstk::Transform::ErrorType errorValue = 1000; // 1 meter

  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 0;
  translation[2] = 0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  m_Group->RequestSetTransform( transform );*/
 
}

/** Return the output as a group */
const VascularNetworkReader::VascularNetworkType *
VascularNetworkReader::GetOutput() const
{
  return m_VascularNetwork;
}

/** Print Self function */
void VascularNetworkReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "VascularNetwork = " << m_VascularNetwork.GetPointer() << std::endl;
}

} // end namespace igstk


