/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystem.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCoordinateReferenceSystem.h"

namespace igstk
{ 

/** Constructor */
CoordinateReferenceSystem
::CoordinateReferenceSystem():m_StateMachine(this)
{
  this->m_Parent = this;
  this->m_TransformToParent.SetToIdentity( 
                                    TimeStamp::GetLongestPossibleTime() );
} 

/** Destructor */
CoordinateReferenceSystem
::~CoordinateReferenceSystem()  
{
}

CoordinateReferenceSystem::IdentifierType 
CoordinateReferenceSystem
::GetIdentifier() const
{
  return m_Token.GetIdentifier();
}

void 
CoordinateReferenceSystem
::SetTransformAndParent( const Transform & transform, const Self * parent )
{
  if( parent == NULL || parent == this )
    {
    this->m_TransformToParent.SetToIdentity( 
                                      TimeStamp::GetLongestPossibleTime() );
    }
  else
    {
    this->m_Parent = parent;
    this->m_TransformToParent = transform;
    }
}

/** Print object information */
void CoordinateReferenceSystem::PrintSelf( 
  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Parent = " << this->m_Parent.GetPointer() << std::endl;
  os << indent << "Identifier = " 
     << this->m_Token.GetIdentifier() << std::endl;
  os << indent << "Transform to parent = " 
     << this->m_TransformToParent << std::endl;
}

CoordinateReferenceSystem::ConstPointer 
  CoordinateReferenceSystem::GetLowestCommonAncestor(
                                   CoordinateReferenceSystem::ConstPointer A,
                                   CoordinateReferenceSystem::ConstPointer B)
{
  //
  // A simple algorithm is to find a path to a root for each 
  // coordinate system and then find the earliest path intersection. We 
  // do this by walking up one path and comparing it against every other
  // node in the other path.
  // 
  typedef CoordinateReferenceSystem::ConstPointer CSPointer;

  if (A == B)
    {
    return A;
    }
  if (A.IsNull() || B.IsNull())
    {
    return NULL;
    }

  CSPointer aTemp; 
  CSPointer bTemp;
  CSPointer aTempPrev = NULL;
  CSPointer bTempPrev = NULL;

  for(bTemp = B; 
      (bTemp.IsNotNull() && bTempPrev != bTemp); 
      bTemp = bTemp->GetParent())
    {
    aTempPrev = NULL;
    for(aTemp = A; 
        (aTemp.IsNotNull() && aTempPrev != aTemp); 
        aTemp = aTemp->GetParent())
      {
      const char* aName = aTemp->GetName();
      const char* bName = bTemp->GetName();
      if (aTemp == bTemp)
        {
        return aTemp;
        }
      aTempPrev = aTemp;
      bTempPrev = bTemp;
      }
    }

  // Last resort
  return NULL;
}

//
// This method can compute the transform between two leaves.
//
Transform CoordinateReferenceSystem::GetTransformBetween(
                          CoordinateReferenceSystem::ConstPointer source,
                          CoordinateReferenceSystem::ConstPointer destination)
{
  CoordinateReferenceSystem::ConstPointer ancestor = 
                                 GetLowestCommonAncestor(source, destination);

  Transform sourceToAncestor = source->ComputeTransformTo(ancestor);
  Transform destinationToAncestor = destination->ComputeTransformTo(ancestor);

  Transform destinationToAncsetorInverse = destinationToAncestor.GetInverse();

  Transform transformBetween = Transform::TransformCompose(
                                              destinationToAncsetorInverse,
                                                          sourceToAncestor);

  return transformBetween;
}

// Check if we should return a transform smart pointer. There seems to be 
// no typedefs for transform smart pointers.
//
// This method can only traverse up the tree.
//
Transform CoordinateReferenceSystem::ComputeTransformTo(
                       CoordinateReferenceSystem::ConstPointer ancestor) const
{
  //
  // Go up the hierarchy of parents until we find the ancestor coordinate 
  // reference system.
  // We also need to guard against not finding the ancestor coordinate system 
  // and endlessly trying to walk up a node with a self loop.
  //
  if ( this->m_Parent == this && ancestor != this )
    {
    // Error condition...we've found a self loop
    // and it's not the ancestor coordinate system.
    Transform identity;
    // This transform is specifically configured to have a
    // time stamp which should never be valid.
    identity.SetToIdentity(-1.0*igstk::TimeStamp::GetLongestPossibleTime());
    return identity;
    }
  else if ( ancestor == this )
    {
    Transform identity;
    identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
    return identity;
    }
  else
    {
    // Check order of composition
    return Transform::TransformCompose( 
      this->m_TransformToParent,
      this->m_Parent->ComputeTransformTo(ancestor));
    }
}

void CoordinateReferenceSystem::SetName(const char* name)
{
  m_Name = new char[strlen(name)];
  strcpy(m_Name, name);
}


} // end namespace igstk
