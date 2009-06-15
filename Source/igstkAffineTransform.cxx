/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAffineTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAffineTransform.h"


namespace igstk
{

AffineTransform
::AffineTransform() : TransformBase()
{
  this->m_Transform = ::itk::AffineTransform<double, 3>::New();
  this->m_Transform->SetIdentity();
}


AffineTransform
::AffineTransform( const AffineTransform & inputTransform  ) :
TransformBase( inputTransform )
{
  this->m_Transform = ::itk::AffineTransform<double, 3>::New();
  this->m_Transform->SetMatrix( inputTransform.m_Transform->GetMatrix() );
  this->m_Transform->SetOffset( inputTransform.m_Transform->GetOffset() );
}


AffineTransform
::~AffineTransform()
{
}


AffineTransform 
AffineTransform
::TransformCompose( AffineTransform &leftTransform, 
                    AffineTransform &rightTransform )
{
  AffineTransform result;

  itk::AffineTransform<double,3>::Pointer itkResult =
    itk::AffineTransform<double,3>::New();
  itkResult->SetMatrix( leftTransform.m_Transform->GetMatrix() );
  itkResult->SetOffset( leftTransform.m_Transform->GetOffset() );
  itkResult->Compose( rightTransform.m_Transform );

  // error values are summed, should be a better way to
  // propagate errors
  result.SetMatrixAndOffset( itkResult->GetMatrix(),
                             itkResult->GetOffset(),
                             leftTransform.m_Error + rightTransform.m_Error,
                             0 );
  result.m_TimeStamp = TimeStamp::ComputeOverlap( leftTransform.m_TimeStamp, 
                                                  rightTransform.m_TimeStamp );
  return result;
}


const AffineTransform &
AffineTransform
::operator=( const AffineTransform & inputTransform )
{
  m_Transform->SetMatrix( inputTransform.m_Transform->GetMatrix() ); 
  m_Transform->SetOffset( inputTransform.m_Transform->GetOffset() ); 
  m_Error = inputTransform.m_Error;
  m_TimeStamp = inputTransform.m_TimeStamp;
  return *this;
}


void 
AffineTransform
::SetMatrixAndOffset( const  MatrixType &matrix,
                      const OffsetType &offset,
                      ErrorType errorValue,
                      TimePeriodType millisecondsToExpiration )
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  m_Error = errorValue;
  this->m_Transform->SetMatrix( matrix );
  this->m_Transform->SetOffset( offset );
}


void 
AffineTransform
::ExportTransform( ::vtkMatrix4x4 & outmatrix ) const
{
  outmatrix.SetElement(3,0,0);
  outmatrix.SetElement(3,1,0);
  outmatrix.SetElement(3,2,0);
  outmatrix.SetElement(3,3,1);

  const MatrixType &matrix = this->m_Transform->GetMatrix();
  const OffsetType &offset = this->m_Transform->GetOffset();
 
  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      outmatrix.SetElement( i, j, matrix(i,j) );   
      }

    outmatrix.SetElement( i, 3, offset[i] );
    }
}


void 
AffineTransform
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
AffineTransform
::PrintHeader( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Affine Transform" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
AffineTransform
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
std::ostream& operator<<( std::ostream& os, const AffineTransform& o )
{
  o.Print(os, 0);
  return os;
}


/** Print object information */
void AffineTransform::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  this->m_TimeStamp.Print( os, indent ); // Get the right indenting.
  os << indent << this->m_Error << std::endl;
  os << indent << this->m_Transform << std::endl;
}


} // end namespace igstk
