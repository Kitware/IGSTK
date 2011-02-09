/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshResliceObjectRepresentationQtTest.cxx
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
#include "igstkMeshResliceObjectRepresentation.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"
#include "igstkMeshObject.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"
#include "igstkTransform.h"
#include "igstkView2D.h"

#include "igstkSpatialObject.h"
#include "igstkMeshReader.h"
#include "igstkMeshObjectRepresentation.h"


namespace MeshResliceObjectRepresentationQtTest
{

igstkObserverObjectMacro( MeshObject, ::igstk::MeshReader::MeshModifiedEvent,
                                        ::igstk::MeshObject );

igstkObserverConstObjectMacro( BoundingBox, ::igstk::SpatialObject::BoundingBoxEvent,
                                              ::igstk::SpatialObject::BoundingBoxType);

}


/** This test demonstrates how to perform mesh reslicing using a tool
 * spatial object ( automatic reslicing ) */
int igstkMeshResliceObjectRepresentationQtTest( int argc , char * argv [] )
{
    std::cout << " igstkMeshResliceObjectRepresentationQtTest: application started " << std::endl;

    igstk::RealTimeClock::Initialize();

    if( argc < 4 )
    {
    std::cerr << " Missing arguments: " << argv[0] << \
                 "Mesh for object representation (.msh)" << \
                 "Mesh for tool representation (.msh)" <<\
                 "Filename for a screenshot" << std::endl; 
    return EXIT_FAILURE;
    }

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

    typedef igstk::MeshReader                            MeshReaderType;
    typedef MeshReaderType::MeshObjectType               MeshSpatialObjectType; 
    typedef MeshResliceObjectRepresentationQtTest::MeshObjectObserver 
                                                        MeshObserverType;

    // Read a mesh (.msh) representing the object we want to reslice
    MeshReaderType::Pointer objectReader = MeshReaderType::New();
    objectReader->RequestSetFileName( argv[1] );
    objectReader->RequestReadObject();

    MeshObserverType::Pointer objectObserver = MeshObserverType::New();
    objectReader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), objectObserver);
    objectReader->RequestGetOutput();

    if( !objectObserver->GotMeshObject() )
    {
    std::cout << "Got Mesh reader error!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

    MeshSpatialObjectType::Pointer objectSpatialObject = objectObserver->GetMeshObject();

    if (objectSpatialObject.IsNull())
    {
    std::cout << "Got Mesh observer error!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

    // Read a mesh (.msh) representing the object we want to reslice
    MeshReaderType::Pointer toolReader = MeshReaderType::New();
    toolReader->RequestSetFileName( argv[2] );
    toolReader->RequestReadObject();

    MeshObserverType::Pointer toolObserver = MeshObserverType::New();
    toolReader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), toolObserver);
    toolReader->RequestGetOutput();

    if( !toolObserver->GotMeshObject() )
    {
    std::cout << "Got Mesh reader error!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

    MeshSpatialObjectType::Pointer toolSpatialObject = toolObserver->GetMeshObject();

    if (toolSpatialObject.IsNull())
    {
    std::cout << "Got Mesh observer error!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

    // Define an identity transform
    igstk::Transform identity;
    identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

    // Connect the object and tool spatial object to the world coordinate system
    objectSpatialObject->RequestSetTransformAndParent( identity, worldReference );  
    toolSpatialObject->RequestSetTransformAndParent( identity, worldReference );  
       
    // tool representation
    typedef igstk::MeshObjectRepresentation                MeshRepresentationType;

    // setup a mesh representation for the object and tool
    MeshRepresentationType::Pointer objectRepresentation = MeshRepresentationType::New();     
    objectRepresentation->RequestSetMeshObject( objectSpatialObject );
    objectRepresentation->SetOpacity(0.1);
    objectRepresentation->SetColor(0, 1, 0);

    MeshRepresentationType::Pointer toolRepresentation = MeshRepresentationType::New();     
    toolRepresentation->RequestSetMeshObject( toolSpatialObject );
    toolRepresentation->SetOpacity(1);
    toolRepresentation->SetColor(1, 0, 0);

    // setup a reslicer plane spatial object
    typedef igstk::ReslicerPlaneSpatialObject            ReslicerPlaneType;

    ReslicerPlaneType::Pointer reslicerPlaneSpatialObject = ReslicerPlaneType::New();
    //  reslicerPlaneSpatialObject->SetLogger( logger );

    // select Orthogonal reslicing mode
    reslicerPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Oblique );

    // select Axial reslicing mode
    reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithZAxesNormal );

    // set bounding box provider spatial object to the reslicer plane object
    reslicerPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( objectSpatialObject );

    // set transform and parent to the reslicer plane
    reslicerPlaneSpatialObject->RequestSetTransformAndParent( identity, worldReference );

    // set the tool spatial object to the reslicer plane
    reslicerPlaneSpatialObject->RequestSetToolSpatialObject( toolSpatialObject );


    // setup a mesh reslice object representation
    typedef igstk::MeshResliceObjectRepresentation  MeshResliceRepresentationType; 

    MeshResliceRepresentationType::Pointer  meshResliceRepresentation = 
                                            MeshResliceRepresentationType::New(); 

    //imageResliceRepresentation->SetLogger( logger );
    meshResliceRepresentation->SetOpacity(1);
    meshResliceRepresentation->SetLineWidth(3);
    meshResliceRepresentation->SetLineWidth(3);//Should be 3 already: return
    meshResliceRepresentation->SetVisibility(true);
    meshResliceRepresentation->SetColor(0, 0, 1);     
    meshResliceRepresentation->RequestSetMeshObject( objectSpatialObject );
    meshResliceRepresentation->RequestSetReslicePlaneSpatialObject( reslicerPlaneSpatialObject );

    //Code Coverage.
    MeshResliceRepresentationType::Pointer  meshResliceRepresentationCopy =
       meshResliceRepresentation->Copy();
    meshResliceRepresentationCopy->Print(std::cout);
    meshResliceRepresentationCopy->SetVisibility(false);
    meshResliceRepresentationCopy->RequestSetMeshObject( NULL );
    meshResliceRepresentationCopy->RequestSetReslicePlaneSpatialObject( NULL );
 
    // set up the view

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
     
    // get the bounding box from the reference spatial object
    MeshResliceObjectRepresentationQtTest::BoundingBoxObserver::Pointer  
          boundingBoxObserver = MeshResliceObjectRepresentationQtTest::BoundingBoxObserver::New();
    boundingBoxObserver->Reset();

    objectSpatialObject->AddObserver( 
              ::igstk::SpatialObject::BoundingBoxEvent(), 
              boundingBoxObserver );

    objectSpatialObject->RequestGetBounds();

    typedef ::igstk::SpatialObject::BoundingBoxType   BoundingBoxType;
    BoundingBoxType::ConstPointer boundingBox;

    if( boundingBoxObserver->GotBoundingBox() ) 
    {
     boundingBox = boundingBoxObserver->GetBoundingBox();
    }

    // a variable to hold world point coords
    typedef itk::Point<double,3> PointType;
    PointType point = boundingBox->GetCenter();

    // setup a transform to drive the tool spatial object
    igstk::Transform toolTransform;
    igstk::Transform::VectorType    translation;
    igstk::Transform::VersorType    rotation;

    translation[0] = point[0];
    translation[1] = point[1];
    translation[2] = point[2];

    const double transformUncertainty = 1.0;

    rotation.SetIdentity();

    toolTransform.SetTranslationAndRotation(
                          translation, 
                          rotation, 
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

    // set transform and parent to the tool spatial object
    toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

    // important: we do not set the view as child of the reslicer plane yet 
    view2D->RequestDetachFromParent();
    view2D->RequestSetTransformAndParent( identity, reslicerPlaneSpatialObject );
    view2D->SetCameraParallelProjection(true);

    // add the object representation to the view
    view2D->RequestAddObject( objectRepresentation );

    // add the mesh reslice representation to the view
    view2D->RequestAddObject( meshResliceRepresentation );
    view2D->RequestAddObject( meshResliceRepresentationCopy );

    // Start the view
    view2D->RequestStart();  

    // reset camera
    view2D->RequestResetCamera();
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

    // select Axial reslicing mode
   // reslicerPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithYAxesNormal );

//// reset camera
//    view2D->RequestResetCamera();
//    view2D->RequestResetCamera();

    for(unsigned int i=0; i<=360; i++)
    {
      double angle = -3.1416 + 2*3.1416*i/360;
      
      rotation.SetRotationAroundZ(angle);

      toolTransform.SetTranslationAndRotation(
                          translation, 
                          rotation, 
                          transformUncertainty,
                          igstk::TimeStamp::GetLongestPossibleTime() );

      toolSpatialObject->RequestSetTransformAndParent( toolTransform, worldReference );

      QTest::qWait(1);
      igstk::PulseGenerator::CheckTimeouts();
    }

    // stop the view
    QTest::qWait(200);
    view2D->RequestStop();
    QTest::qWait(200);
    view2D->RequestSaveScreenShot( argv[3] );

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
