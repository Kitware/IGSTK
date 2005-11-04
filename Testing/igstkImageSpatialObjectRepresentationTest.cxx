/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentationTest.cxx
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
#pragma warning ( disable : 4786 )
#endif

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkView2D.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

int igstkImageSpatialObjectRepresentationTest( int , char* [] )
{


  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  
  typedef igstk::ImageSpatialObjectRepresentation< 
                                  ImageSpatialObjectType 
                                                      >   RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  representation->SetLogger( logger );

  // Test error case
  representation->RequestSetImageSpatialObject( NULL );

  // Test correct case
  ImageSpatialObjectType::Pointer imageSpatialObject = ImageSpatialObjectType::New();
  representation->RequestSetImageSpatialObject( imageSpatialObject );

  // Exercise the TypeMacro() which defines teh GetNameOfClass()
  std::string name = representation->GetNameOfClass();

  std::cout << "Name of class = " << name << std::endl;
  
  representation->RequestSetSliceNumber( 10 );

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(532,532,"CT Read View Test");
    
  typedef igstk::View2D  View2DType;

  View2DType * view2D = new View2DType( 10,10,512,512,"2D View");

  form->end();
  form->show();
   
  // Test all possible orientations 
  representation->RequestSetOrientation( RepresentationType::Sagittal );
  representation->RequestSetOrientation( RepresentationType::Coronal );
  representation->RequestSetOrientation( RepresentationType::Axial );

  representation->RequestSetSliceNumber( 10 );

  // Do manual redraws
  for(unsigned int i=0; i<10; i++)
    {
    view2D->Update();  // schedule redraw of the view
    Fl::check();       // trigger FLTK redraws
    std::cout << "i= " << i << std::endl;
    }

  delete view2D;
  delete form;
 

  return EXIT_SUCCESS;
}

