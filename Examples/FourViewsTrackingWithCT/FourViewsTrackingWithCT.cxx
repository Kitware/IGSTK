/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FourViewsTrackingWithCT.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "FourViewsTrackingWithCT.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"
#include "vtkCommand.h"
#include "igstkEvents.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
FourViewsTrackingWithCT::FourViewsTrackingWithCT():m_StateMachine(this)
{  
  /** Setup m_Logger, this logger is used to log the application class. */
  m_Logger = LoggerType::New();

  /** Setup logger, for all other igstk components. */
  logger   = LoggerType::New();

  /** Direct the application log message to the std::cout and FLTK text display. */
  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cout );
  m_Logger->AddLogOutput( m_LogCoutOutput );

  Fl_Text_Buffer * textBuffer = new Fl_Text_Buffer();                    //FIXME
  this->m_LogWindow->buffer( textBuffer );
  m_LogFLTKOutput = FLTKTextLogOutput::New();
  m_LogFLTKOutput->SetStream( *m_LogWindow );
  m_Logger->AddLogOutput( m_LogFLTKOutput );

  /** Direct the igstk components log message to the file. */
  m_LogFileOutput = LogOutputType::New();
  std::string logFileName = "logFourViewsTrackingWithCT.txt";                   //FIXME. use a date/time file name
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
    m_Logger->AddLogOutput( m_LogFileOutput );
    logger->AddLogOutput( m_LogFileOutput );
    }
  else
    {
    igstkLogMacro( DEBUG, "Problem opening Log file: " << logFileName << "\n" ); //FIXME. may not return
    return;
    }

  /** Initialize all member variables and set logger */
  m_ImageReader = ImageReaderType::New();
  m_ImageReader->SetLogger( logger );

  m_LandmarkRegistration = RegistrationType::New();
  m_LandmarkRegistration->SetLogger( logger );
  m_LandmarkRegistrationObserver = ObserverType2::New();
  m_LandmarkRegistrationObserver->SetCallbackFunction( this, &FourViewsTrackingWithCT::GetLandmarkRegistrationTransform );
  m_LandmarkRegistration->AddObserver( TransformModifiedEvent(), m_LandmarkRegistrationObserver );

  m_SerialCommunication = CommunicationType::New();
  m_SerialCommunication->SetLogger( logger );
  m_SerialCommunication->SetPortNumber( SerialCommunication::PortNumber2 ); // FIXME. Should be configurable in CMake, GUI or ini file
  m_SerialCommunication->SetParity( SerialCommunication::NoParity );
  m_SerialCommunication->SetBaudRate( SerialCommunication::BaudRate9600 );
  m_SerialCommunication->SetDataBits( SerialCommunication::DataBits8 );
  m_SerialCommunication->SetStopBits( SerialCommunication::StopBits1 );
  m_SerialCommunication->SetHardwareHandshake( SerialCommunication::HandshakeOff );
  m_SerialCommunication->OpenCommunication();
  
  m_Tracker = TrackerType::New();
  m_Tracker->SetLogger( logger );
  m_Tracker->SetCommunication( m_SerialCommunication );

  /** Tool calibration transform */
  igstk::Transform toolCalibrationTransform;
  igstk::Transform::VectorType translation;
  igstk::Transform::VersorType rotation;
  translation[0] = -18.0;   // Tip offset
  translation[1] = 0.5;
  translation[2] = -157.5;
  rotation.SetIdentity();
  //rotation.SetRotationAroundX(-1.0);
  toolCalibrationTransform.SetTranslationAndRotation(translation, rotation, 0.1, 10000);
  m_Tracker->SetToolCalibrationTransform(toolCalibrationTransform);
  //toolCalibrationTransform.SetToIdentity(10000);

  m_PulseGenerator = PulseGenerator::New();
  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & FourViewsTrackingWithCT::GetTrackerTransform );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );
  m_PulseGenerator->RequestSetFrequency( 20 ); //FIXME, move to request start tracking??

  /** Temporary disable this logger */
  //this->DisplayAxial->SetLogger( logger );
  //this->DisplaySagittal->SetLogger( logger );
  //this->DisplayCoronal->SetLogger( logger );

  m_ViewPickerObserver = ObserverType2::New();
  m_ViewPickerObserver->SetCallbackFunction( this, &FourViewsTrackingWithCT::DrawPickedPoint );

  this->DisplayAxial->AddObserver( TransformModifiedEvent(), m_ViewPickerObserver );
  this->DisplaySagittal->AddObserver( TransformModifiedEvent(), m_ViewPickerObserver );
  this->DisplayCoronal->AddObserver( TransformModifiedEvent(), m_ViewPickerObserver );

  this->DisplayAxial->RequestSetOrientation( igstk::View2D::Axial );
  this->DisplaySagittal->RequestSetOrientation( igstk::View2D::Sagittal );
  this->DisplayCoronal->RequestSetOrientation( igstk::View2D::Coronal );

  m_ImageRepresentationAxial    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal = ImageRepresentationType::New();
  m_ImageRepresentationCoronal  = ImageRepresentationType::New();
  m_ImageRepresentationAxial3D    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal3D = ImageRepresentationType::New();
  m_ImageRepresentationCoronal3D  = ImageRepresentationType::New();

  m_ImageRepresentationAxial->SetLogger( logger );
  m_ImageRepresentationSagittal->SetLogger( logger );
  m_ImageRepresentationCoronal->SetLogger( logger );
  m_ImageRepresentationAxial3D->SetLogger( logger );
  m_ImageRepresentationSagittal3D->SetLogger( logger );
  m_ImageRepresentationCoronal3D->SetLogger( logger );


  m_Ellipsoid                   = EllipsoidType::New();
  m_EllipsoidRepresentation     = EllipsoidRepresentationType::New();
  m_Ellipsoid->SetRadius( 10, 10, 10 );  
  m_EllipsoidRepresentation->RequestSetEllipsoidObject( m_Ellipsoid );
  m_EllipsoidRepresentation->SetColor(1.0,0.0,0.0);
  m_EllipsoidRepresentation->SetOpacity(1.0);

  m_Cylinder                    = CylinderType::New();
  m_CylinderRepresentation      = CylinderRepresentationType::New();
  m_Cylinder->SetRadius( 1.5 );   //   1.5 mm
  m_Cylinder->SetHeight( 200 );   // 200.0 mm
  m_CylinderRepresentation->RequestSetCylinderObject( m_Cylinder );
  m_CylinderRepresentation->SetColor(0.0,1.0,0.0);
  m_CylinderRepresentation->SetOpacity(1.0);

  /** Initialize State Machine */
  m_StateMachine.AddState( m_InitialState,                "InitialState"              );
  m_StateMachine.AddState( m_PatientNameReadyState,       "PatientNameReadyState"     );
  m_StateMachine.AddState( m_ImageReadyState,             "ImageReadyState"           );
  m_StateMachine.AddState( m_PatientNameVerifiedState,    "PatientNameVerifiedState"  );
  m_StateMachine.AddState( m_TrackerReadyState,           "TrackerReadyState"         );
  m_StateMachine.AddState( m_AddingImageLandmarkState,    "AddingImageLandmarkState"  );
  m_StateMachine.AddState( m_ImageLandmarksReadyState,    "ImageLandmarksReadyState"  );
  m_StateMachine.AddState( m_AddingTrackerLandmarkState,  "AddingTrackerLandmarkState"  );
  m_StateMachine.AddState( m_TrackerLandmarksReadyState,  "TrackerLandmarksReadyState");
  m_StateMachine.AddState( m_LandmarkRegistrationReadyState, "LandmarkRegistrationReadyState" );
  m_StateMachine.AddState( m_TrackingState,               "TrackingState"             );

  m_StateMachine.AddInput( m_RequestSetPatientNameInput,        "RequestSetPatientNameInput"      );
  m_StateMachine.AddInput( m_RequestLoadImageInput,             "RequestLoadImageInput"           );
  m_StateMachine.AddInput( m_RequestInitializeTrackerInput,     "RequestInitializeTrackerInput"   );
  m_StateMachine.AddInput( m_RequestAddImageLandmarkInput,      "RequestAddImageLandmarkInput"   );
  m_StateMachine.AddInput( m_RequestClearImageLandmarksInput,   "RequestClearImageLandmarksInput"   );
  m_StateMachine.AddInput( m_RequestAddTrackerLandmarkInput,    "RequestAddTrackerLandmarkInput" );
  m_StateMachine.AddInput( m_RequestClearTrackerLandmarksInput, "RequestClearTrackerLandmarksInput" );
  m_StateMachine.AddInput( m_RequestRegistrationInput,          "RequestRegistrationInput"        );
  m_StateMachine.AddInput( m_RequestStartTrackingInput,         "RequestStartTrackingInput"       );
  m_StateMachine.AddInput( m_RequestStopTrackingInput,          "RequestStopTrackingInput"        );
  m_StateMachine.AddInput( m_RequestResliceImageInput,          "RequestResliceImageInput");

  m_StateMachine.AddInput( m_PatientNameInput,                  "PatientNameInput"          );
  m_StateMachine.AddInput( m_PatientNameEmptyInput,             "PatientNameEmptyInput"     );

  m_StateMachine.AddInput( m_LoadImageSuccessInput,             "LoadImageSuccessInput"     );
  m_StateMachine.AddInput( m_LoadImageFailureInput,             "LoadImageFailureInput"     );

  m_StateMachine.AddInput( m_PatientNameMatchInput,             "PatientNameMatchInput"     );
  m_StateMachine.AddInput( m_OverwritePatientNameInput,         "OverwritePatientNameInput" );
  m_StateMachine.AddInput( m_ReloadImageInput,                  "ReloadImageInput"          );

  m_StateMachine.AddInput( m_InitializeTrackerSuccessInput,     "InitializeTrackerSuccessInput");
  m_StateMachine.AddInput( m_InitializeTrackerFailureInput,     "InitializeTrackerFailureInput");

  m_StateMachine.AddInput( m_NeedMoreLandmarkPointsInput,       "NeedMoreLandmarkPointsInput"   );
  m_StateMachine.AddInput( m_EnoughLandmarkPointsInput,         "EnoughLandmarkPointsInput"   );

  m_StateMachine.AddInput( m_RegistrationSuccessInput,          "RegistrationSuccessInput");
  m_StateMachine.AddInput( m_RegistrationFailureInput,          "RegistrationFailureInput");

  m_StateMachine.AddInput( m_StartTrackingSuccessInput,         "StartTrackingSuccessInput");
  m_StateMachine.AddInput( m_StartTrackingFailureInput,         "StartTrackingFailureInput");

  m_StateMachine.AddInput( m_StopTrackingSuccessInput,          "StopTrackingSuccessInput");
  m_StateMachine.AddInput( m_StopTrackingFailureInput,          "StopTrackingFailureInput");

  const ActionType NoAction = 0;
  
  /** Register patient name */
  m_StateMachine.AddTransition( m_InitialState, m_RequestSetPatientNameInput, m_InitialState, 
    &FourViewsTrackingWithCT::SetPatientName );
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_RequestSetPatientNameInput, m_InitialState, 
    &FourViewsTrackingWithCT::SetPatientName );
  m_StateMachine.AddTransition( m_InitialState, m_PatientNameInput, m_PatientNameReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_InitialState, m_PatientNameEmptyInput, m_InitialState, 
    NoAction );
  /** Load image and verify patient name */
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_RequestLoadImageInput, m_PatientNameReadyState, 
    &FourViewsTrackingWithCT::LoadImage );
  m_StateMachine.AddTransition( m_ImageReadyState, m_RequestLoadImageInput, m_PatientNameReadyState, 
    &FourViewsTrackingWithCT::LoadImage );
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_LoadImageSuccessInput, m_ImageReadyState, 
    &FourViewsTrackingWithCT::VerifyPatientName );
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_LoadImageFailureInput, m_PatientNameReadyState, 
    NoAction );
  /** Display image */
  m_StateMachine.AddTransition( m_ImageReadyState, m_PatientNameMatchInput, m_PatientNameVerifiedState, 
    &FourViewsTrackingWithCT::ConnectImageRepresentation );
  m_StateMachine.AddTransition( m_ImageReadyState, m_OverwritePatientNameInput, m_PatientNameVerifiedState, 
    &FourViewsTrackingWithCT::ConnectImageRepresentation );
  m_StateMachine.AddTransition( m_ImageReadyState, m_ReloadImageInput, m_PatientNameReadyState, 
    &FourViewsTrackingWithCT::RequestLoadImage );
  /** Initialize tracker */
  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_RequestInitializeTrackerInput, m_PatientNameVerifiedState, 
    &FourViewsTrackingWithCT::InitializeTracker );
  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_InitializeTrackerSuccessInput, m_TrackerReadyState, 
   NoAction );
  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_InitializeTrackerFailureInput, m_PatientNameVerifiedState, 
   NoAction );
  /** Set image landmarks */
  m_StateMachine.AddTransition( m_TrackerReadyState, m_RequestAddImageLandmarkInput, m_TrackerReadyState, 
    &FourViewsTrackingWithCT::AddImageLandmark );
  m_StateMachine.AddTransition( m_AddingImageLandmarkState, m_RequestAddImageLandmarkInput, m_AddingImageLandmarkState, 
    &FourViewsTrackingWithCT::AddImageLandmark );
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_RequestAddImageLandmarkInput, m_ImageLandmarksReadyState, 
    &FourViewsTrackingWithCT::AddImageLandmark );
  m_StateMachine.AddTransition( m_TrackerReadyState, m_NeedMoreLandmarkPointsInput, m_AddingImageLandmarkState, 
    NoAction );
  m_StateMachine.AddTransition( m_AddingImageLandmarkState, m_NeedMoreLandmarkPointsInput, m_AddingImageLandmarkState, 
    NoAction );
  m_StateMachine.AddTransition( m_AddingImageLandmarkState, m_EnoughLandmarkPointsInput, m_ImageLandmarksReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_EnoughLandmarkPointsInput, m_ImageLandmarksReadyState, 
    NoAction );
  /** Clear image landmarks */
  m_StateMachine.AddTransition( m_AddingImageLandmarkState, m_RequestClearImageLandmarksInput, m_TrackerReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_RequestClearImageLandmarksInput, m_TrackerReadyState, 
    NoAction );

  /** Set tracker landmarks */
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_RequestAddTrackerLandmarkInput, m_ImageLandmarksReadyState, 
    &FourViewsTrackingWithCT::AddTrackerLandmark );
  m_StateMachine.AddTransition( m_AddingTrackerLandmarkState, m_RequestAddTrackerLandmarkInput, m_AddingTrackerLandmarkState, 
    &FourViewsTrackingWithCT::AddTrackerLandmark );
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_RequestAddTrackerLandmarkInput, m_TrackerLandmarksReadyState, 
    &FourViewsTrackingWithCT::AddTrackerLandmark );
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_NeedMoreLandmarkPointsInput, m_AddingTrackerLandmarkState, 
    NoAction );
  m_StateMachine.AddTransition( m_AddingTrackerLandmarkState, m_NeedMoreLandmarkPointsInput, m_AddingTrackerLandmarkState, 
    NoAction );
  m_StateMachine.AddTransition( m_AddingTrackerLandmarkState, m_EnoughLandmarkPointsInput, m_TrackerLandmarksReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_EnoughLandmarkPointsInput, m_TrackerLandmarksReadyState, 
    NoAction );
  /** Clear tracker landmarks */
  m_StateMachine.AddTransition( m_AddingTrackerLandmarkState, m_RequestClearTrackerLandmarksInput, m_ImageLandmarksReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_RequestClearTrackerLandmarksInput, m_ImageLandmarksReadyState, 
    NoAction );

  /** Registration */
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_RequestRegistrationInput, m_TrackerLandmarksReadyState,
    & FourViewsTrackingWithCT::Registration );
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_RegistrationSuccessInput, m_LandmarkRegistrationReadyState,
    NoAction );
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_RegistrationFailureInput, m_TrackerLandmarksReadyState,
    NoAction );

  /** Tracking */
  m_StateMachine.AddTransition( m_LandmarkRegistrationReadyState, m_RequestStartTrackingInput, m_LandmarkRegistrationReadyState,
    & FourViewsTrackingWithCT::StartTracking );
  m_StateMachine.AddTransition( m_LandmarkRegistrationReadyState, m_StartTrackingSuccessInput, m_TrackingState,
    NoAction);
  m_StateMachine.AddTransition( m_LandmarkRegistrationReadyState, m_StartTrackingFailureInput, m_LandmarkRegistrationReadyState,
    NoAction );

  m_StateMachine.AddTransition( m_TrackingState, m_RequestStopTrackingInput, m_TrackingState,
    & FourViewsTrackingWithCT::StopTracking );
  m_StateMachine.AddTransition( m_TrackingState, m_StopTrackingSuccessInput, m_LandmarkRegistrationReadyState,
    NoAction);
  m_StateMachine.AddTransition( m_TrackingState, m_StopTrackingFailureInput, m_TrackingState,
    NoAction );

  /** Reslicing image does not change any state*/
  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_RequestResliceImageInput, m_PatientNameVerifiedState,
    & FourViewsTrackingWithCT::ResliceImage );
  m_StateMachine.AddTransition( m_TrackerReadyState, m_RequestResliceImageInput, m_TrackerReadyState,
    & FourViewsTrackingWithCT::ResliceImage );
  m_StateMachine.AddTransition( m_AddingImageLandmarkState, m_RequestResliceImageInput, m_AddingImageLandmarkState,
    & FourViewsTrackingWithCT::ResliceImage );
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_RequestResliceImageInput, m_ImageLandmarksReadyState,
    & FourViewsTrackingWithCT::ResliceImage );
  m_StateMachine.AddTransition( m_TrackerLandmarksReadyState, m_RequestResliceImageInput, m_TrackerLandmarksReadyState,
    & FourViewsTrackingWithCT::ResliceImage );
  m_StateMachine.AddTransition( m_TrackingState, m_RequestResliceImageInput, m_TrackingState,
    & FourViewsTrackingWithCT::ResliceImage );
  
  m_StateMachine.SelectInitialState( m_InitialState );
  m_StateMachine.SetReadyToRun();

  std::ofstream ofile;
  ofile.open("DemoApplicationStateMachineDiagram.dot");
 
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );

  ofile.close();

}

/** Destructor */
FourViewsTrackingWithCT::~FourViewsTrackingWithCT()
{
 
}

void FourViewsTrackingWithCT::RequestSetPatientName()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestSetPatientName called...\n" )
  m_StateMachine.PushInput( m_RequestSetPatientNameInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::SetPatientName()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::GetPatientName called...\n" )
  const char *patientName = fl_input("Patient Name:", "");
  if( patientName != NULL )
    {
    m_PatientName = patientName;
    igstkLogMacro(          DEBUG, "Patient registered as: "<< m_PatientName <<"\n" )
    igstkLogMacro2( logger, DEBUG, "Patient registered as: "<< m_PatientName <<"\n" )
    m_StateMachine.PushInput( m_PatientNameInput );
    }
  else
    {
    m_StateMachine.PushInput( m_PatientNameEmptyInput );
    }
}


void FourViewsTrackingWithCT::RequestLoadImage()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestLoadImage called...\n" )
  m_StateMachine.PushInput( m_RequestLoadImageInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::LoadImage()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::LoadImage called...\n" )
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  if ( directoryname != NULL )
    {
      igstkLogMacro2( logger, DEBUG, "m_ImageReader->RequestSetDirectory called...\n" )
      m_ImageReader->RequestSetDirectory( directoryname ); //FIXME. Add observer and callbacks to catch errors?

      igstkLogMacro2( logger, DEBUG, "m_ImageReader->RequestReadImage called... \n")
      m_ImageReader->RequestReadImage();                   //FIXME. Add observer and callbacks to catch errors?

      m_StateMachine.PushInputBoolean( m_ImageReader->FileSuccessfullyRead(), m_LoadImageSuccessInput, m_LoadImageFailureInput);
    }
  else
    {
      igstkLogMacro(          DEBUG, "No directory is selected\n")
      igstkLogMacro2( logger, DEBUG, "No directory is selected\n")
    }
}

void FourViewsTrackingWithCT::VerifyPatientName()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::VerifyPatientName called ... \n")
  if ( m_ImageReader->GetPatientName() == m_PatientName )
    {
    m_StateMachine.PushInput( m_PatientNameMatchInput );
    }
  else
    {
    igstkLogMacro( DEBUG, "Patient name mismatch\n")
      std::string msg = "Patient Registered as: " + m_PatientName + "\n";
    msg += "Image has the name of: " + m_ImageReader->GetPatientName() +"\n";
    msg += "Name mismatch!!!!\n";
    msg += "Do you want to load another image? choose \'no\' will overwrite the name\n";
    int i = fl_ask( msg.c_str() );
    if ( i )
      {
      m_StateMachine.PushInput( m_ReloadImageInput );
      }
    else
      {
      m_PatientName = m_ImageReader->GetPatientName();
      igstkLogMacro( DEBUG, "Patient name is overwritten to:" << m_PatientName << "\n")
      m_StateMachine.PushInput( m_OverwritePatientNameInput );
      }
    }

}

void FourViewsTrackingWithCT::RequestInitializeTracker()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestInitializeTracker called ... \n")
  m_StateMachine.PushInput( m_RequestInitializeTrackerInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::InitializeTracker()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::InitializeTracker called ... \n")    
  m_Tracker->Open();
  m_Tracker->AttachSROMFileNameToPort( 3, "C:/Patrick/Vicra/Tookit/Tool Definition Files/8700340.rom" ); //FIXME use ini file
  m_Tracker->Initialize();            //FIXME, how to check if this is success???
  m_Tracker->StartTracking();

  m_StateMachine.PushInputBoolean( m_Tracker->GetNumberOfTools(), m_InitializeTrackerSuccessInput, m_InitializeTrackerFailureInput );
}

void FourViewsTrackingWithCT::RequestAddImageLandmark()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestAddImageLandmark called ... \n")
  m_StateMachine.PushInput( m_RequestAddImageLandmarkInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::AddImageLandmark()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::AddImageLandmark called ... \n")
  LandmarkPointType  p;
  m_ImageLandmarksContainer.push_back( p );                       // FIXME, Need real point from user
  m_StateMachine.PushInputBoolean( (m_ImageLandmarksContainer.size()>=3), m_EnoughLandmarkPointsInput, m_NeedMoreLandmarkPointsInput);
}

void FourViewsTrackingWithCT::RequestClearImageLandmarks()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestClearImageLandmarks called ... \n")
  m_ImageLandmarksContainer.clear();
  m_StateMachine.PushInput( m_RequestClearImageLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::RequestAddTrackerLandmark()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestAddTrackerLandmark called ... \n")
  m_StateMachine.PushInput( m_RequestAddTrackerLandmarkInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::AddTrackerLandmark()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::AddTrackerLandmark called ... \n")
  
  TrackerType::TransformType    transitions;
  m_Tracker->UpdateStatus();
  m_Tracker->GetToolTransform( 3, 0, transitions );                 //FIXME, port number
  
  LandmarkPointType  p;
  p[0] = transitions.GetTranslation()[0];
  p[1] = transitions.GetTranslation()[1];
  p[2] = transitions.GetTranslation()[2];
  m_TrackerLandmarksContainer.push_back( p );                       // Need testing

  std::cout<< p << "\n";

  //igstkLogMacro2( logger, DEBUG,  p << "\n")

  m_StateMachine.PushInputBoolean( (m_TrackerLandmarksContainer.size()>=3), m_EnoughLandmarkPointsInput, m_NeedMoreLandmarkPointsInput);
}

void FourViewsTrackingWithCT::RequestClearTrackerLandmarks()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestClearTrackerLandmarks called ... \n")
  m_TrackerLandmarksContainer.clear();
  m_StateMachine.PushInput( m_RequestClearTrackerLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::RequestRegistration()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestRegistration called ... \n")
  m_StateMachine.PushInput( m_RequestRegistrationInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::Registration()
{
  LandmarkPointContainerType::iterator it1, it2;
  for( it1 = m_ImageLandmarksContainer.begin(), it2 = m_TrackerLandmarksContainer.begin(); 
    it1!=m_ImageLandmarksContainer.end(), it2!=m_TrackerLandmarksContainer.end(); it1++, it2++)
    {
    m_LandmarkRegistration->RequestAddImageLandmarkPoint( *it1 );
    m_LandmarkRegistration->RequestAddTrackerLandmarkPoint( *it2 );
    }

  // Calculate transform
  m_LandmarkRegistration->RequestComputeTransform();
  m_LandmarkRegistration->RequestGetTransform();

}

void FourViewsTrackingWithCT::RequestStartTracking()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestStartTracking called ... \n")
  m_StateMachine.PushInput( m_RequestStartTrackingInput );
  m_StateMachine.ProcessInputs();
}
void FourViewsTrackingWithCT::StartTracking()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::StartTracking called ... \n")
  m_Tracker->AttachObjectToTrackerTool( 3, 0, m_Cylinder );
  m_Tracker->StartTracking();
  // We don't have observer for tracker, we are actively reading the transform right now
  m_PulseGenerator->RequestStart(); 
  m_StateMachine.PushInput( m_StartTrackingSuccessInput ); // FIXME, How to get the failure condition
}

void FourViewsTrackingWithCT::RequestStopTracking()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestStopTracking called ... \n")
  m_StateMachine.PushInput( m_RequestStopTrackingInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::StopTracking()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::StopTracking called ... \n")
  // We don't have observer for tracker, we are actively reading the transform right now
  m_PulseGenerator->RequestStop();
  m_Tracker->StopTracking();
  m_StateMachine.PushInput( m_StopTrackingSuccessInput ); // FIXME, How to get the failure condition
}

void FourViewsTrackingWithCT::RequestResliceImage()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestResliceImage called ... \n")
  m_StateMachine.PushInput( m_RequestResliceImageInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::ResliceImage()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::ResliceImage called ... \n")
  m_SliceNumberToBeSet[0] = static_cast<unsigned int>( this->AxialSlider->value() );
  m_SliceNumberToBeSet[1] = static_cast<unsigned int>( this->SagittalSlider->value() );
  m_SliceNumberToBeSet[2] = static_cast<unsigned int>( this->CoronalSlider->value() );

  // FIXME.
  m_SliceNumber[0] = m_SliceNumberToBeSet[0];
  m_SliceNumber[1] = m_SliceNumberToBeSet[1];
  m_SliceNumber[2] = m_SliceNumberToBeSet[2];

  m_ImageRepresentationAxial->RequestSetSliceNumber( m_SliceNumber[0] );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( m_SliceNumber[1] );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( m_SliceNumber[2] );

  m_ImageRepresentationAxial3D->RequestSetSliceNumber( m_SliceNumber[0] );
  m_ImageRepresentationSagittal3D->RequestSetSliceNumber( m_SliceNumber[1] );
  m_ImageRepresentationCoronal3D->RequestSetSliceNumber( m_SliceNumber[2] );

  this->ViewerGroup->redraw();
  Fl::check();
}


/** This method should be invoked by the State Machine 
 *  only when the Image has been loaded and the Patient
 *  name has been verified */
void FourViewsTrackingWithCT::ConnectImageRepresentation()
{
  m_ImageRepresentationAxial->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
  m_ImageRepresentationSagittal->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
  m_ImageRepresentationCoronal->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );

  m_ImageRepresentationAxial3D->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
  m_ImageRepresentationSagittal3D->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
  m_ImageRepresentationCoronal3D->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
 
  m_ImageRepresentationAxial->RequestSetOrientation( ImageRepresentationType::Axial );
  m_ImageRepresentationSagittal->RequestSetOrientation( ImageRepresentationType::Sagittal );
  m_ImageRepresentationCoronal->RequestSetOrientation( ImageRepresentationType::Coronal );

  m_ImageRepresentationAxial3D->RequestSetOrientation( ImageRepresentationType::Axial );
  m_ImageRepresentationSagittal3D->RequestSetOrientation( ImageRepresentationType::Sagittal );
  m_ImageRepresentationCoronal3D->RequestSetOrientation( ImageRepresentationType::Coronal );

  
  /** Initialize the slider */
  unsigned int min = m_ImageRepresentationAxial->GetMinimumSliceNumber();
  unsigned int max = m_ImageRepresentationAxial->GetMaximumSliceNumber();
  unsigned int slice = static_cast< unsigned int > ( ( min + max ) / 2.0 );
  m_ImageRepresentationAxial->RequestSetSliceNumber( slice );
  m_ImageRepresentationAxial3D->RequestSetSliceNumber( slice );
  this->AxialSlider->minimum( min );
  this->AxialSlider->maximum( max );
  this->AxialSlider->value( slice );  
  this->AxialSlider->activate();
    
  min = m_ImageRepresentationSagittal->GetMinimumSliceNumber();
  max = m_ImageRepresentationSagittal->GetMaximumSliceNumber();
  slice = static_cast< unsigned int > ( ( min + max ) / 2.0 );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( slice );
  m_ImageRepresentationSagittal3D->RequestSetSliceNumber( slice );
  this->SagittalSlider->minimum( min );
  this->SagittalSlider->maximum( max );
  this->SagittalSlider->value( slice );  
  this->SagittalSlider->activate();
    
  min = m_ImageRepresentationCoronal->GetMinimumSliceNumber();
  max = m_ImageRepresentationCoronal->GetMaximumSliceNumber();
  slice = static_cast< unsigned int > ( ( min + max ) / 2.0 );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( slice );
  m_ImageRepresentationCoronal3D->RequestSetSliceNumber( slice );
  this->CoronalSlider->minimum( min );
  this->CoronalSlider->maximum( max );
  this->CoronalSlider->value( slice );  
  this->CoronalSlider->activate();
  
  this->DisplayAxial->RequestAddObject( m_ImageRepresentationAxial );
  this->DisplayAxial->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->DisplaySagittal->RequestAddObject( m_ImageRepresentationSagittal );
  this->DisplaySagittal->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->DisplayCoronal->RequestAddObject( m_ImageRepresentationCoronal );
  this->DisplayCoronal->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_CylinderRepresentation->Copy() );


  this->Display3D->RequestAddObject( m_ImageRepresentationAxial3D );
  this->Display3D->RequestAddObject( m_ImageRepresentationSagittal3D );
  this->Display3D->RequestAddObject( m_ImageRepresentationCoronal3D );
  this->Display3D->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->Display3D->RequestResetCamera();
  this->Display3D->Update();
  this->Display3D->RequestEnableInteractions();
  this->Display3D->RequestSetRefreshRate( 30 ); // 30 Hz
  this->Display3D->RequestStart();
  
  this->DisplaySagittal->RequestResetCamera();
  this->DisplaySagittal->Update();
  this->DisplaySagittal->RequestEnableInteractions();
  this->DisplaySagittal->RequestSetRefreshRate( 30 ); // 30 Hz
  this->DisplaySagittal->RequestStart();

  this->DisplayCoronal->RequestResetCamera();
  this->DisplayCoronal->Update();
  this->DisplayCoronal->RequestEnableInteractions();
  this->DisplayCoronal->RequestSetRefreshRate( 30 ); // 30 Hz
  this->DisplayCoronal->RequestStart();  

  this->DisplayAxial->RequestResetCamera();
  this->DisplayAxial->Update();
  this->DisplayAxial->RequestEnableInteractions();
  this->DisplayAxial->RequestSetRefreshRate( 30 ); // 30 Hz
  this->DisplayAxial->RequestStart();

  this->ResliceImage();  //FIXME: This is not right

}

void FourViewsTrackingWithCT::GetTrackerTransform()
{
  igstk::Transform             transitions;
  //m_Tracker->UpdateStatus();
  // Tracker should have an AddObserver method to observe the TransformModifiedEvents
  // We don't need
  m_Tracker->GetToolTransform( 3, 0, transitions ); //FIXME, Port Number
  igstkLogMacro( DEBUG, transitions.GetTranslation() << "\n" )
  // Translate this into index, and do reslcing. ...
  // FillMe
}

void FourViewsTrackingWithCT::GetLandmarkRegistrationTransform( const itk::EventObject & event )
{
  if ( TransformModifiedEvent().CheckEvent( &event ) )
    {
    TransformModifiedEvent *tmevent = ( TransformModifiedEvent *) & event;
    m_Transform = tmevent->Get();
    m_Tracker->SetPatientTransform( m_Transform );
    m_StateMachine.PushInput( m_RegistrationSuccessInput );
    }
  else
    {
    m_StateMachine.PushInput( m_RegistrationFailureInput );   // FIXME.. how to get the failure condition
    }
}

void FourViewsTrackingWithCT::DrawPickedPoint( const itk::EventObject & event)
{
  if ( TransformModifiedEvent().CheckEvent( &event ) )
    {
    TransformModifiedEvent *tmevent = ( TransformModifiedEvent *) & event;
    m_Transform = tmevent->Get();
    std::cout<< " I Picked!!\n";
    //m_StateMachine.PushInput( m_RegistrationSuccessInput );
    }
  else
    {
    //m_StateMachine.PushInput( m_RegistrationFailureInput );   // FIXME.. how to get the failure condition
    }
}
} // end of namespace
