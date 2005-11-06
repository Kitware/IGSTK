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
#include "vtkWorldPointPicker.h"
#include "vtkCommand.h"

class vtkMyCallback : public vtkCommand
  {
  public:
    static vtkMyCallback *New() 
      { return new vtkMyCallback; }
    void Delete()
      { delete this; }
    virtual void Execute(vtkObject *caller, unsigned long, void*)
      {
      vtkWorldPointPicker * picker = vtkWorldPointPicker::SafeDownCast(caller);
      }
  };

namespace igstk
{

/** Constructor: Initializes all internal variables. */
FourViewsTrackingWithCT::FourViewsTrackingWithCT():m_StateMachine(this)
{  
  /** Setup m_Logger, this logger is used to log the application class. */
  m_Logger = LoggerType::New();

  /** Setup logger, for all other igstk components. */
  logger   = LoggerType::New();

  /** Direct the application log message to the std::cout. */
  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cout );
  m_Logger->AddLogOutput( m_LogCoutOutput );

  /** Direct the igstk components log message to the file. */
  m_LogFileOutput = LogOutputType::New();
  std::string logFileName = "logFourViewsTrackingWithCT.txt";                   //FIXME. use a date/time file name
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
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

  m_LandmarkRegistrtion = RegistrationType::New();
  m_LandmarkRegistrtion->SetLogger( logger );

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

  m_PulseGenerator = PulseGenerator::New();
  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & FourViewsTrackingWithCT::GetTrackerTransform );
  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );
  m_PulseGenerator->RequestSetFrequency( 20 ); //FIXME, move to request start tracking??

  this->DisplayAxial->SetLogger( logger );
  this->DisplaySagittal->SetLogger( logger );
  this->DisplayCoronal->SetLogger( logger );

  this->DisplayAxial->RequestSetOrientation( igstk::View2D::Axial );
  this->DisplaySagittal->RequestSetOrientation( igstk::View2D::Sagittal );
  this->DisplayCoronal->RequestSetOrientation( igstk::View2D::Coronal );

  m_ImageRepresentationAxial    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal = ImageRepresentationType::New();
  m_ImageRepresentationCoronal  = ImageRepresentationType::New();

  m_ImageRepresentationAxial->SetLogger( logger );
  m_ImageRepresentationSagittal->SetLogger( logger );
  m_ImageRepresentationCoronal->SetLogger( logger );


  /** Initialize State Machine */
  m_StateMachine.AddState( m_InitialState,                "InitialState"              );
  m_StateMachine.AddState( m_PatientNameReadyState,       "PatientNameReadyState"     );
  m_StateMachine.AddState( m_ImageReadyState,             "ImageReadyState"           );
  m_StateMachine.AddState( m_PatientNameVerifiedState,    "PatientNameVerifiedState"  );
  m_StateMachine.AddState( m_ImageLandmarksReadyState,    "ImageLandmarksReadyState"  );
  m_StateMachine.AddState( m_TrackerReadyState,           "TrackerReadyState"         );
  m_StateMachine.AddState( m_TrackerLandmarksReadyState,  "TrackerLandmarksReadyState");
  m_StateMachine.AddState( m_LandmarkRegistrationReadyState, "LandmarkRegistrationReadyState" );
  m_StateMachine.AddState( m_TrackingState,               "TrackingState"             );

  m_StateMachine.AddInput( m_RequestSetPatientNameInput,        "RequestSetPatientNameInput"      );
  m_StateMachine.AddInput( m_RequestLoadImageInput,             "RequestLoadImageInput"           );
  m_StateMachine.AddInput( m_RequestSetImageLandmarksInput,     "RequestSetImageLandmarksInput"   );
  m_StateMachine.AddInput( m_RequestInitializeTrackerInput,     "RequestInitializeTrackerInput"   );
  m_StateMachine.AddInput( m_RequestSetTrackerLandmarksInput,   "RequestSetTrackerLandmarksInput" );
  m_StateMachine.AddInput( m_RequestRegistrationInput,          "RequestRegistrationInput"        );
  m_StateMachine.AddInput( m_RequestStartTrackingInput,         "RequestStartTrackingInput"       );
  m_StateMachine.AddInput( m_RequestStopTrackingInput,          "RequestStopTrackingInput"        );

  m_StateMachine.AddInput( m_PatientNameInput,                  "PatientNameInput"          );
  m_StateMachine.AddInput( m_PatientNameEmptyInput,             "PatientNameEmptyInput"     );

  m_StateMachine.AddInput( m_LoadImageSuccessInput,             "LoadImageSuccessInput"     );
  m_StateMachine.AddInput( m_LoadImageFailureInput,             "LoadImageFailureInput"     );

  m_StateMachine.AddInput( m_PatientNameMatchInput,             "PatientNameMatchInput"     );
  m_StateMachine.AddInput( m_OverwritePatientNameInput,         "OverwritePatientNameInput" );
  m_StateMachine.AddInput( m_ReloadImageInput,                  "ReloadImageInput"          );

  m_StateMachine.AddInput( m_AddImageLandmarksSuccessInput,     "AddImageLandmarksSuccessInput");
  m_StateMachine.AddInput( m_AddImageLandmarksFailureInput,     "AddImageLandmarksFailureInput");

  m_StateMachine.AddInput( m_InitializeTrackerSuccessInput,     "InitializeTrackerSuccessInput");
  m_StateMachine.AddInput( m_InitializeTrackerFailureInput,     "InitializeTrackerFailureInput");

  m_StateMachine.AddInput( m_AddTrackerLandmarksSuccessInput,   "AddTrackerLandmarksSuccessInput");
  m_StateMachine.AddInput( m_AddTrackerLandmarksFailureInput,   "AddTrackerLandmarksFailureInput");

  m_StateMachine.AddInput( m_RegistrationSuccessInput,          "RegistrationSuccessInput");
  m_StateMachine.AddInput( m_RegistrationFailureInput,          "RegistrationFailureInput");

  m_StateMachine.AddInput( m_StartTrackingSuccessInput,         "StartTrackingSuccessInput");
  m_StateMachine.AddInput( m_StartTrackingFailureInput,         "StartTrackingFailureInput");

  m_StateMachine.AddInput( m_StopTrackingSuccessInput,          "StopTrackingSuccessInput");
  m_StateMachine.AddInput( m_StopTrackingFailureInput,          "StopTrackingFailureInput");

  const ActionType NoAction = 0;
  
  m_StateMachine.AddTransition( m_InitialState, m_RequestSetPatientNameInput, m_InitialState, 
    &FourViewsTrackingWithCT::SetPatientName );
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_RequestSetPatientNameInput, m_InitialState, 
    &FourViewsTrackingWithCT::SetPatientName );
  m_StateMachine.AddTransition( m_InitialState, m_PatientNameInput, m_PatientNameReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_InitialState, m_PatientNameEmptyInput, m_InitialState, 
    NoAction );

  m_StateMachine.AddTransition( m_PatientNameReadyState, m_RequestLoadImageInput, m_PatientNameReadyState, 
    &FourViewsTrackingWithCT::LoadImage );
  m_StateMachine.AddTransition( m_ImageReadyState, m_RequestLoadImageInput, m_PatientNameReadyState, 
    &FourViewsTrackingWithCT::LoadImage );
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_LoadImageSuccessInput, m_ImageReadyState, 
    &FourViewsTrackingWithCT::VerifyPatientName );
  m_StateMachine.AddTransition( m_PatientNameReadyState, m_LoadImageFailureInput, m_PatientNameReadyState, 
    NoAction );

  m_StateMachine.AddTransition( m_ImageReadyState, m_PatientNameMatchInput, m_PatientNameVerifiedState, 
    &FourViewsTrackingWithCT::ConnectImageRepresentation );
  m_StateMachine.AddTransition( m_ImageReadyState, m_OverwritePatientNameInput, m_PatientNameVerifiedState, 
    &FourViewsTrackingWithCT::ConnectImageRepresentation );
  m_StateMachine.AddTransition( m_ImageReadyState, m_ReloadImageInput, m_PatientNameReadyState, 
    &FourViewsTrackingWithCT::RequestLoadImage );

  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_RequestInitializeTrackerInput, m_PatientNameVerifiedState, 
    &FourViewsTrackingWithCT::InitializeTracker );
  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_InitializeTrackerSuccessInput, m_TrackerReadyState, 
   NoAction );
  m_StateMachine.AddTransition( m_PatientNameVerifiedState, m_InitializeTrackerFailureInput, m_PatientNameVerifiedState, 
   NoAction );

  m_StateMachine.AddTransition( m_TrackerReadyState, m_RequestSetImageLandmarksInput, m_TrackerReadyState, 
    &FourViewsTrackingWithCT::SetImageLandmarks );
  m_StateMachine.AddTransition( m_ImageLandmarksReadyState, m_RequestSetImageLandmarksInput, m_TrackerReadyState, 
    &FourViewsTrackingWithCT::SetImageLandmarks );
  m_StateMachine.AddTransition( m_TrackerReadyState, m_AddImageLandmarksSuccessInput, m_ImageLandmarksReadyState, 
    NoAction );
  m_StateMachine.AddTransition( m_TrackerReadyState, m_AddImageLandmarksFailureInput, m_TrackerReadyState, 
    NoAction );
  
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
  m_Tracker->AttachSROMFileNameToPort( 3, "C:/Patrick/Vicra/Tookit/Tool Definition Files/8700339.rom" ); //FIXME
  m_Tracker->Initialize();  //FIXME, how to check if this is success???

  m_StateMachine.PushInputBoolean( m_Tracker->GetNumberOfTools(), m_InitializeTrackerSuccessInput, m_InitializeTrackerFailureInput );
}

void FourViewsTrackingWithCT::RequestSetImageLandmarks()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestSetImageLandmarks called ... \n")
  m_StateMachine.PushInput( m_RequestSetImageLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::SetImageLandmarks()
{
  igstkLogMacro2( logger, DEBUG, "FourViewsTrackingWithCT::RequestSetImageLandmarks called ... \n")
  m_StateMachine.PushInput( m_RequestSetImageLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::RequestSetTrackerLandmarks()
{
}
void FourViewsTrackingWithCT::RequestRegistration()
{
}
void FourViewsTrackingWithCT::RequestStartTracking()
{
  m_Tracker->StartTracking();
  m_PulseGenerator->RequestStart();
}
void FourViewsTrackingWithCT::RequestStopTracking()
{
  m_PulseGenerator->RequestStop();
  m_Tracker->StopTracking();
}
void FourViewsTrackingWithCT::GetTrackerTransform()
{
  Transform             transitions;
  m_Tracker->UpdateStatus();
  m_Tracker->GetToolTransform( 3, 0, transitions ); //FIXME, Port Number
  igstkLogMacro( DEBUG, transitions.GetTranslation() << "\n" )
}

void FourViewsTrackingWithCT::RequestResliceImage()
{
  m_SliceNumberToBeSet[0] = this->AxialSlider->value();
  m_SliceNumberToBeSet[1] = this->SagittalSlider->value();
  m_SliceNumberToBeSet[2] = this->CoronalSlider->value();

  // FIXME.
  m_SliceNumber[0] = m_SliceNumberToBeSet[0];
  m_SliceNumber[1] = m_SliceNumberToBeSet[1];
  m_SliceNumber[2] = m_SliceNumberToBeSet[2];

  m_ImageRepresentationAxial->RequestSetSliceNumber( m_SliceNumber[0] );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( m_SliceNumber[1] );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( m_SliceNumber[2] );
}


/** This method should be invoked by the State Machine 
 *  only when the Image has been loaded and the Patient
 *  name has been verified */
void FourViewsTrackingWithCT::ConnectImageRepresentation()
{

  m_ImageRepresentationAxial->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
  m_ImageRepresentationSagittal->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );
  m_ImageRepresentationCoronal->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );

  m_ImageRepresentationAxial->RequestSetOrientation( ImageRepresentationType::Axial );
  m_ImageRepresentationSagittal->RequestSetOrientation( ImageRepresentationType::Sagittal );
  m_ImageRepresentationCoronal->RequestSetOrientation( ImageRepresentationType::Coronal );

  this->DisplayAxial->RequestAddObject( m_ImageRepresentationAxial );
  this->DisplaySagittal->RequestAddObject( m_ImageRepresentationSagittal );
  this->DisplayCoronal->RequestAddObject( m_ImageRepresentationCoronal );

  this->DisplayAxial->RequestResetCamera();
  this->DisplayAxial->Update();
  this->DisplayAxial->RequestEnableInteractions();
  this->DisplayAxial->RequestSetRefreshRate( 30 ); // 30 Hz
  this->DisplayAxial->RequestStart();

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

  /** Initialize the slider */
  //int ext[6];
  //->GetExtent( ext );
  //this->AxialSlider->maximum( ext[5] );
  //this->AxialSlider->minimum( ext[4] );
  //this->AxialSlider->value( (int) (ext[4]+ext[5])/2 );

}


} // end of namespace
