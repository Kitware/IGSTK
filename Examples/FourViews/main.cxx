/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "FourViewsImplementation.h"
#include <itkStdStreamLogOutput.h>

int main(int , char** )
{ 
  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  igstk::FourViewsImplementation   application;
  application.Show();

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  application.SetLogger(logger);

  // Try to read a tube
  /*igstk::TubeReader::Pointer tubeReader =  igstk::TubeReader::New();
  tubeReader->SetLogger( logger );
  tubeReader->RequestSetFileName("E:/Vessel.tre");
  tubeReader->RequestReadObject();
  igstk::TubeGroupObject::ConstPointer tubeGroup = tubeReader->GetOutput();

  // Create the object representations for the tube
  for(unsigned int i=0;i<tubeGroup->GetNumberOfObjects();i++)
    {
    // Create the ellipsoid representation
    igstk::TubeObject::ConstPointer tube = tubeGroup->GetTube(i);
    igstk::TubeObjectRepresentation::Pointer tubeRepresentation = igstk::TubeObjectRepresentation::New();
    tubeRepresentation->RequestSetTubeObject( tube );
    tubeRepresentation->SetColor(0.0,1.0,0.0);
    tubeRepresentation->SetOpacity(1.0);
    application.AddTube( tubeRepresentation );
    }

  // Try to read a mesh
  igstk::MeshReader::Pointer meshReader =  igstk::MeshReader::New();
  meshReader->SetLogger( logger );
  meshReader->RequestSetFileName("E:/Liver.msh");
  meshReader->RequestReadObject();
  igstk::MeshObject::ConstPointer mesh = meshReader->GetOutput();

  // Create the object representations for the mesh
  igstk::MeshObjectRepresentation::Pointer meshRepresentation = igstk::MeshObjectRepresentation::New();
  meshRepresentation->RequestSetMeshObject( mesh );
  meshRepresentation->SetColor(1.0,0.0,0.0);
  meshRepresentation->SetOpacity(1.0);
  application.AddMesh( meshRepresentation );

  application.ResetCameras();
*/
  Fl::run();

  return EXIT_SUCCESS;
}
