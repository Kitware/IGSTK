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
  this->m_TransformToParent.SetToIdentity( TimeStamp::GetLongestPossibleTime() );
  this->m_TransformToRoot.SetToIdentity( TimeStamp::GetLongestPossibleTime() );
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


CoordinateReferenceSystem::IdentifierType 
CoordinateReferenceSystem
::GetRootIdentifier() const
{
  if( this->m_Parent == this )
    {
    return m_Token.GetIdentifier();
    }
  else
    {
    return this->m_Parent->GetRootIdentifier();
    }
}


void 
CoordinateReferenceSystem
::SetTransformAndParent( const Transform & transform, const Self * parent )
{
  if( parent == NULL || parent == this )
    {
    this->m_TransformToParent.SetToIdentity( TimeStamp::GetLongestPossibleTime() );
    this->m_TransformToRoot.SetToIdentity( TimeStamp::GetLongestPossibleTime() );
    }
  else
    {
    this->m_Parent = parent;
    this->m_TransformToParent = transform;
    }
}


const Transform & 
CoordinateReferenceSystem
::ComputeTransformToRoot() const
{
  if( this->m_Parent == this )
    {
    this->m_TransformToRoot.SetToIdentity( TimeStamp::GetLongestPossibleTime() );
    }
  else
    {
    this->m_TransformToRoot = Transform::TransformCompose( 
      this->m_Parent->ComputeTransformToRoot(),
      this->m_TransformToParent );
    }

 igstkLogMacro( DEBUG, " CoordinateReferenceSystem::ComputeTransformToRoot() " 
   << this->m_TransformToRoot << "\n" );

  return this->m_TransformToRoot;
}


/** Print object information */
void CoordinateReferenceSystem::PrintSelf( 
  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Parent = " << this->m_Parent.GetPointer() << std::endl;
  os << indent << "Identifier = " << this->m_Token.GetIdentifier() << std::endl;
  os << indent << "Transform to parent = " << this->m_TransformToParent << std::endl;
  os << indent << "Transform to root   = " << this->m_TransformToRoot << std::endl;
}


} // end namespace igstk
