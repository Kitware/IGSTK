/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundGuidedRFAImplementation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "UltrasoundGuidedRFAImplementation.h"
#include "igstkEvents.h"

#include "time.h"

namespace igstk
{

/** Constructor */
UltrasoundGuidedRFAImplementation::UltrasoundGuidedRFAImplementation()
                                                         :m_StateMachine(this)
{
  m_Tracker = TrackerType::New();
    
  m_Logger = LoggerType::New();
  m_LogOutput = LogOutputType::New();
  m_LogFile.open("C:/Julien/logUltrasoundGuidedRFA.txt");
  if( !m_LogFile.fail() )
    {
    m_LogOutput->SetStream( m_LogFile );
    }
  else
    {
    std::cerr << "Problem opening Log file, using cerr instead " << std::endl;
    m_LogOutput->SetStream( std::cerr );
    }
  //m_Logger->AddLogOutput( m_LogOutput );
  // add stdout for debug purposes
  LogOutputType::Pointer coutLogOutput = LogOutputType::New();
  coutLogOutput->SetStream( std::cout );
  m_Logger->AddLogOutput( coutLogOutput );
  
  m_Logger->SetPriorityLevel( LoggerType::DEBUG );
  //m_Tracker->SetLogger( m_Logger );

#ifdef UGRFA_USE_FOB
  m_Communication = CommunicationType::New();
  //m_Communication->SetLogger( m_Logger );
  m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  m_Communication->SetParity( igstk::SerialCommunication::NoParity );
  m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate19200 );
#else
  m_Communication = CommunicationType::New();
  //m_Communication->SetLogger( m_Logger );
  m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 ); 
  m_Communication->SetParity( igstk::SerialCommunication::NoParity );
  m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake(
                                   igstk::SerialCommunication::HandshakeOff );
#endif

  m_Tracker->SetCommunication(m_Communication);
  m_Communication->OpenCommunication();

  /** Tool calibration transform */
  igstk::Transform toolCalibrationTransform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;   // Tip offset
  translation[1] = 0;
  translation[2] = 120; //390
      
  igstk::Transform::VersorType rotation;
  //rotation.SetRotationAroundY(-3.141597/2.0);
  rotation.SetRotationAroundZ(3.141597/2.0);
  
  igstk::Transform::VersorType rotation2;
  //rotation2.SetRotationAroundX(3.141597/2.0);
  rotation2.SetRotationAroundX(3.141597);

  rotation = rotation2*rotation;
  
  translation = rotation.Transform(translation);

  toolCalibrationTransform.SetTranslationAndRotation(translation,
                                                  rotation,0.0001,100000000);
  m_Tracker->SetToolCalibrationTransform( TRACKER_TOOL_PORT, 0, 
                                          toolCalibrationTransform);

  /** Tool calibration transform */
  igstk::Transform patientTransform;
  igstk::Transform::VectorType translationP;
  translationP[0] = 0;   // Tip offset
  translationP[1] = 0;
  translationP[2] = -1500;

  igstk::Transform::VersorType rotationP;
  //rotationP.SetRotationAroundY(-3.141597/2.0);
  rotationP.SetRotationAroundZ(-3.141597/2.0);
    
  igstk::Transform::VersorType rotation2P;
  //rotation2P.SetRotationAroundZ(3.141597/2.0);
  rotation2P.SetRotationAroundY(3.141597);
   
  rotationP = rotation2P*rotationP;

  patientTransform.SetTranslationAndRotation(translationP,
                                             rotationP,0.0001,100000000);
  m_Tracker->SetPatientTransform(patientTransform);

  m_Tracker->Open();
  m_Tracker->Initialize();

  // Set up the four quadrant views
  this->Display3D->RequestResetCamera();
  this->Display3D->Update();
  this->Display3D->RequestEnableInteractions();
  this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
  this->Display3D->RequestStart();

  this->Display2D->RequestResetCamera();
  this->Display2D->Update();
  this->Display2D->RequestEnableInteractions();
  this->Display2D->RequestSetRefreshRate( 60 ); // 60 Hz
  this->Display2D->RequestStart();
      
  m_Tracking = false;

  m_MeshReader = LiverReaderType::New();
  m_VascularNetworkReader = VascularNetworkReaderType::New();
  m_MRImageReader = MRImageReaderType::New();
  m_USImageReader = USImageReaderType::New();
  m_ContourLiverRepresentation = ObliqueContourMeshObjectRepresentation::New();
  //m_ContourLiverRepresentation = ContourMeshObjectRepresentation::New();
  
  m_ContourVascularNetworkRepresentation = 
                      ContourVascularNetworkObjectRepresentation::New();
  m_VascularNetworkRepresentation = VascularNetworkRepresentationType::New();
  
  m_ObliquePoint.Fill(0);
  m_HasQuitted = false;
   
  m_LiverMRRepresentation = MRImageRepresentationType::New();
  //m_ObliqueLiverMRRepresentation = MRImageRepresentationType::New();
  m_ObliqueLiverMRRepresentation = MRObliqueImageRepresentationType::New();
  
  this->ObserveAxialBoundsInput(    m_LiverMRRepresentation    );
  
  igstkAddStateMacro( Initial );
  igstkAddInputMacro( AxialBounds );
  igstkAddTransitionMacro( Initial, AxialBounds, 
                           Initial, SetAxialSliderBounds );
 
  igstkSetInitialStateMacro( Initial );
  m_StateMachine.SetReadyToRun();

  m_USSimulator = USSimulatorType::New();
  m_Registration = RegistrationType::New();

}

/** Desctructor */
UltrasoundGuidedRFAImplementation
::~UltrasoundGuidedRFAImplementation()
{
  this->Display3D->RequestStop();
  this->Display2D->RequestStop();
  this->m_Tracker->Reset();
  this->m_Tracker->StopTracking();
  this->m_Tracker->Close();
}

/** Quit */
void UltrasoundGuidedRFAImplementation::Quit()
{
  int i = fl_choice("Are you sure you want to quit?", NULL,"Yes","No"); 
  if ( i == 1 )
    {
    mainWindow->hide();
    m_HasQuitted = true;
    }
  else
    {
    return;
    }
}

void UltrasoundGuidedRFAImplementation::SetAxialSliderBoundsProcessing()
{
  const unsigned int min = m_AxialBoundsInputToBeSet.minimum;
  const unsigned int max = m_AxialBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( 
                                                   ( min + max ) / 2.0 );
  //m_ObliqueLiverMRRepresentation->RequestSetSliceNumber( slice );
  this->slider->minimum( min );
  this->slider->maximum( max );
  this->slider->value( slice );  
  this->slider->activate();
  Fl::check();
}

void UltrasoundGuidedRFAImplementation::EnableTracking()
{
  m_Tracking = true;
  m_Tracker->StartTracking();
}

/** Start the tracking */
void UltrasoundGuidedRFAImplementation::DisableTracking()
{
  m_Tracker->Reset();
  m_Tracker->StopTracking();
  m_Tracking = false;
}

/** Add the probe object to the display */
void UltrasoundGuidedRFAImplementation
::AddProbe( igstk::UltrasoundProbeObjectRepresentation 
            * cylinderRepresentation )
{
  //this->Display2D->RequestAddObject(    cylinderRepresentation->Copy() );
  this->Display3D->RequestAddObject(    cylinderRepresentation->Copy() );
}

/** Add Axes to the display */
void UltrasoundGuidedRFAImplementation
::AddAxes( igstk::AxesObjectRepresentation * cylinderRepresentation )
{
  //this->Display2D->RequestAddObject(    cylinderRepresentation->Copy() );
  this->Display3D->RequestAddObject(    cylinderRepresentation->Copy() );
}

/** Add object to track */
void UltrasoundGuidedRFAImplementation
::AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  const unsigned int toolNumber = 0;
  m_Tracker->AttachObjectToTrackerTool( TRACKER_TOOL_PORT, toolNumber, 
                                                        objectToTrack );
}

/** Load a liver surface */
void UltrasoundGuidedRFAImplementation
::LoadLiverSurface()
{
  const char * liverfilename = fl_file_chooser("Load a liver mesh",
                                               "*.msh","*.msh");
  if(liverfilename)
    {
    m_MeshReader->RequestSetFileName(liverfilename);
    m_MeshReader->RequestReadObject();
    m_LiverRepresentation = MeshObjectRepresentation::New();
    m_LiverRepresentation->RequestSetMeshObject( m_MeshReader->GetOutput() );
    m_LiverRepresentation->SetColor(1.0,0.0,0.0);
    m_LiverRepresentation->SetOpacity(1.0);
    this->Display3D->RequestAddObject(m_LiverRepresentation);
    /*
    m_ContourLiverRepresentation->RequestSetMeshObject( 
                                                  m_MeshReader->GetOutput() );
    //m_ContourLiverRepresentation->RequestSetOrientation(
    //                                  ContourMeshObjectRepresentation::Axial);
    m_ContourLiverRepresentation->SetColor(1.0,0.0,0.0);
    m_ContourLiverRepresentation->SetOpacity(1.0);
    this->Display2D->RequestAddObject(m_ContourLiverRepresentation);
    */
    }
}

/** Load liver vasculature */
void UltrasoundGuidedRFAImplementation
::LoadLiverVasculature()
{
  const char * liverfilename = fl_file_chooser("Load a liver vasculature",
                                               "*.tre","*.tre");
  if(liverfilename)
    {
    m_VascularNetworkReader->RequestSetFileName(liverfilename);
    m_VascularNetworkReader->RequestReadObject();

    VascularNetworkObserver::Pointer vascularNetworkObserver 
                                            = VascularNetworkObserver::New();
    m_VascularNetworkReader->AddObserver(
                        VascularNetworkReader::VascularNetworkModifiedEvent(),
                        vascularNetworkObserver);

    m_VascularNetworkReader->RequestGetVascularNetwork();

    m_VascularNetworkRepresentation->RequestSetVascularNetworkObject( 
                              vascularNetworkObserver->GetVascularNetwork() );
    //m_VascularNetworkRepresentation->SetColor(1.0,0.0,0.0);
    //m_VascularNetworkRepresentation->SetOpacity(1.0);
    this->Display3D->RequestAddObject( m_VascularNetworkRepresentation );

    // Represent as a 2D contour on the 2D View
    m_ContourVascularNetworkRepresentation->RequestSetVascularNetworkObject( 
                              vascularNetworkObserver->GetVascularNetwork() );
    m_ContourVascularNetworkRepresentation->RequestSetOrientation(
                           ContourVascularNetworkObjectRepresentation::Axial);
    m_ContourVascularNetworkRepresentation->SetColor(1.0,1.0,1.0);
    m_ContourVascularNetworkRepresentation->SetOpacity(1.0);

    this->Display2D->RequestAddObject(m_ContourVascularNetworkRepresentation);
    }
}


/** Load liver image */
void UltrasoundGuidedRFAImplementation
::LoadLiverImage()
{
  const char * liverfilename = fl_dir_chooser("Load a liver image","");
  if(liverfilename)
    {
    std::string filename = liverfilename;
    // Remove the slash at the end of the filename
    if(filename[strlen(liverfilename)-1] == '/')
      {
      filename[strlen(liverfilename)-1] = 0;
      }

    //m_MRImageReader->SetLogger(m_Logger);
    MRImageObserver::Pointer mrImageObserver = MRImageObserver::New();

    m_MRImageReader->RequestSetDirectory(filename.c_str());
    m_MRImageReader->RequestReadImage();

    m_MRImageReader->AddObserver(igstk::MRImageReader::ImageModifiedEvent(),
                                 mrImageObserver);

    m_MRImageReader->RequestGetImage();

    if(!mrImageObserver->GotMRImage())
      {
      std::cout << "No MRImage!" << std::endl;
      return;
      }
 
    m_Registration->RequestSetMovingMR3D(mrImageObserver->GetMRImage());

    m_LiverMRRepresentation->RequestSetImageSpatialObject( 
                                                mrImageObserver->GetMRImage());
    m_LiverMRRepresentation->SetWindowLevel(52,52);
    m_LiverMRRepresentation->RequestSetOrientation(
                                            MRImageRepresentationType::Axial);
    this->Display3D->RequestAddObject( m_LiverMRRepresentation );

    m_ObliqueLiverMRRepresentation->RequestSetImageSpatialObject(
                                                mrImageObserver->GetMRImage());
    m_ObliqueLiverMRRepresentation->SetWindowLevel(52,52);
    
    //m_ObliqueLiverMRRepresentation->RequestSetOrientation(
     //                                       MRImageRepresentationType::Axial);
    //m_ObliqueLiverMRRepresentation->RequestGetSliceNumberBounds();

    m_USSimulator->RequestSetImageGeometricModel(mrImageObserver->GetMRImage());

    m_StateMachine.ProcessInputs();

    this->Display2D->RequestAddObject( m_ObliqueLiverMRRepresentation );
    this->Display2D->RequestResetCamera();
    }
}

/** Load 2D Ultrasound */
void UltrasoundGuidedRFAImplementation
::Load2DUltrasound()
{
  const char * liverfilename = fl_dir_chooser("Load a 2D ultrasound","");
  if(liverfilename)
    {
    std::string filename = liverfilename;

    // Remove the slash at the end of the filename
    if(filename[strlen(liverfilename)-1] == '/')
      {
      filename[strlen(liverfilename)-1] = 0;
      }

    m_USImageReader->RequestSetDirectory(filename.c_str());
    m_USImageReader->RequestReadImage();

    USImageObserver::Pointer usImageObserver = USImageObserver::New();
    m_USImageReader->AddObserver(igstk::USImageReader::ImageModifiedEvent(),
                                 usImageObserver);

    m_USImageReader->RequestGetImage();

    if(!usImageObserver->GotUSImage())
      {
      std::cout << "No USImage!" << std::endl;
      return;
      }
 
    m_LiverUSRepresentation = USImageRepresentationType::New();
    m_LiverUSRepresentation->RequestSetImageSpatialObject(
                                              usImageObserver->GetUSImage());
    m_LiverUSRepresentation->SetWindowLevel(255/2.0,255/2.0);
    this->Display2D->RequestAddObject( m_LiverUSRepresentation );
    this->Display2D->RequestResetCamera();
    }
}

/** Set the slice number */
void UltrasoundGuidedRFAImplementation
::SetSliceNumber(unsigned int value)
{
  m_LiverMRRepresentation->RequestSetSliceNumber(value);
  //m_ObliqueLiverMRRepresentation->RequestSetSliceNumber(value);

  MRObliqueImageRepresentationType::PointType origin;
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = value*0.78125;
  MRObliqueImageRepresentationType::VectorType v1;
  v1[0] = 1;
  v1[1] = 0;
  v1[2] = 0;
  MRObliqueImageRepresentationType::VectorType v2;
  v2[0] = 0;
  v2[1] = 1;
  v2[2] = 0;

  m_ObliqueLiverMRRepresentation->RequestSetOriginPointOnThePlane(origin);
  m_ObliqueLiverMRRepresentation->RequestSetVector1OnThePlane(v1);
  m_ObliqueLiverMRRepresentation->RequestSetVector2OnThePlane(v2);
  m_ObliqueLiverMRRepresentation->RequestReslice();

  //m_ContourLiverRepresentation->RequestSetOrientation(
  //                                    ContourObjectRepresentation::Axial);
  //m_ContourLiverRepresentation->RequestSetSlicePosition(value*0.78125);
  m_ContourLiverRepresentation->RequestSetOriginPointOnThePlane(origin);
  m_ContourLiverRepresentation->RequestSetVector1OnThePlane(v1);
  m_ContourLiverRepresentation->RequestSetVector2OnThePlane(v2);
  m_ContourLiverRepresentation->RequestReslice();
  
  this->Display2D->RequestResetCamera();

}

/** Randomize. Test only. */
void UltrasoundGuidedRFAImplementation
::Randomize()
{
  if(!m_Tracking)
    {
    return;
    }

  m_Tracker->UpdateStatus();

  typedef igstk::Transform            TransformType;
  typedef TransformType::VectorType   VectorType;

  TransformType             transform;
  VectorType                position;

  m_Tracker->GetToolTransform( 0, 0, transform );
  position = transform.GetTranslation();
  m_ObliquePoint[0] = position[0]-(512*0.78125/2);
  m_ObliquePoint[1] = position[1];
  m_ObliquePoint[2] = position[2];

  MRObliqueImageRepresentationType::VectorType v1;
  v1[0] = 1;
  v1[1] = 0;
  v1[2] = 0;
  v1 = transform.GetRotation().Transform(v1);
  
  MRObliqueImageRepresentationType::VectorType v2;
  v2[0] = 0;
  v2[1] = 0;
  v2[2] = 1;
  v2 = transform.GetRotation().Transform(v2);  
  m_ObliqueLiverMRRepresentation->RequestSetOriginPointOnThePlane(
                                                             m_ObliquePoint);
  m_ObliqueLiverMRRepresentation->RequestSetVector1OnThePlane(v1);
  m_ObliqueLiverMRRepresentation->RequestSetVector2OnThePlane(v2);
  m_ObliqueLiverMRRepresentation->RequestReslice();

  m_ContourLiverRepresentation->RequestSetOriginPointOnThePlane(
                                                             m_ObliquePoint);
  m_ContourLiverRepresentation->RequestSetVector1OnThePlane(v1);
  m_ContourLiverRepresentation->RequestSetVector2OnThePlane(v2);
  m_ContourLiverRepresentation->RequestReslice();

  this->Display2D->RequestResetCamera();

  // Generate 
  /*m_USSimulator->RequestSetTransform(transform);
  m_USSimulator->RequestReslice();
  //m_USSimulator->SetLogger(m_Logger);

  SimulatedUSImageObserver::Pointer usImageObserver = 
                                              SimulatedUSImageObserver::New();
  m_USSimulator->AddObserver(USSimulatorType::ImageModifiedEvent(),
                             usImageObserver);

  m_USSimulator->RequestGetImage();

  if(!usImageObserver->GotSimulatedUSImage())
    {
    std::cout << "No USImage!" << std::endl;
    return;
    }
  */
  /*
  USImageObject::Pointer usImage = usImageObserver->GetSimulatedUSImage();

  USImageTransformObserver::Pointer transformObserver = 
                                                USImageTransformObserver::New();

  usImage->AddObserver( ::igstk::TransformModifiedEvent(), 
                        transformObserver );
  usImage->RequestGetTransform();
  
  if( !transformObserver->GotUSImageTransform() )
    {
    std::cerr << "The usImage did not returned ";
    std::cerr << "a Transform event" << std::endl;
    return;
    }

  std::cout << transformObserver->GetUSImageTransform() << std::endl;
  */

  //registration->SetLogger(logger);
  // m_Registration->RequestSetFixedUS3D(
  //                                   usImageObserver->GetSimulatedUSImage()); 
  // m_Registration->RequestCalculateRegistration();

  /*
  RegistrationTransformObserver::Pointer registrationTransformObserver 
                                    = RegistrationTransformObserver::New();

  m_Registration->AddObserver(TransformModifiedEvent(),
                              registrationTransformObserver);
  m_Registration->RequestGetRegistrationTransform();

  if(registrationTransformObserver->GotRegistrationTransform())
    {
    std::cout << "Got Transform! " << std::endl;
    std::cout << registrationTransformObserver->GetRegistrationTransform() 
              << std::endl;
    }
  else
    {
    std::cout << "No Transform!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return;
    }*/
}

} // end of namespace
