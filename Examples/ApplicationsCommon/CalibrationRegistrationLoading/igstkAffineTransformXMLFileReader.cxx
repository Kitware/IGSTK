/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAffineTransformXMLFileReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkAffineTransformXMLFileReader.h"

#include "igstkAffineTransform.h"

namespace igstk
{

void 
AffineTransformXMLFileReader::ProcessTransformation() 
  throw (FileFormatException )
{
  igstk::AffineTransform::MatrixType A;
  igstk::AffineTransform::OffsetType t;
  
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>A(0,0)>>A(0,1)>>A(0,2)>>t[0];
  instr>>A(1,0)>>A(1,1)>>A(1,2)>>t[1];
  instr>>A(2,0)>>A(2,1)>>A(2,2)>>t[2];

  //check that we got to the end of the stream, (assumes that the string 
  //m_CurrentTagData has no trailing white spaces)
  if( !instr.eof() )
    {
    throw FileFormatException( 
      "Error in transformation data, possibly non numeric values" );
    }
  //check that we got all 12 values
  if( instr.fail() )
    {
    throw FileFormatException( "Missing transformation data"); 
    }
  delete this->m_Transform;
  AffineTransform *affineTransform = new AffineTransform();

  affineTransform->SetMatrixAndOffset( 
    A, t, 
    this->m_EstimationError, 
    itk::NumericTraits< TimeStamp::TimePeriodType >::max() );
  this->m_Transform = affineTransform;
}

} //namespace
