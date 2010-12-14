/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    TransformWriterExample.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPrecomputedTransformData.h"
#include "igstkTransformFileWriter.h"
#include "igstkRigidTransformXMLFileWriter.h"

#include "itksys/SystemTools.hxx"

/**
 * This program writes an xml file with the given transformation data.
 *
 */
int main(int , char *[])
{
  std::string OUTPUT_FILE_NAME("example.xml");
  igstk::PrecomputedTransformData::Pointer transformationData = 
  igstk::PrecomputedTransformData::New();

    //rigid transformation
  igstk::Transform::VersorType r;
  igstk::Transform::VectorType t;
  r.Set(0.7606, 0.1850, 0.4858, 0.3890);
  t[0] = 2; t[1] = 3; t[2] = 5;
  
  double estimationError = 0.2;
  
  igstk::Transform registrationResult;
  registrationResult.SetTranslationAndRotation( t, r, estimationError,
    igstk::TimeStamp::GetLongestPossibleTime() );
  std::string description;
  description = "MR/CT registration"; 

  std::string estimationDate = 
    itksys::SystemTools::GetCurrentDateTime( "%Y %b %d %H:%M:%S" );

  transformationData->RequestInitialize( &registrationResult, estimationDate,
                                         description, estimationError );
  
  igstk::TransformFileWriter::Pointer transformFileWriter = 
    igstk::TransformFileWriter::New();

  igstk::TransformXMLFileWriterBase::Pointer xmlFileWriter;
  xmlFileWriter = igstk::RigidTransformXMLFileWriter::New();

  transformFileWriter->RequestSetWriter( xmlFileWriter );
  transformFileWriter->RequestSetData( transformationData,  
                                       OUTPUT_FILE_NAME );

  transformFileWriter->RequestWrite();
}
