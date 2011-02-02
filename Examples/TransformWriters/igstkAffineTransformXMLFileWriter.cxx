/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAffineTransformXMLFileWriter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkAffineTransformXMLFileWriter.h"

#include "igstkAffineTransform.h"

namespace igstk
{

void 
AffineTransformXMLFileWriter::WriteTransformation( std::ofstream &out )
{
  TransformRequestObserver::Pointer transformObserver = 
    TransformRequestObserver::New();
  unsigned long observerID = this->m_InputObject->AddObserver( 
    igstk::PrecomputedTransformData::TransformTypeEvent(), 
    transformObserver );
  this->m_InputObject->RequestTransform();
  this->m_InputObject->RemoveObserver( observerID );

   TransformErrorObserver::Pointer transformErrorObserver = 
      TransformErrorObserver::New();
   observerID = this->m_InputObject->AddObserver( 
      igstk::PrecomputedTransformData::TransformErrorTypeEvent(), 
      transformErrorObserver );
  this->m_InputObject->RequestEstimationError();
  this->m_InputObject->RemoveObserver( observerID );

  if( transformObserver->GotTransformRequest() && 
      transformErrorObserver->GotTransformError() )
    {
       //ten digits after the decimal point should be enough to retain accuracy
      //in ASCII format
    const std::streamsize PRECISION = 10;
    std::streamsize originalPrecision = out.precision( PRECISION );
    std::ios::fmtflags originalFlags = 
      out.setf( std::ios::fixed, std::ios::floatfield ); 

    igstk::TransformBase *transformData = 
      transformObserver->GetTransformRequest();
    const igstk::AffineTransform *affineTransformation = 
      dynamic_cast<const igstk::AffineTransform *>( transformData );
    out<<"\t <transformation estimation_error=\"";
    out<<transformErrorObserver->GetTransformError()<<"\">\n";
    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    affineTransformation->ExportTransform( *matrix );
    
    out<<"\t"<<(*matrix)[0][0]<<"\t"<<(*matrix)[0][1]<<"\t"<<(*matrix)[0][2]
       <<"\t"<< (*matrix)[0][3]<<"\n";
    out<<"\t"<<(*matrix)[1][0]<<"\t"<<(*matrix)[1][1]<<"\t"<<(*matrix)[1][2]
       <<"\t"<< (*matrix)[1][3]<<"\n";
    out<<"\t"<<(*matrix)[2][0]<<"\t"<<(*matrix)[2][1]<<"\t"<<(*matrix)[2][2]
       <<"\t"<< (*matrix)[2][3]<<"\n";
    out<<"\t</transformation>\n\n";
    matrix->Delete();
    out.precision( originalPrecision );
    out.setf( originalFlags ); 
    }
}


bool 
AffineTransformXMLFileWriter::IsCompatible( 
  const PrecomputedTransformData::Pointer transformation )
{
  TransformRequestObserver::Pointer transformObserver = 
     TransformRequestObserver::New();
  transformation->AddObserver( 
     igstk::PrecomputedTransformData::TransformTypeEvent(), 
     transformObserver );
  transformation->RequestTransform();

  if( transformObserver->GotTransformRequest() )
    {
    igstk::TransformBase *transformData =
       transformObserver->GetTransformRequest();
    return dynamic_cast<const igstk::AffineTransform *>( transformData )
       != NULL;
    }
  return false;
}

} //namespace
