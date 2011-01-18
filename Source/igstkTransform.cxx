/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTransform.h"


namespace igstk
{

Transform
::Transform() : TransformBase()
{
  m_Translation.Fill(0.0);
  m_Rotation.SetIdentity();
}

Transform
::Transform( const Transform & inputTransform  ) : 
TransformBase( inputTransform )
{
  m_Translation  = inputTransform.m_Translation;
  m_Rotation     = inputTransform.m_Rotation;
}


Transform
::~Transform()
{
}

Transform 
Transform
::TransformCompose( Transform leftTransform, Transform rightTransform )
{
  VersorType rotation;
  VectorType translation;

  // start with rightTransform
  rotation = rightTransform.GetRotation();
  translation = rightTransform.GetTranslation();

  // left multiple by the leftTransform
  rotation = leftTransform.GetRotation()*rotation;
  translation = leftTransform.GetRotation().Transform(translation);
  translation += leftTransform.GetTranslation();

  // Add the error value together
  TransformBase::ErrorType  error = leftTransform.GetError() + 
                                    rightTransform.GetError();

  Transform transform;
  transform.SetTranslationAndRotation( translation, rotation, error, 0);

  // Compute the overlap of two time stamps
  transform.m_TimeStamp = TimeStamp::ComputeOverlap( leftTransform.m_TimeStamp, 
                                                   rightTransform.m_TimeStamp );

  return transform;
}

const Transform &
Transform
::operator=( const Transform & inputTransform )
{
  m_Translation = inputTransform.m_Translation; 
  m_Rotation    = inputTransform.m_Rotation;
  m_Error       = inputTransform.m_Error;
  m_TimeStamp   = inputTransform.m_TimeStamp;
  return *this;
}


void 
Transform
::SetTranslationAndRotation(
          const  VectorType & translation,
          const  VersorType & rotation,
          TransformBase::ErrorType errorValue,
          TimeStamp::TimePeriodType millisecondsToExpiration)
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
  m_Rotation    = rotation;
  m_Error       = errorValue;
}


void 
Transform
::SetTranslation(
          const  VectorType & translation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration )
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation = translation;
  m_Error       = errorValue;
  m_Rotation.SetIdentity();
}


void 
Transform
::SetRotation(
          const  VersorType & rotation,
                 ErrorType errorValue,
          TimePeriodType millisecondsToExpiration )
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Rotation = rotation;
  m_Error    = errorValue;
  m_Translation.Fill( 0.0 );
}


void 
Transform
::ExportTransform( ::vtkMatrix4x4 & outmatrix ) const
{
  outmatrix.SetElement(3,0,0);
  outmatrix.SetElement(3,1,0);
  outmatrix.SetElement(3,2,0);
  outmatrix.SetElement(3,3,1);

  VersorType::MatrixType inmatrix = m_Rotation.GetMatrix();
 
  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      outmatrix.SetElement(i,j,inmatrix.GetVnlMatrix().get(i,j));   
      }

    outmatrix.SetElement(i,3,m_Translation[i]);
    }
}

void 
Transform
::ImportTransform( ::vtkMatrix4x4 & inmatrix )
{
  igstk::Transform::VersorType::MatrixType matrix = m_Rotation.GetMatrix();
 
  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      matrix.GetVnlMatrix().put(i,j,inmatrix.GetElement(i,j));   
      }
    }

  m_Rotation.Set( matrix );

  m_Translation[0] = inmatrix.GetElement(0,3);
  m_Translation[1] = inmatrix.GetElement(1,3);
  m_Translation[2] = inmatrix.GetElement(2,3);
}


bool 
Transform
::IsNumericallyEquivalent( const Transform& inputTransform, double tol ) const
{
  Transform shouldBeIdentity = TransformCompose( inputTransform, 
                                                 this->GetInverse() );
  bool isEquivalent = shouldBeIdentity.IsIdentity( tol );
  return isEquivalent;
}


bool 
Transform
::IsIdentity( double tol ) const
{
  bool isIdentity = true;

  // If the cosinus of half the angle is not 1.0 (up to a tolerance) 
  // then this Transform is not an identity.
  if( vnl_math_abs( this->m_Rotation.GetW() - 1.0 ) > tol )
    {
    isIdentity = false;
    }

  // If the norm of the translation vector is not 0.0 (up to a tolerance)
  // then this Transform is not an identity.
  if( vnl_math_abs( this->m_Translation.GetNorm() ) > tol )
    {
    isIdentity = false;
    }

  return isIdentity;
}


bool
Transform
::operator==( const Transform & inputTransform )
{
  // 
  // FIXME Discuss how the == operator should be defined for the Transform.
  // 
  // In the meantime, let's go safe and consider two transforms equal
  // only if they are the same object in memory. It is arguable whether this
  // should be a method based on how close the numerical content of the two
  // transforms is, based on some tolerance.
  return (this==&inputTransform);
}


bool
Transform
::operator!=( const Transform & inputTransform )
{
  return !(this->operator==( inputTransform ));
}


void
Transform
::SetToIdentity( TimePeriodType millisecondsToExpiration )
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Translation.Fill( 0.0 );
  m_Rotation.SetIdentity();
  m_Error = itk::NumericTraits< ErrorType >::min();
}


Transform
Transform
::GetInverse() const
{
  Transform inverse;
  inverse.m_TimeStamp = this->m_TimeStamp;
  inverse.m_Rotation = this->m_Rotation.GetConjugate();
  inverse.m_Translation = 
                      inverse.m_Rotation.Transform( -(this->m_Translation) );
  inverse.m_Error = this->m_Error;
  return inverse;
}


void 
Transform
::Print( std::ostream& os, itk::Indent indent ) const
{
  this->PrintHeader( os, indent ); 
  this->PrintSelf( os, indent.GetNextIndent() );
  this->PrintTrailer( os, indent );
}


/**
 * Define a default print header for all objects.
 */
void 
Transform
::PrintHeader( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Transform" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
Transform
::PrintTrailer( std::ostream& itkNotUsed(os), 
                itk::Indent itkNotUsed(indent) ) const
{
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<( std::ostream& os, const Transform& o )
{
  o.Print(os, 0);
  return os;
}


/** Print object information */
void Transform::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  this->m_TimeStamp.Print( os, indent ); // Get the right indenting.
  os << indent << this->m_Translation << std::endl;
  os << indent << this->m_Rotation << std::endl;
  os << indent << this->m_Error << std::endl;
}


} // end namespace igstk
