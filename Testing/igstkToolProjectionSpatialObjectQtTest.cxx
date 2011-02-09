/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionSpatialObjectQtTest.cxx
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

// QT header files
#include <QApplication>
#include <QMainWindow>
#include <QtTest/QTest>

#include "igstkQTWidget.h"

#include "igstkConfigure.h"
#include "igstkImageResliceObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkCTImageReader.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkCylinderObject.h"
#include "igstkTransform.h"
#include "igstkView2D.h"

#include "igstkMeshReader.h"
#include "igstkMeshObjectRepresentation.h"

#include "igstkToolProjectionSpatialObject.h"
#include "igstkToolProjectionObjectRepresentation.h"


namespace ToolProjectionSpatialObjectQtTest
{
igstkObserverObjectMacro(CTImage, ::igstk::CTImageReader::ImageModifiedEvent,
                                                ::igstk::CTImageSpatialObject);

igstkObserverMacro( VTKImage, ::igstk::VTKImageModifiedEvent, 
                               ::igstk::EventHelperType::VTKImagePointerType );

igstkObserverObjectMacro( MeshObject, ::igstk::MeshReader::MeshModifiedEvent,
                                        ::igstk::MeshObject );
}


/** This test demonstrates how to perform orthogonal reslicing using a tool
 * spatial object ( automatic reslicing ) */
int igstkToolProjectionSpatialObjectQtTest( int argc , char * argv [] )
{
  std::cout << " igstkToolProjectionSpatialObjectQtTest: application started " << std::endl;

  igstk::RealTimeClock::Initialize();

  if( argc < 4 )
    {
    std::cerr << " Missing arguments: " << argv[0] << "CTImage " << "Tool mesh representation "\
              << "Output image file for a screenshot" << std::endl; 
    return EXIT_FAILURE;
    }

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject<PixelType,Dimension> ImageSpatialObjectType;
  typedef ImageSpatialObjectType::IndexType::IndexValueType IndexValueType;

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
  worldReference->SetSize(100, 100, 100);

  typedef igstk::AxesObjectRepresentation    AxesRepresentationType;
  AxesRepresentationType::Pointer axesRepresentation = AxesRepresentationType::New();
  axesRepresentation->RequestSetAxesObject(worldReference);  

  // Define identity transform
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();
  //reader->SetLogger( logger );

  //set up CT image observer
  typedef ToolProjectionSpatialObjectQtTest::CTImageObserver 
                                                        CTImageObserverType;
  CTImageObserverType::Pointer ctImageObserver = CTImageObserverType::New(); 
  reader->AddObserver(::igstk::CTImageReader::ImageModifiedEvent(), ctImageObserver);

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
  typedef ToolProjectionSpatialObjectQtTest::VTKImageObserver 
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

  // Read a .msh mesh representing the tool
   typedef igstk::MeshReader                           MeshReaderType;

   MeshReaderType::Pointer meshReader = MeshReaderType::New();
   meshReader->RequestSetFileName( argv[2] );

   meshReader->RequestReadObject();
 
   typedef ToolProjectionSpatialObjectQtTest::MeshObjectObserver 
                                                        MeshObjectObserverType;

   MeshObjectObserverType::Pointer meshObserver = MeshObjectObserverType::New();

   meshReader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), meshObserver);

   meshReader->RequestGetOutput();

   if( !meshObserver->GotMeshObject() )
    {
    std::cout << "Got Mesh reader error!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

   typedef MeshReaderType::MeshObjectType             MeshType;
   // get the mesh spatial object
   MeshType::Pointer toolSpatialObject = meshObserver->GetMeshObject();
    
   if (toolSpatialObject.IsNull())
    {
    std::cout << "Got Mesh observer error!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

   // set transform and parent to the mesh spatial object 
   toolSpatialObject->RequestSetTransformAndParent( identity, worldReference );
   
   // tool representation
   typedef igstk::MeshObjectRepresentation                MeshRepresentationType;

   // setup a mesh representation
   MeshRepresentationType::Pointer toolRepresentation = MeshRepresentationType::New();     
   toolRepresentation->RequestSetMeshObject( toolSpatialObject );
   toolRepresentation->SetOpacity(1);
   toolRepresentation->SetColor(1, 0, 0);
  
  typedef igstk::View2D          View2DType;
  typedef igstk::QTWidget        QTWidgetType;

  View2DType::Pointer view2D = View2DType::New();
  //view2D->SetLogger( logger );
       
  // Create a QT minimal GUI
  QApplication app(argc, argv);
  QMainWindow  * qtMainWindow = new QMainWindow();
  qtMainWindow->setFixedSize(512,512);  

  // instantiate QT widget 
  QTWidgetType * qtWidget2D = new QTWidgetType();
  qtWidget2D->RequestSetView( view2D );
  //qtWidget2D->SetLogger( logger );
  qtMainWindow->setCentralWidget( qtWidget2D );

  view2D->RequestSetTransformAndParent( identity, worldReference );
  view2D->SetRefreshRate( 20 );

  qtMainWindow->show();

  QTest::qWait(10);
  
  typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                        ImageResliceRepresentationType; 

  ImageResliceRepresentationType::Pointer  imageResliceRepresentation = 
    ImageResliceRepresentationType::New(); 

  //imageResliceRepresentation->SetLogger( logger );
  imageResliceRepresentation->SetWindowLevel( 1559, -244 );
  imageResliceRepresentation->RequestSetImageSpatialObject( imageSpatialObject );
  
  igstk::Transform toolTransform;
  igstk::Transform::VectorType    translation;
  igstk::Transform::VersorType    rotation;
  
    // a variable to hold image index
  ImageSpatialObjectType::IndexType index;

  // a variable to hold world point coords
  ImageSpatialObjectType::PointType point;

//  initialize the tool transform in the middle of the image
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));
  
  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );

  // auxiliar
  const double *data = NULL;

  data = point.GetVnlVector().data_block();

  translation[0] =  data[0];
  translation[1] =  data[1];
  translation[2] =  data[2];

  const double transformUncertainty = 1.0;

  rotation.SetIdentity();

  toolTransform.SetTranslationAndRotation(
                          translation, 
                          rotation, 
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

  // set transform and parent to the tool spatial object
  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

  // Instantiate and use a reslicer plane spatial object
  typedef igstk::ReslicerPlaneSpatialObject            ReslicerPlaneType;

  ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
//  reslicerPlaneSpatialObject->SetLogger( logger );

  // select Orthogonal reslicing mode
  reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );

  // select Axial reslicing mode
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );

  // set bounding box provider spatial object to the reslicer plane object
  reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( imageSpatialObject );

  // set transform and parent to the reslicer plane
  reslicerPlaneSpatialObject->RequestSetTransformAndParent( identity, worldReference );

  // set the reslicer plane spatial object to the image representation
  imageResliceRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

  // set the tool spatial object to the reslicer plane
  reslicerPlaneSpatialObject->RequestSetToolSpatialObject( toolSpatialObject );

  // setup a tool projection spatial object
  typedef igstk::ToolProjectionSpatialObject             ToolProjectionType;
  typedef igstk::ToolProjectionObjectRepresentation      ToolProjectionRepresentationType;

  ToolProjectionType::Pointer toolProjection = ToolProjectionType::New();
  toolProjection->SetSize(150);
  toolProjection->RequestSetTransformAndParent( identity, worldReference );
  toolProjection->PrintSelf();

  ToolProjectionRepresentationType::Pointer 
            toolProjectionRepresentation = ToolProjectionRepresentationType::New();
  toolProjectionRepresentation->RequestSetToolProjectionObject( toolProjection );
  toolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );
  toolProjectionRepresentation->SetColor( 1,1,0 );
  toolProjectionRepresentation->PrintSelf();

  // important: we do not set the view as child of the reslicer plane yet 
  //view2D->RequestDetachFromParent();
  //view2D->RequestSetTransformAndParent( identity, reslicerPlaneSpatialObject );
  //view2D->SetCameraParallelProjection(true);

  // add the image representation to the view
  view2D->RequestAddObject( imageResliceRepresentation );

  // add the tool representation to the view
  view2D->RequestAddObject( toolRepresentation );

  // add axes representation to the view
  //view2D->RequestAddObject( axesRepresentation );

  // add the tool projection to the view
  view2D->RequestAddObject( toolProjectionRepresentation );

  // Start the view
  view2D->RequestStart();  

//  initialize the tool transform in the middle of the image
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));

  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  data = point.GetVnlVector().data_block();
  
  translation[0] = data[0];
  translation[1] = data[1];
  translation[2] = data[2];

  // set orientation type to Axial
  view2D->RequestSetOrientation( View2DType::Axial );

  // reset camera
  view2D->RequestResetCamera();

  for(unsigned int i=0; i<=360; i++)
  {
      double angle = -3.1416 + 2*3.1416*i/360;
      
      rotation.SetRotationAroundY(angle);

      toolTransform.SetTranslationAndRotation(
                          translation, 
                          rotation, 
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      QTest::qWait(1);
      igstk::PulseGenerator::CheckTimeouts();
  }

  // select Sagittal reslicing mode
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );

  // set orientation type to Sagittal
  view2D->RequestSetOrientation( View2DType::Sagittal );

  // reset camera
  view2D->RequestResetCamera();

  for(unsigned int i=0; i<=360; i++)
  {
      double angle = -3.1416 + 2*3.1416*i/360;
      
      rotation.SetRotationAroundY(angle);

      toolTransform.SetTranslationAndRotation(
                          translation, 
                          rotation, 
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      QTest::qWait(1);
      igstk::PulseGenerator::CheckTimeouts();
  }

  // select Coronal reslicing mode
  reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );

  // set orientation type to Sagittal
  view2D->RequestSetOrientation( View2DType::Coronal );

  // reset camera
  view2D->RequestResetCamera();

  for(unsigned int i=0; i<=360; i++)
  {
      double angle = -3.1416 + 2*3.1416*i/360;
      
      rotation.SetRotationAroundY(angle);

      toolTransform.SetTranslationAndRotation(
                          translation, 
                          rotation, 
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      QTest::qWait(1);
      igstk::PulseGenerator::CheckTimeouts();
  }

  // finally, take a screenshot.
  view2D->RequestResetCamera();

//  initialize the tool transform in the middle of the image
  index[0] = static_cast<IndexValueType>(0.5*(imageExtent[0]+imageExtent[1]));
  index[1] = static_cast<IndexValueType>(0.5*(imageExtent[2]+imageExtent[3]));
  index[2] = static_cast<IndexValueType>(0.5*(imageExtent[4]+imageExtent[5]));

  imageSpatialObject->TransformIndexToPhysicalPoint( index, point );
  data = point.GetVnlVector().data_block();
  translation[0] = data[0];
  translation[1] = data[1];
  translation[2] = data[2];

  toolTransform.SetTranslation(
                      translation,
                      transformUncertainty,
                      igstk::TimeStamp::GetLongestPossibleTime() );

  toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );    

  std::cout << "Saving snapshot to: " << argv[3] << std::endl;
  view2D->RequestStop();
  QTest::qWait(1);
  igstk::PulseGenerator::CheckTimeouts();
  view2D->RequestSaveScreenShot( argv[3] );

  // stop the view
  view2D->RequestStop();
  delete qtWidget2D;
  qtMainWindow->hide();
  delete qtMainWindow;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    std::cout << "Got VTK Logger errors!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
 
  std::cout << "[SUCCESS]" << std::endl;
  return EXIT_SUCCESS;

}
