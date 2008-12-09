/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentationFltkTest3.cxx
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
#pragma warning ( disable : 4786 )
#endif

#include "igstkFLTKWidget.h"

#include "igstkConfigure.h"
#include "igstkImageResliceSpatialObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkCylinderObject.h"
#include "igstkTransform.h"
#include "igstkView2D.h"


namespace ImageResliceSpatialObjectRepresentationFltkTest3
{
igstkObserverObjectMacro(CTImage,
    ::igstk::CTImageReader::ImageModifiedEvent,::igstk::CTImageSpatialObject)

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                       ::igstk::EventHelperType::VTKImagePointerType );
}




/** This test demonstrates how to perform oblique reslicing using a tool
 * spatial object ( automatic reslicing ) */
int igstkImageResliceSpatialObjectRepresentationFltkTest3( int argc , char * argv [] )
{
   igstk::RealTimeClock::Initialize();

  if( argc < 3 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " \
              << "Output image file for a screenshot" << std::endl; 
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
  vtkLoggerOutput->SetLogger(logger);// redirect messages from VTK 
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
  reader->SetLogger( logger );

  //set up CT image observer
  typedef ImageResliceSpatialObjectRepresentationFltkTest3::CTImageObserver 
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

  //Determine the image parameters
  //first access the VTK image data
  typedef ImageResliceSpatialObjectRepresentationFltkTest3::VTKImageObserver 
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

  /* Compute image bounds */
  double imageSpacing[3];
  imageData->GetSpacing( imageSpacing );
  std::cout << "Image spacing: " << "(" << imageSpacing[0] << "," 
                                 << imageSpacing[1] << ","
                                 << imageSpacing[2] << ")" << std::endl;
  double imageOrigin[3];
  imageData->GetOrigin( imageOrigin );
  std::cout << "Image origin: " << "(" << imageOrigin[0] << "," 
                                 << imageOrigin[1] << ","
                                 << imageOrigin[2] << ")" << std::endl;

  int imageExtent[6];
  imageData->GetWholeExtent( imageExtent );
  std::cout << "Image extent: " << "(" << imageExtent[0] << "," 
                                << imageExtent[1] << ","
                                << imageExtent[2] << ","
                                << imageExtent[3] << ","
                                << imageExtent[4] << ","
                                << imageExtent[5] << ")" << std::endl;

  double bounds[] = { imageOrigin[0] + imageSpacing[0]*imageExtent[0], //xmin
                       imageOrigin[0] + imageSpacing[0]*imageExtent[1], //xmax
                       imageOrigin[1] + imageSpacing[1]*imageExtent[2], //ymin
                       imageOrigin[1] + imageSpacing[1]*imageExtent[3], //ymax
                       imageOrigin[2] + imageSpacing[2]*imageExtent[4], //zmin
                       imageOrigin[2] + imageSpacing[2]*imageExtent[5]};//zmax

  for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
      {
      if ( bounds[i] > bounds[i+1] )
        {
        double t = bounds[i+1];
        bounds[i+1] = bounds[i];
        bounds[i] = t;
        }
      }

  std::cout << "Image bounds: " << "(" << bounds[0] << "," 
                                << bounds[1] << ","
                                << bounds[2] << ","
                                << bounds[3] << ","
                                << bounds[4] << ","
                                << bounds[5] << ")" << std::endl;

  //Connect the image spatial object to the reference coordinate system
  imageSpatialObject->RequestSetTransformAndParent( identity, worldReference );  
  
  typedef igstk::View2D          View2DType;
  typedef igstk::FLTKWidget      FLTKWidgetType;

  View2DType::Pointer view2D = View2DType::New();
  view2D->SetLogger( logger );
       
  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(512,512,"ImageResliceSpatialObjectRepresentationFltkTest");
    
  // instantiate FLTK widget 
  FLTKWidgetType * fltkWidget2D = 
                      new FLTKWidgetType(0,0,512,512,"2D View");

  fltkWidget2D->RequestSetView( view2D );
//  fltkWidget2D->SetLogger( logger );

  view2D->RequestSetTransformAndParent( identity, worldReference );
  view2D->SetRefreshRate( 10 );

  form->end();
  form->show();
  
  typedef igstk::ImageResliceSpatialObjectRepresentation< ImageSpatialObjectType >
                                        ImageRepresentationType; 

  ImageRepresentationType::Pointer  imageRepresentation =  ImageRepresentationType::New(); 
  imageRepresentation->SetLogger( logger );
  imageRepresentation->SetWindowLevel( 1559, -244 );
  imageRepresentation->RequestSetImageSpatialObject( imageSpatialObject );

  // Instantiate and use a reslicer plane spatial object
  typedef igstk::ReslicerPlaneSpatialObject            ReslicerPlaneType;

  ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
//  reslicerPlaneSpatialObject->SetLogger( logger );

  // Select Oblique reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Oblique );

  // Select Axial orientation type
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithXAxesNormal );

  // Set bounding box provider spatial object to the reslicer plane object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  // Set the reslicer plane spatial object to the image representation
  imageRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  // set the view as child of the reslicer plane spatial object. This allows to
  // have the view's camera facing the reslicer plane

  view2D->RequestDetachFromParent();
  view2D->RequestSetTransformAndParent( identity, reslicerPlaneSpatialObject );

  // add the image representation to the view
  view2D->RequestAddObject( imageRepresentation );

  // a variable to hold image index
  ImageSpatialObjectType::IndexType index;

  // a variable to hold world point coords
  ImageSpatialObjectType::PointType point;

  // auxiliar
  const double *data = NULL;

  // build a tool spatial object using a cylinder object
  typedef igstk::CylinderObject                           ToolSpatialObjectType;
  ToolSpatialObjectType::Pointer toolSpatialObject = ToolSpatialObjectType::New();  
  toolSpatialObject->SetRadius( 0.1 );
  toolSpatialObject->SetHeight( 2.0 );

  // set the tool spatial object to the reslicer plane
  reslicerPlaneSpatialObject->RequestSetToolSpatialObject( toolSpatialObject );

  /* Change slice orientation to PlaneOrientationWithXAxesNormal */
  //view2D->RequestSetOrientation( View2DType::Axial );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithXAxesNormal );

  /* Change slice orientation to coronal */
  // position the tool on one side of the image in the axial direction
  igstk::Transform toolTransform;
  igstk::Transform::VectorType    translation;
  igstk::Transform::VersorType    rotation;
  
  index[0] = 0.5*(imageExtent[0]+imageExtent[1]);
  index[1] = 0.5*(imageExtent[2]+imageExtent[3]);
  index[2] = imageExtent[4];
  
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();

  translation[0] =  data[0];
  translation[1] =  data[1];
  translation[2] =  data[2];
  rotation.Set(0.0, 0.0, 0.0, 1.0);

  const double transformUncertainty = 1.0;
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

  // set transform and parent
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  form->show();

  // Iteratively change the tool transform to reslice from one side to the
  // middle of the image in the axial direction
  for(unsigned int i=(unsigned int)(imageExtent[4]); i<(unsigned int)(imageExtent[5]/2); i++)
      {
      index[2] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      data = point.GetVnlVector().data_block();
      reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
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

  /* Change slice orientation to PlaneOrientationWithYAxesNormal */
  std::cout << "Sagittal view: " << std::endl;
  //view2D->RequestSetOrientation( View2DType::Sagittal );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithYAxesNormal );

  // position the tool on one side of the image in the sagittal direction
  index[0] = imageExtent[0];
  index[1] = 0.5*(imageExtent[2]+imageExtent[3]);
  index[2] = 0.5*(imageExtent[4]+imageExtent[5]);
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();

  translation[0] = data[0];
  translation[1] = data[1];
  translation[2] = data[2];

  rotation.Set(0.0, 0.0, 0.0, 1.0);
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

  view2D->RequestStart();
  view2D->RequestResetCamera();
  form->show();

  // Iteratively change the tool transform to reslice from one side to the
  // middle of the image in the sagittal direction
  for(unsigned int i=(unsigned int)(imageExtent[0]); i<(unsigned int)(imageExtent[1]/2); i++)
      {
      index[0] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      const double *data = point.GetVnlVector().data_block();
      reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
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
  view2D->RequestStop();

  /* Change slice orientation to PlaneOrientationWithZAxesNormal */
  std::cout << "Coronal view: " << std::endl;
  //view2D->RequestSetOrientation( View2DType::Coronal );
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithZAxesNormal );

  // position the tool on one side of the image in the coronal direction
  index[0] = 0.5*(imageExtent[0]+imageExtent[1]);
  index[1] = imageExtent[2];
  index[2] = 0.5*(imageExtent[4]+imageExtent[5]);
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  data = point.GetVnlVector().data_block();
  reslicerPlaneSpatialObject->RequestSetCursorPosition( data );

  translation[0] = data[0];
  translation[1] = data[1];
  translation[2] = data[2];

  rotation.Set(0.0, 0.0, 0.0, 1.0);
  toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetZeroValue() );

  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

  view2D->RequestStart();
  view2D->RequestResetCamera();

  form->show();
  // Iteratively change the tool transform to reslice from one side to the
  // middle of the image in the coronal direction
  for(unsigned int i=(unsigned int)(imageExtent[2]); i<(unsigned int)(imageExtent[3]/2); i++)
      {
      index[1] = i;
      imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
      const double *data = point.GetVnlVector().data_block();
      reslicerPlaneSpatialObject->RequestSetCursorPosition( data );
      std::cout << data[0] << " " << data[1] << " " << data[2] << " coronal slice # " << i << std::endl;

      translation[0] = data[0];
      translation[1] = data[1];
      translation[2] = data[2];
      toolTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          igstk::TimeStamp::GetZeroValue() );
      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );     

      Fl::wait( 0.01 );
      igstk::PulseGenerator::CheckTimeouts();
      }

  //Request refreshing stop to take a screenshot
  view2D->RequestStop();
  std::string filename;
  filename = argv[2]; 
  std::cout << "Saving a screen shot in file:" << argv[2] << std::endl;
  view2D->RequestSaveScreenShot( filename );

  delete fltkWidget2D;
  delete form;

 if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Got VTK Logger errors!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  std::cout << "[SUCCESS]" << std::endl;
 
  return EXIT_SUCCESS;
}

