/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionSpatialObjectTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the 
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkFLTKWidget.h"
#include "igstkView2D.h"

#include "igstkToolProjectionSpatialObject.h"
#include "igstkToolProjectionObjectRepresentation.h"

#include "igstkImageResliceObjectRepresentation.h"
#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkAxesObject.h"
#include "igstkEvents.h"
#include "igstkTransform.h"
#include "igstkVTKLoggerOutput.h"

namespace ToolProjectionSpatialObjectTest
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}


int igstkToolProjectionSpatialObjectTest( int argc, char * argv[] )
{
  if( argc < 2 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " << std::endl; 
    return EXIT_FAILURE;
    }

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject<PixelType,Dimension> ImageSpatialObjectType;

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput = 
                                            igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  //vtkLoggerOutput->SetLogger(logger);// redirect messages from VTK 
                                     // OutputWindow -> logger

  // Create Axes object to act as a reference coordinate system
  typedef igstk::AxesObject    AxesObjectType;
  AxesObjectType::Pointer worldReference = AxesObjectType::New();
  worldReference->SetSize(3.0, 3.0, 3.0);

  // Define identity transform
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  //reader->SetLogger( logger );

  //set up CT image observer
  typedef ToolProjectionSpatialObjectTest::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New(); 
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(),
                            ctImageObserver);

  /* Read in a DICOM series */
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );
  reader->RequestReadImage();
  reader->RequestGetImage();
 
  if(!ctImageObserver->GotCTImage())
    {
    std::cout << "No CTImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  // Set input image spatial object
  ImageSpatialObjectType::Pointer imageSpatialObject = ImageSpatialObjectType::New(); 
  imageSpatialObject = ctImageObserver->GetCTImage();

  //Connect the image spatial object to the reference coordinate system
  imageSpatialObject->RequestSetTransformAndParent( identity, worldReference );  
 
  //Determine the image parameters
  //first access the VTK image data
  typedef ToolProjectionSpatialObjectTest::VTKImageObserver 
                                                        VTKImageObserverType;
  
  VTKImageObserverType::Pointer vtkImageObserver = VTKImageObserverType::New();

  imageSpatialObject->AddObserver( ::igstk::VTKImageModifiedEvent(), 
                                     vtkImageObserver );
  vtkImageObserver->Reset();
  imageSpatialObject->RequestGetVTKImage();

  if( !vtkImageObserver->GotVTKImage() ) 
    {
    std::cout << "No VTKImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  vtkImageData * imageData = vtkImageObserver->GetVTKImage();

  int imageExtent[6];
  imageData->GetWholeExtent( imageExtent );
  std::cout << "Image extent: " << "(" << imageExtent[0] << "," 
                                << imageExtent[1] << ","
                                << imageExtent[2] << ","
                                << imageExtent[3] << ","
                                << imageExtent[4] << ","
                                << imageExtent[5] << ")" << std::endl;

  //Connect the image spatial object to the reference coordinate system
  imageSpatialObject->RequestSetTransformAndParent( identity, worldReference );  
 
  // Set a 2D View
  typedef igstk::View2D  View2DType;
  View2DType::Pointer view2D = View2DType::New();

  Fl_Window * form = new Fl_Window(512,512,"igstkImageResliceObjectRepresentationFltkTest2");

  typedef igstk::FLTKWidget      FLTKWidgetType;

   // instantiate FLTK widget 
  FLTKWidgetType * fltkWidget2D = 
                      new FLTKWidgetType(0,0,512,512,"2D View");

  fltkWidget2D->RequestSetView( view2D );

  view2D->RequestSetTransformAndParent( identity, worldReference );
  view2D->SetRefreshRate( 10 );

  form->end();
  form->show();

  typedef igstk::ToolProjectionSpatialObject                ObjectType;
  typedef igstk::ToolProjectionObjectRepresentation  RepresentationType;

 // Set image representation 
 typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageRepresentationType;

  ImageRepresentationType::Pointer  imageRepresentation =  ImageRepresentationType::New(); 
  imageRepresentation->SetWindowLevel( 1559, -244 );
  imageRepresentation->RequestSetImageSpatialObject( imageSpatialObject );

 // build a tool spatial object using a cylinder object
  typedef igstk::CylinderObject                           ToolSpatialObjectType;
  ToolSpatialObjectType::Pointer toolSpatialObject = ToolSpatialObjectType::New();  
  toolSpatialObject->SetRadius( 2.0 );
  toolSpatialObject->SetHeight( 150 );

  // place the tool on one side of the image in the axial direction
  igstk::Transform toolTransform;
  igstk::Transform::VectorType    translation;
  igstk::Transform::VersorType    rotation;
  
 // a variable to hold image index
  typedef ImageSpatialObjectType::IndexType IndexType;
  typedef ImageSpatialObjectType::PointType PointType;
  typedef ImageSpatialObjectType::IndexType::IndexValueType   IndexValueType;  

  // a variable to hold world point coords and indexes
  PointType point;
  IndexType index;

  // auxiliar
  const double *data = NULL;

  // initialize the tool transform in the middle of the image
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();

  translation[0] =  data[0];
  translation[1] =  data[1];
  translation[2] =  data[2];

  const double transformUncertainty = 1.0;

  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );


  // set transform and parent to the tool spatial object
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

 //Set the reslicing plane
 typedef ::igstk::ReslicerPlaneSpatialObject               ReslicerPlaneType;
 ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
//  reslicerPlaneSpatialObject->SetLogger( logger );

  // Select Orthogonal reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );

  // Select axial orientation
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  // Set bounding box provider spatial object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  // set the tool spatial object to the reslicer plane
  reslicerPlaneSpatialObject->RequestSetToolSpatialObject( toolSpatialObject );

  // Set the reslicer plane spatial object to the image representation
  imageRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  // Set the tool projection object
  ObjectType::Pointer toolProjectionObject = ObjectType::New();
  toolProjectionObject->SetSize( toolSpatialObject->GetHeight() );
  toolProjectionObject->RequestSetTransformAndParent( identity, worldReference );

  RepresentationType::Pointer toolProjectionObjectRepresentation = RepresentationType::New();
  toolProjectionObjectRepresentation->SetLogger ( logger );
  // Set the reslicer plane spatial object to the tool projection
  toolProjectionObjectRepresentation->RequestSetToolProjectionObject( toolProjectionObject );
  toolProjectionObjectRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject);
  toolProjectionObjectRepresentation->SetVisibility(true);
  toolProjectionObjectRepresentation->SetLineWidth( 2.0 );
  toolProjectionObjectRepresentation->SetColor( 1, 1, 0 );

  // add the image representation to the view
  view2D->RequestAddObject( imageRepresentation );

  // add the tool projection representation to the view
  view2D->RequestAddObject( toolProjectionObjectRepresentation );

  form->show();

   /* Change slice orientation to sagittal */
  std::cout << "Axial view: " << std::endl;
//  view2D->RequestStop();
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
  view2D->RequestSetOrientation( View2DType::Axial ); 

  view2D->RequestStart();
  view2D->RequestResetCamera();

  /* Iteratively change the tool transform to reslice through the image */
  for(unsigned int i=(unsigned int)(imageExtent[4]); i<(unsigned int)(imageExtent[5]); i++)
  {
      index[2] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      const double *data = point.GetVnlVector().data_block();
      std::cout << data[0] << " " << data[1] << " " << data[2] << " axial slice # " << i << std::endl;

      translation[0] = data[0];
      translation[1] = data[1];
      translation[2] = data[2];

      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();
  }

  /* Change slice orientation to sagittal */
  std::cout << "Sagittal view: " << std::endl;
  view2D->RequestStop();
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );
  view2D->RequestSetOrientation( View2DType::Sagittal ); 

  view2D->RequestStart();
  view2D->RequestResetCamera();

  /* Iteratively change the tool transform to reslice through the image */
  for(unsigned int i=(unsigned int)(imageExtent[1]/2); i<(unsigned int)(3*imageExtent[1]/4); i++)
  {
      index[0] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      const double *data = point.GetVnlVector().data_block();
      std::cout << data[0] << " " << data[1] << " " << data[2] << " sagittal slice # " << i << std::endl;

      translation[0] = data[0];
      translation[1] = data[1];
      translation[2] = data[2];

      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();
  }

   /* Change slice orientation to sagittal */
  std::cout << "Coronal view: " << std::endl;
  view2D->RequestStop();
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );
  view2D->RequestSetOrientation( View2DType::Coronal ); 

  view2D->RequestStart();
  view2D->RequestResetCamera();

  /* Iteratively change the tool transform to reslice through the image */
  for(unsigned int i=(unsigned int)(imageExtent[3]/2); i<(unsigned int)(3*imageExtent[3]/4); i++)
  {
      index[1] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      const double *data = point.GetVnlVector().data_block();
      std::cout << data[0] << " " << data[1] << " " << data[2] << " coronal slice # " << i << std::endl;

      translation[0] = data[0];
      translation[1] = data[1];
      translation[2] = data[2];

      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();
  }

  view2D->RequestSaveScreenShot( argv[2] );
  view2D->RequestStop();

  //toolProjectionObjectRepresentation->Print( std::cout );
  //toolProjectionObject->Print( std::cout );

  toolProjectionObjectRepresentation->GetNameOfClass();
  toolProjectionObject->GetNameOfClass();

  std::cout << "[SUCCESS]" << std::endl;

  return EXIT_SUCCESS; 
}
