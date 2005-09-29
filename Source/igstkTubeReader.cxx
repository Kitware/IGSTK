/*=========================================================================

  Program:   SpatialObject Guided Surgery Software Toolkit
  Module:    igstkTubeReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkTubeReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
TubeReader::TubeReader()
{ 
  m_Group = GroupObjectType::New();
} 

/** Destructor */
TubeReader::~TubeReader()  
{
}

/** Read the spatialobject file */
void TubeReader::AttemptReadObject()
{
  igstkLogMacro( DEBUG, "igstk::TubeReader::AttemptReadObject called...\n");
  Superclass::AttemptReadObject();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject = m_SpatialObjectReader->GetGroup();
  GroupSpatialObjectType::ChildrenListType * children = m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it = children->begin();

  while(it != children->end())
    {
    if(!strcmp((*it)->GetTypeName(),"TubeSpatialObject"))
      {
      // Create a new TubeObject
      TubeType::Pointer tube = TubeType::New();
      
      // Get the points from the tube spatialobject
      TubeSpatialObjectType::PointListType::const_iterator ptIt;
      ptIt = dynamic_cast<TubeSpatialObjectType*>((*it).GetPointer())->GetPoints().begin();

      while(ptIt != dynamic_cast<TubeSpatialObjectType*>((*it).GetPointer())->GetPoints().end())
        {
        TubeType::PointType pt;
        pt.SetPosition((*ptIt).GetPosition());
        pt.SetRadius((*ptIt).GetRadius());
        tube->AddPoint(pt);
        ptIt++;
        }

      // For the moment no hierarchy
      m_Group->RequestAddObject(tube);

      }
    else if(!strcmp((*it)->GetTypeName(),"VesselTubeSpatialObject"))
      {
      // Create a new TubeObject
      TubeType::Pointer tube = TubeType::New();
      
      // Get the points from the tube spatialobject
      typedef itk::VesselTubeSpatialObject<3> VesselTubeSpatialObjectType;
      VesselTubeSpatialObjectType::PointListType::const_iterator ptIt;
      ptIt = dynamic_cast<VesselTubeSpatialObjectType*>((*it).GetPointer())->GetPoints().begin();

      while(ptIt != dynamic_cast<VesselTubeSpatialObjectType*>((*it).GetPointer())->GetPoints().end())
        {
        TubeType::PointType pt;
        pt.SetPosition((*ptIt).GetPosition());
        pt.SetRadius((*ptIt).GetRadius());
        tube->AddPoint(pt);
        ptIt++;
        }

      // For the moment no hierarchy
      m_Group->RequestAddObject(tube);

      }
    it++;
    }
  delete children;
}

/** Return the output as a group */
const TubeReader::GroupObjectType *
TubeReader::GetOutput() const
{
  return m_Group;
}

/** Print Self function */
void TubeReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk


