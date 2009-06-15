/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPerspectiveTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPerspectiveTransform.h"


namespace igstk
{

PerspectiveTransform
::PerspectiveTransform() : TransformBase()
{
}


PerspectiveTransform
::PerspectiveTransform( const PerspectiveTransform & inputTransform  ) :
TransformBase( inputTransform )
{
  this->m_ExtrinsicTransform = inputTransform.m_ExtrinsicTransform;
  this->m_IntrinsicTransform = inputTransform.m_IntrinsicTransform;
}


PerspectiveTransform
::~PerspectiveTransform()
{
}


const PerspectiveTransform &
PerspectiveTransform
::operator=( const PerspectiveTransform & inputTransform )
{
  this->m_ExtrinsicTransform = inputTransform.m_ExtrinsicTransform;
  this->m_IntrinsicTransform = inputTransform.m_IntrinsicTransform;
  this->m_Error = inputTransform.m_Error;
  this->m_TimeStamp = inputTransform.m_TimeStamp;
  return *this;
}


void 
PerspectiveTransform
::SetTransform( const ExtrinsicMatrixType &extrinsic,
                const IntrinsicMatrixType &intrinsic,
                ErrorType errorValue,
                TimePeriodType millisecondsToExpiration )
{
  this->m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
  this->m_Error = errorValue;
  this->m_ExtrinsicTransform = extrinsic;
  this->m_IntrinsicTransform = intrinsic;
}


void 
PerspectiveTransform
::ExportTransform( vtkPerspectiveTransform &outMatrix ) const
{
  //the 3x4 concatenated transformation matrix
  ExtrinsicMatrixType::InternalMatrixType T = 
    this->m_IntrinsicTransform.GetVnlMatrix()*
    this->m_ExtrinsicTransform.GetVnlMatrix();

  //constructor initializes vtkMatrix4x4 to identity
  vtkMatrix4x4 *vtkT = vtkMatrix4x4::New();
  for(unsigned int i=0; i<3; i++ )
  {
    for(unsigned int j=0; j<4; j++ )
    {
      vtkT->SetElement(i,j,T(i,j));   
    }
  }
  outMatrix.SetMatrix( vtkT  );
  vtkT->Delete();
}


void 
PerspectiveTransform
::ExportExtrinsicParameters( ::vtkMatrix4x4 & outmatrix ) const
{
  outmatrix.SetElement(3,0,0);
  outmatrix.SetElement(3,1,0);
  outmatrix.SetElement(3,2,0);
  outmatrix.SetElement(3,3,1);
 
  for(unsigned int i=0; i<3; i++ )
  {
    for(unsigned int j=0; j<4; j++ )
    {
      outmatrix.SetElement(i,j,this->m_ExtrinsicTransform(i,j));   
    }
  }
}


void 
PerspectiveTransform
::ExportIntrinsicParameters( vtkPerspectiveTransform &outMatrix ) const
{
        //constructor initializes vtkMatrix4x4 to identity
  vtkMatrix4x4 *vtkT = vtkMatrix4x4::New();

  for(unsigned int i=0; i<3; i++ )
  {
    for(unsigned int j=0; j<3; j++ )
    {
      vtkT->SetElement(i,j,this->m_IntrinsicTransform(i,j));   
    }
  }  
  outMatrix.SetMatrix( vtkT  );
  vtkT->Delete();
}


void 
PerspectiveTransform
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
PerspectiveTransform
::PrintHeader( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Perspective Transform" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
PerspectiveTransform
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
std::ostream& operator<<( std::ostream& os, const PerspectiveTransform& o )
{
  o.Print(os, 0);
  return os;
}


/** Print object information */
void PerspectiveTransform::PrintSelf( std::ostream& os, itk::Indent indent ) 
const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  this->m_TimeStamp.Print( os, indent ); // Get the right indenting.
  os << indent << this->m_Error << std::endl;
  os << indent << this->m_IntrinsicTransform << std::endl;
  os << indent << this->m_ExtrinsicTransform << std::endl;
}


} // end namespace igstk
