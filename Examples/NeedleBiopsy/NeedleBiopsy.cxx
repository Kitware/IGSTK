/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    NeedleBiopsy.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "NeedleBiopsy.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"
#include "igstkEvents.h"


/** Constructor: Initializes all internal variables. */
NeedleBiopsy::NeedleBiopsy():m_StateMachine(this)
{  
  /** Setup logger, for all other igstk components. */
  m_Logger   = LoggerType::New();

  /** Direct the application log message to the std::cout 
    * and FLTK text display */
  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );
  //logger->AddLogOutput( m_LogCoutOutput );

  Fl_Text_Buffer * textBuffer = new Fl_Text_Buffer();
  this->m_LogWindow->buffer( textBuffer );
  m_LogFLTKOutput = igstk::FLTKTextLogOutput::New();
  m_LogFLTKOutput->SetStream( *m_LogWindow );
  this->GetLogger()->AddLogOutput( m_LogFLTKOutput );

  /** Direct the igstk components log message to the file. */
  m_LogFileOutput = LogOutputType::New();
  //FIXME. use a date/time file name
  std::string logFileName = "logNeedleBiopsy.txt";
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    m_Logger->AddLogOutput( m_LogFileOutput );
    }
  else
    {
    //FIXME. should we return or not
    igstkLogMacro( DEBUG, "Problem opening Log file: " << logFileName << "\n" );
    return;
    }

  /** Initialize all member variables and set logger */
  m_ImageReader = ImageReaderType::New();
  m_CTImageObserver = CTImageObserver::New();
  m_ImageReader->AddObserver(igstk::CTImageReader::ImageModifiedEvent(),
                             m_CTImageObserver);

  m_LandmarkRegistration = RegistrationType::New();

  m_LandmarkRegistrationObserver = ObserverType2::New();
  m_LandmarkRegistrationObserver->SetCallbackFunction( this, 
                              &NeedleBiopsy::GetLandmarkRegistrationTransform );
  m_LandmarkRegistration->AddObserver( igstk::TransformModifiedEvent(), 
                                               m_LandmarkRegistrationObserver );
 
  m_LandmarkRegistrationRMSErrorObserver = ObserverType2::New();
  m_LandmarkRegistrationRMSErrorObserver->SetCallbackFunction( this, 
                              &NeedleBiopsy::GetLandmarkRegistrationRMSError );
  m_LandmarkRegistration->AddObserver( igstk::DoubleTypeEvent(), 
                                               m_LandmarkRegistrationRMSErrorObserver );
 
  //Initialize the registration RMS error 
  m_LandmarkRegistrationRMSError = 0.0;

  m_Annotation2D = igstk::Annotation2D::New();
  
  igstk::Transform transform;
  transform.SetToIdentity( 100 );

  m_PickedPoint                   = EllipsoidType::New();
  m_PickedPoint->RequestSetTransform( transform );
  m_PickedPointRepresentation     = EllipsoidRepresentationType::New();
  //m_PickedPointRepresentation->SetLogger( logger );
  m_PickedPoint->SetRadius( 5, 5, 5 );  
  m_PickedPointRepresentation->RequestSetEllipsoidObject( m_PickedPoint );
  m_PickedPointRepresentation->SetColor(1.0,1.0,0.0);
  m_PickedPointRepresentation->SetOpacity(1.0);

  m_NeedleTip                   = EllipsoidType::New();
  m_NeedleTip->RequestSetTransform( transform );
  m_NeedleTipRepresentation     = EllipsoidRepresentationType::New();
  m_NeedleTip->SetRadius( 5, 5, 5 );  
  m_NeedleTipRepresentation->RequestSetEllipsoidObject( m_NeedleTip );
  m_NeedleTipRepresentation->SetColor(1.0,0.0,0.0);
  m_NeedleTipRepresentation->SetOpacity(1.0);

  m_Needle                    = CylinderType::New();
  m_Needle->RequestSetTransform( transform );
  m_NeedleRepresentation      = CylinderRepresentationType::New();
  m_Needle->SetRadius( 1.5 );   //   1.5 mm
  m_Needle->SetHeight( 100 );   // 200.0 mm
  m_NeedleRepresentation->RequestSetCylinderObject( m_Needle );
  m_NeedleRepresentation->SetColor(0.0,1.0,0.0);
  m_NeedleRepresentation->SetOpacity(1.0);

  m_TargetPoint                 = EllipsoidType::New();
  m_TargetPoint->RequestSetTransform( transform );
  m_TargetRepresentation        = EllipsoidRepresentationType::New();
  m_TargetPoint->SetRadius( 6, 6, 6 );
  m_TargetRepresentation->RequestSetEllipsoidObject( m_TargetPoint );
  m_TargetRepresentation->SetColor( 1.0, 0.0, 0.0);
  m_TargetRepresentation->SetOpacity( 0.6 );
 
  m_EntryPoint                  = EllipsoidType::New();
  m_EntryPoint->RequestSetTransform( transform );
  m_EntryRepresentation         = EllipsoidRepresentationType::New();
  m_EntryPoint->SetRadius( 6, 6, 6 );
  m_EntryRepresentation->RequestSetEllipsoidObject( m_EntryPoint );
  m_EntryRepresentation->SetColor( 0.0, 0.0, 1.0);
  m_EntryRepresentation->SetOpacity( 0.6 );

  m_Path                       = PathType::New();
  TubePointType p;
  p.SetPosition( 0, 0, 0);
  p.SetRadius( 2 );
  m_Path->AddPoint( p );
  m_Path->AddPoint( p );

  m_PathRepresentationAxial         = PathRepresentationType::New();
  m_PathRepresentationAxial->RequestSetTubeObject( m_Path );
  m_PathRepresentationAxial->SetColor( 0.0, 1.0, 0.0);
  m_PathRepresentationAxial->SetOpacity( 0.4 );

  m_PathRepresentationSagittal         = PathRepresentationType::New();
  m_PathRepresentationSagittal->RequestSetTubeObject( m_Path );
  m_PathRepresentationSagittal->SetColor( 0.0, 1.0, 0.0);
  m_PathRepresentationSagittal->SetOpacity( 0.4 );

  m_PathRepresentationCoronal         = PathRepresentationType::New();
  m_PathRepresentationCoronal->RequestSetTubeObject( m_Path );
  m_PathRepresentationCoronal->SetColor( 0.0, 1.0, 0.0);
  m_PathRepresentationCoronal->SetOpacity( 0.4 );

  m_PathRepresentation3D         = PathRepresentationType::New();
  m_PathRepresentation3D->RequestSetTubeObject( m_Path );
  m_PathRepresentation3D->SetColor( 0.0, 1.0, 0.0);
  m_PathRepresentation3D->SetOpacity( 0.4 );

  m_ImageToTrackerTransform.SetToIdentity( -1 );
  m_ImageLandmarkTransform.SetToIdentity( -1 );
  m_TrackerLandmarkTransform.SetToIdentity( -1 );

  m_PulseGenerator = igstk::PulseGenerator::New();
  m_Observer = ObserverType::New();
  m_Observer->SetCallbackFunction( this, & NeedleBiopsy::Tracking );
  m_PulseGenerator->AddObserver( igstk::PulseEvent(), m_Observer );
  m_PulseGenerator->RequestSetFrequency( 30 ); 

  m_ViewPickerObserver = ObserverType2::New();
  m_ViewPickerObserver->SetCallbackFunction( this, 
                                               &NeedleBiopsy::DrawPickedPoint );

  this->DisplayAxial->RequestSetOrientation( igstk::View2D::Axial );
  this->DisplaySagittal->RequestSetOrientation( igstk::View2D::Sagittal );
  this->DisplayCoronal->RequestSetOrientation( igstk::View2D::Coronal );

  m_ImageRepresentationAxial    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal = ImageRepresentationType::New();
  m_ImageRepresentationCoronal  = ImageRepresentationType::New();
  m_ImageRepresentationAxial3D    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal3D = ImageRepresentationType::New();
  m_ImageRepresentationCoronal3D  = ImageRepresentationType::New();

  /** Set logger */
  if(0) // Temporary disable this logger
    {
    m_ImageReader->SetLogger( m_Logger );
    m_Tracker->SetLogger( m_Logger );
    m_LandmarkRegistration->SetLogger( m_Logger );
    m_SerialCommunication->SetLogger( m_Logger );  
    this->DisplayAxial->SetLogger( m_Logger );
    this->DisplaySagittal->SetLogger( m_Logger );
    this->DisplayCoronal->SetLogger( m_Logger );
    this->Display3D->SetLogger( m_Logger );
    m_ImageRepresentationAxial->SetLogger( m_Logger );
    m_ImageRepresentationSagittal->SetLogger( m_Logger );
    m_ImageRepresentationCoronal->SetLogger( m_Logger );
    m_ImageRepresentationAxial3D->SetLogger( m_Logger );
    m_ImageRepresentationSagittal3D->SetLogger( m_Logger );
    m_ImageRepresentationCoronal3D->SetLogger( m_Logger );
    }

  this->ObserveAxialBoundsInput(    m_ImageRepresentationAxial    );
  this->ObserveSagittalBoundsInput( m_ImageRepresentationSagittal );
  this->ObserveCoronalBoundsInput(  m_ImageRepresentationCoronal  );

  /** Initialize  Machine */
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( WaitingForPatientName      );
  igstkAddStateMacro( PatientNameReady           );
  igstkAddStateMacro( WaitingForDICOMDirectory   );
  igstkAddStateMacro( ImageReady                 );
  igstkAddStateMacro( PatientNameVerified        );
  igstkAddStateMacro( AddingImageLandmark        );
  igstkAddStateMacro( ImageLandmarksReady        );
  igstkAddStateMacro( AttemptingInitializeTracker);
  igstkAddStateMacro( TrackerReady               );  
  igstkAddStateMacro( AddingTrackerLandmark      );
  igstkAddStateMacro( TrackerLandmarksReady      );
  igstkAddStateMacro( AttemptingRegistration     );
  igstkAddStateMacro( EvaluatingRegistrationError);
  igstkAddStateMacro( LandmarkRegistrationReady  );
  igstkAddStateMacro( TargetPointReady           );
  igstkAddStateMacro( PathReady                  );
  igstkAddStateMacro( AttemptingStartTracking    );
  igstkAddStateMacro( Tracking                   );
  igstkAddStateMacro( AttemptingStopTracking     );


  igstkAddInputMacro( RequestSetPatientName        );
  igstkAddInputMacro( PatientName                  );
  igstkAddInputMacro( PatientNameEmpty             );

  igstkAddInputMacro( RequestLoadImage             );
  igstkAddInputMacro( LoadImageSuccess             );
  igstkAddInputMacro( LoadImageFailure             );
  igstkAddInputMacro( PatientNameMatch             );
  igstkAddInputMacro( OverwritePatientName         );
  igstkAddInputMacro( ReloadImage                  );

  igstkAddInputMacro( RequestAddImageLandmark      );
  igstkAddInputMacro( NeedMoreLandmarkPoints       );
  igstkAddInputMacro( EnoughLandmarkPoints         );
  igstkAddInputMacro( RequestClearImageLandmarks   );
  
  igstkAddInputMacro( RequestInitializeTracker     );
  igstkAddInputMacro( InitializeTrackerSuccess     );
  igstkAddInputMacro( InitializeTrackerFailure     );
  
  igstkAddInputMacro( RequestAddTrackerLandmark    );
  igstkAddInputMacro( RequestClearTrackerLandmarks );

  igstkAddInputMacro( RequestRegistration          );
  igstkAddInputMacro( RegistrationSuccess          );
  igstkAddInputMacro( RegistrationFailure          );
  igstkAddInputMacro( RegistrationErrorAccepted    );
  igstkAddInputMacro( RegistrationErrorRejected    );

  igstkAddInputMacro( RequestSetTargetPoint        );
  igstkAddInputMacro( RequestSetEntryPoint         );
  
  igstkAddInputMacro( RequestStartTracking         );
  igstkAddInputMacro( StartTrackingSuccess         );
  igstkAddInputMacro( StartTrackingFailure         );

  igstkAddInputMacro( RequestStopTracking          );
  igstkAddInputMacro( StopTrackingSuccess          );
  igstkAddInputMacro( StopTrackingFailure          );

  igstkAddInputMacro( AxialBounds                  );
  igstkAddInputMacro( SagittalBounds               );
  igstkAddInputMacro( CoronalBounds                );

  /** Register patient name */
  igstkAddTransitionMacro( Initial, RequestSetPatientName, 
                                        WaitingForPatientName, SetPatientName );
  igstkAddTransitionMacro( PatientNameReady, RequestSetPatientName, 
                                        WaitingForPatientName, SetPatientName );
  igstkAddTransitionMacro( WaitingForPatientName, PatientName, 
                                                         PatientNameReady, No );
  igstkAddTransitionMacro( WaitingForPatientName, PatientNameEmpty, 
                                                                  Initial, No );

  /** Load image and verify patient name */
  igstkAddTransitionMacro( PatientNameReady, RequestLoadImage, 
                                          WaitingForDICOMDirectory, LoadImage );
  igstkAddTransitionMacro( PatientNameVerified, RequestLoadImage, 
                                          WaitingForDICOMDirectory, LoadImage );
  igstkAddTransitionMacro( WaitingForDICOMDirectory, LoadImageSuccess, 
                                                ImageReady, VerifyPatientName );
  igstkAddTransitionMacro( WaitingForDICOMDirectory, LoadImageFailure, 
                                                         PatientNameReady, No );

  /** Display image */
  igstkAddTransitionMacro( ImageReady, PatientNameMatch, 
                              PatientNameVerified, ConnectImageRepresentation );
  igstkAddTransitionMacro( ImageReady, OverwritePatientName, 
                              PatientNameVerified, ConnectImageRepresentation );
  igstkAddTransitionMacro( ImageReady, ReloadImage, 
                                          WaitingForDICOMDirectory, LoadImage );

  /** Receive Slice Information from the Image Representation */
  igstkAddTransitionMacro( PatientNameVerified, AxialBounds, 
                                    PatientNameVerified, SetAxialSliderBounds );
  igstkAddTransitionMacro( PatientNameVerified, SagittalBounds, 
                                 PatientNameVerified, SetSagittalSliderBounds );
  igstkAddTransitionMacro( PatientNameVerified, CoronalBounds, 
                                  PatientNameVerified, SetCoronalSliderBounds );
  
  /** Set image landmarks, any number of landmarks >= 3 */
  igstkAddTransitionMacro( PatientNameVerified, RequestAddImageLandmark, 
                                        AddingImageLandmark, AddImageLandmark );
  igstkAddTransitionMacro( AddingImageLandmark, RequestAddImageLandmark, 
                                        AddingImageLandmark, AddImageLandmark );
  igstkAddTransitionMacro( ImageLandmarksReady, RequestAddImageLandmark, 
                                        ImageLandmarksReady, AddImageLandmark );
  igstkAddTransitionMacro( AddingImageLandmark, NeedMoreLandmarkPoints, 
                                                      AddingImageLandmark, No );
  igstkAddTransitionMacro( AddingImageLandmark, EnoughLandmarkPoints, 
                                                      ImageLandmarksReady, No );
  igstkAddTransitionMacro( ImageLandmarksReady, EnoughLandmarkPoints, 
                                                      ImageLandmarksReady, No );

  /** Clear image landmarks */
  igstkAddTransitionMacro( AddingImageLandmark, RequestClearImageLandmarks, 
                                     PatientNameVerified, ClearImageLandmarks );
  igstkAddTransitionMacro( ImageLandmarksReady, RequestClearImageLandmarks, 
                                     PatientNameVerified, ClearImageLandmarks );

  /** Initialize tracker */
  igstkAddTransitionMacro( ImageLandmarksReady, RequestInitializeTracker, 
                               AttemptingInitializeTracker, InitializeTracker );
  igstkAddTransitionMacro( AttemptingInitializeTracker, 
                           InitializeTrackerSuccess, 
                           TrackerReady, No );
  igstkAddTransitionMacro( AttemptingInitializeTracker, 
                           InitializeTrackerFailure, 
                           ImageLandmarksReady, No );

  /** Set tracker landmarks, require same number of landmarks as in 
    * image landmark */
  igstkAddTransitionMacro( TrackerReady, RequestAddTrackerLandmark, 
                                    AddingTrackerLandmark, AddTrackerLandmark );
  igstkAddTransitionMacro( AddingTrackerLandmark, RequestAddTrackerLandmark, 
                                    AddingTrackerLandmark, AddTrackerLandmark );
  igstkAddTransitionMacro( AddingTrackerLandmark, NeedMoreLandmarkPoints, 
                                                    AddingTrackerLandmark, No );
  igstkAddTransitionMacro( AddingTrackerLandmark, EnoughLandmarkPoints, 
                                                    TrackerLandmarksReady, No );
  /** Clear tracker landmarks */
  igstkAddTransitionMacro( AddingTrackerLandmark, RequestClearTrackerLandmarks, 
                                          TrackerReady, ClearTrackerLandmarks );
  igstkAddTransitionMacro( TrackerLandmarksReady, RequestClearTrackerLandmarks, 
                                          TrackerReady, ClearTrackerLandmarks );

  /** Registration */
  igstkAddTransitionMacro( TrackerLandmarksReady, RequestRegistration, 
                                         AttemptingRegistration, Registration );
  igstkAddTransitionMacro( AttemptingRegistration, RegistrationSuccess, 
                     EvaluatingRegistrationError, EvaluatingRegistrationError );
  igstkAddTransitionMacro( AttemptingRegistration, RegistrationFailure, 
                                                    TrackerLandmarksReady, No );

  igstkAddTransitionMacro( EvaluatingRegistrationError, 
                     RegistrationErrorAccepted, LandmarkRegistrationReady, No );
  igstkAddTransitionMacro( EvaluatingRegistrationError, 
          RegistrationErrorRejected, TrackerLandmarksReady, ResetRegistration );

  /** Path Planning */
  igstkAddTransitionMacro( LandmarkRegistrationReady, RequestSetTargetPoint, 
                                            TargetPointReady, DrawTargetPoint );
  igstkAddTransitionMacro( TargetPointReady, RequestSetTargetPoint, 
                                            TargetPointReady, DrawTargetPoint );
  
  igstkAddTransitionMacro( TargetPointReady, RequestSetEntryPoint, 
                                                          PathReady, DrawPath );
  igstkAddTransitionMacro( PathReady, RequestSetTargetPoint, 
                                                          PathReady, DrawPath );
  igstkAddTransitionMacro( PathReady, RequestSetEntryPoint, 
                                                          PathReady, DrawPath );

  /** Tracking */
  igstkAddTransitionMacro( PathReady, RequestStartTracking, 
                                       AttemptingStartTracking, StartTracking );
  igstkAddTransitionMacro( AttemptingStartTracking, StartTrackingSuccess, 
                                                                 Tracking, No );
  igstkAddTransitionMacro( AttemptingStartTracking, StartTrackingFailure, 
                                                                PathReady, No );

  igstkAddTransitionMacro( Tracking, RequestStopTracking, 
                                         AttemptingStopTracking, StopTracking );
  igstkAddTransitionMacro( AttemptingStopTracking, StopTrackingSuccess, 
                                                                PathReady, No );
  igstkAddTransitionMacro( AttemptingStopTracking, StopTrackingFailure, 
                                                                 Tracking, No );
  
  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();

  std::ofstream ofile;
  ofile.open("DemoApplicationStateMachineDiagram.dot");
 
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );

  ofile.close();

}

/** Destructor */
NeedleBiopsy::~NeedleBiopsy()
{
 
}

/** Method to be invoked when no operation is required */
void NeedleBiopsy::NoProcessing()
{
}


void NeedleBiopsy::RequestSetPatientName()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                          "NeedleBiopsy::RequestSetPatientName called...\n" )
  m_StateMachine.PushInput( m_RequestSetPatientNameInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::SetPatientNameProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "NeedleBiopsy::GetPatientName called...\n" )
  const char *patientName = fl_input("Patient Name:", "");
  if( patientName != NULL )
    {
    m_PatientName = patientName;
    igstkLogMacro( DEBUG, "Patient registered as: "<< m_PatientName <<"\n" )
    igstkLogMacro2( m_Logger, DEBUG, 
                              "Patient registered as: "<< m_PatientName <<"\n" )
    m_StateMachine.PushInput( m_PatientNameInput );
    }
  else
    {
    m_StateMachine.PushInput( m_PatientNameEmptyInput );
    }
}


void NeedleBiopsy::RequestLoadImage()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                        "NeedleBiopsy::RequestLoadImageProcessing called...\n" )
  m_StateMachine.PushInput( m_RequestLoadImageInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::LoadImageProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                               "NeedleBiopsy::LoadImageProcessing called...\n" )
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  if ( directoryname != NULL )
    {
    igstkLogMacro( DEBUG, 
                      "Set ImageReader directory: " << directoryname << "\n" )
    igstkLogMacro2( m_Logger, DEBUG, 
                            "m_ImageReader->RequestSetDirectory called...\n" )
    m_ImageReader->RequestSetDirectory( directoryname ); 

    igstkLogMacro( DEBUG, "ImageReader loading images... \n" )
    igstkLogMacro2( m_Logger, DEBUG, 
                              "m_ImageReader->RequestReadImage called... \n" )
    m_ImageReader->RequestReadImage();

    m_ImageReader->RequestGetImage();

    if(!m_CTImageObserver->GotCTImage())
      {
      igstkLogMacro(          DEBUG, "Cannot read image\n" )
      igstkLogMacro2( m_Logger, DEBUG, "Cannot read image\n" )
      m_StateMachine.PushInput( m_LoadImageFailureInput);
      }
    else
      {
      igstkLogMacro(          DEBUG, "Image Loaded...\n" )
      igstkLogMacro2( m_Logger, DEBUG, "Image Loaded...\n" )
      m_ImageSpatialObject = m_CTImageObserver->GetCTImage();
      m_StateMachine.PushInput( m_LoadImageSuccessInput);
      }
    }
  else
    {
    igstkLogMacro(          DEBUG, "No directory is selected\n" )
    igstkLogMacro2( m_Logger, DEBUG, "No directory is selected\n" )
    m_StateMachine.PushInput( m_LoadImageFailureInput );
    }
}

void NeedleBiopsy::VerifyPatientNameProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "NeedleBiopsy::VerifyPatientNameProcessing called ... \n")
  if ( m_ImageReader->GetPatientName() == m_PatientName )
    {
    igstkLogMacro( DEBUG, 
            "Registered patient name match with the name in loaded image \n" )
    igstkLogMacro2( m_Logger, DEBUG, 
            "Registered patient name match with the name in loaded image \n" )
    m_StateMachine.PushInput( m_PatientNameMatchInput );
    }
  else
    {
    igstkLogMacro (         DEBUG, "Patient name mismatch\n" )
    igstkLogMacro2( m_Logger, DEBUG, "Patient name mismatch\n" )
      std::string msg = "Patient Registered as: " + m_PatientName + "\n";
    msg += "Image has the name of: " + m_ImageReader->GetPatientName() +"\n";
    msg += "Name mismatch!!!!\n";
    msg += "Do you want to overwrite the name?\n";
    int i = fl_choice( msg.c_str(), NULL, "Yes", "No");
    if ( i == 1 )
      {
        m_PatientName = m_ImageReader->GetPatientName();
        igstkLogMacro( DEBUG, 
                  "Patient name is overwritten to:" << m_PatientName << "\n" )
        igstkLogMacro2( m_Logger, DEBUG, 
                  "Patient name is overwritten to:" << m_PatientName << "\n" )
        m_StateMachine.PushInput( m_OverwritePatientNameInput );
      }
    else
      {
        igstkLogMacro (         DEBUG, "Load another image\n" )
        igstkLogMacro2( m_Logger, DEBUG, "Load another image\n" )
        m_StateMachine.PushInput( m_ReloadImageInput );
      
      }
    }
}

void NeedleBiopsy::RequestInitializeTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                      "NeedleBiopsy::RequestInitializeTracker called ... \n" )
  m_StateMachine.PushInput( m_RequestInitializeTrackerInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::InitializeTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                   "NeedleBiopsy::InitializeTrackerProcessing called ... \n" )
  
  m_SerialCommunication = CommunicationType::New();  
  switch( SerialPort->value() ) 
    {
    case 0 : 
      m_SerialCommunication->SetPortNumber( 
                                      igstk::SerialCommunication::PortNumber0 );
      break;
    case 1 : 
      m_SerialCommunication->SetPortNumber( 
                                      igstk::SerialCommunication::PortNumber1 );
      break;
    case 2 : 
      m_SerialCommunication->SetPortNumber( 
                                      igstk::SerialCommunication::PortNumber2 );
      break;
    case 3 : 
      m_SerialCommunication->SetPortNumber( 
                                      igstk::SerialCommunication::PortNumber3 );
      break;
    default:
      m_SerialCommunication->SetPortNumber( 
                                      igstk::SerialCommunication::PortNumber0 );
    }
  
  m_SerialCommunication->SetParity( igstk::SerialCommunication::NoParity );
  m_SerialCommunication->SetBaudRate(igstk::SerialCommunication::BaudRate9600);
  m_SerialCommunication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  m_SerialCommunication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  m_SerialCommunication->SetHardwareHandshake( 
                                      igstk::SerialCommunication::HandshakeOff);
  m_SerialCommunication->OpenCommunication();

  m_Tracker = TrackerType::New();
  m_Tracker->SetCommunication( m_SerialCommunication );

  /** Tool calibration transform */
  igstk::Transform toolCalibrationTransform;

  igstk::Transform::VectorType translation;
  translation[0] = OffsetX->value();   // Tip offset
  translation[1] = OffsetY->value();
  translation[2] = OffsetZ->value();

  igstk::Transform::VersorType rotation;
  rotation.SetIdentity();

  toolCalibrationTransform.SetTranslationAndRotation( translation, 
                                                             rotation, 0.1, -1);
  m_Tracker->SetToolCalibrationTransform( TrackerToolPort->value(), 
                                                   0, toolCalibrationTransform);
  m_Tracker->RequestOpen();
  m_Tracker->AttachSROMFileNameToPort( TrackerToolPort->value(), 
                                                 TrackerToolSROMFile->value() );
  m_Tracker->AttachSROMFileNameToPort( ReferenceToolPort->value(), 
                                               ReferenceToolSROMFile->value() );
  m_Tracker->SetReferenceTool( UseReferenceTool->value(), 
                                                 ReferenceToolPort->value(), 0);
  m_Tracker->RequestInitialize();
  m_Tracker->RequestStartTracking();
  m_StateMachine.PushInputBoolean( m_Tracker->GetNumberOfTools(),
             m_InitializeTrackerSuccessInput, m_InitializeTrackerFailureInput );
}

void NeedleBiopsy::RequestAddImageLandmark()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                       "NeedleBiopsy::RequestAddImageLandmark called ... \n" )
  m_StateMachine.PushInput( m_RequestAddImageLandmarkInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::AddImageLandmarkProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "NeedleBiopsy::AddImageLandmarkProcessing called ... \n" )

  /** Check if there is an updated image landmark picking point */
  if ( m_ImageLandmarkTransform.GetTranslation() != 
                          m_ImageLandmarkTransformToBeSet.GetTranslation() )
    {
    LandmarkPointType  p;
    p[0] = m_ImageLandmarkTransformToBeSet.GetTranslation()[0];
    p[1] = m_ImageLandmarkTransformToBeSet.GetTranslation()[1];
    p[2] = m_ImageLandmarkTransformToBeSet.GetTranslation()[2];
    m_ImageLandmarksContainer.push_back( p );

    this->NumberOfImageLandmarks->value( m_ImageLandmarksContainer.size() );
    if ( m_ImageLandmarksContainer.size() < 3 )
      {
      this->NumberOfImageLandmarks->textcolor( FL_BLACK );
      }
    else
      {
      this->NumberOfImageLandmarks->textcolor( FL_BLUE );
      }
      
    m_ImageLandmarkTransform.SetTranslation( 
               m_ImageLandmarkTransformToBeSet.GetTranslation(), 0.1, 10000 );
    igstkLogMacro( DEBUG, "Image landmark point added: "<< p << "\n" )
    igstkLogMacro2( m_Logger, DEBUG, "Image landmark point added:"<< p << "\n" )
    }
  else
    {
    igstkLogMacro(          DEBUG, "No new image landmark point picked.\n" )
    igstkLogMacro2( m_Logger, DEBUG, "No new image landmark point picked.\n" )
    }

  m_StateMachine.PushInputBoolean( (m_ImageLandmarksContainer.size()>=3), 
                  m_EnoughLandmarkPointsInput, m_NeedMoreLandmarkPointsInput);
}

void NeedleBiopsy::RequestClearImageLandmarks()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                     "NeedleBiopsy::RequestClearImageLandmarks called ... \n")
  m_StateMachine.PushInput( m_RequestClearImageLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::ClearImageLandmarksProcessing()
{
  igstkLogMacro(          DEBUG, "Image landmark points cleared...\n" )
  igstkLogMacro2( m_Logger, DEBUG, "Image landmark points cleared...\n" )
  m_ImageLandmarksContainer.clear();
  this->NumberOfImageLandmarks->value( 0 );
  this->NumberOfImageLandmarks->textcolor( FL_BLACK );
  m_ImageLandmarkTransform.SetTranslation( 
                 m_ImageLandmarkTransformToBeSet.GetTranslation(), 0.1, 10000 );
}

void NeedleBiopsy::RequestAddTrackerLandmark()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                        "NeedleBiopsy::RequestAddTrackerLandmark called ... \n")
  m_StateMachine.PushInput( m_RequestAddTrackerLandmarkInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::AddTrackerLandmarkProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                     "NeedleBiopsy::AddTrackerLandmarkProcessing called ... \n")
  
  this->GetTrackerTransform();
  
  if ( m_TrackerLandmarkTransform.GetTranslation() != 
                             m_TrackerLandmarkTransformToBeSet.GetTranslation())
    {

    LandmarkPointType  p;
    p[0] = m_TrackerLandmarkTransformToBeSet.GetTranslation()[0];
    p[1] = m_TrackerLandmarkTransformToBeSet.GetTranslation()[1];
    p[2] = m_TrackerLandmarkTransformToBeSet.GetTranslation()[2];
    m_TrackerLandmarksContainer.push_back( p );

    this->NumberOfTrackerLandmarks->value( m_TrackerLandmarksContainer.size() );
    if ( m_TrackerLandmarksContainer.size() < m_ImageLandmarksContainer.size() )
      {
      this->NumberOfTrackerLandmarks->textcolor( FL_BLACK );
      }
    else
      {
      this->NumberOfTrackerLandmarks->textcolor( FL_BLUE );
      }

    m_TrackerLandmarkTransform.SetTranslation( 
               m_TrackerLandmarkTransformToBeSet.GetTranslation(), 0.1, 10000 );
    igstkLogMacro( DEBUG, "Tracker landmark point added: "<< p << "\n" )
    igstkLogMacro2( m_Logger, DEBUG, 
                    "Tracker landmark point added:" << p << "\n" )

    }
  else
    {
    igstkLogMacro(          DEBUG, "No new tracker landmark point reading.\n" )
    igstkLogMacro2( m_Logger, DEBUG, 
                    "No new tracker landmark point reading.\n" )
    }  

  m_StateMachine.PushInputBoolean( 
      ( m_TrackerLandmarksContainer.size() < m_ImageLandmarksContainer.size() ),
                   m_NeedMoreLandmarkPointsInput, m_EnoughLandmarkPointsInput );
}

void NeedleBiopsy::GetTrackerTransform()
{
  igstkLogMacro2( m_Logger, DEBUG, "Tracker::GetToolTransform called...\n" )
  m_Tracker->RequestUpdateStatus();
  m_Tracker->GetToolTransform( 
               TrackerToolPort->value(), 0, m_TrackerLandmarkTransformToBeSet );
}

void NeedleBiopsy::RequestClearTrackerLandmarks()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                     "NeedleBiopsy::RequestClearTrackerLandmarks called ... \n")
  m_StateMachine.PushInput( m_RequestClearTrackerLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::ClearTrackerLandmarksProcessing()
{
  igstkLogMacro(          DEBUG, "Tracker landmark points cleared...\n" )
  igstkLogMacro2( m_Logger, DEBUG, "Tracker landmark points cleared...\n" )
  m_TrackerLandmarksContainer.clear();
  this->NumberOfTrackerLandmarks->value( 0 );
  this->NumberOfTrackerLandmarks->textcolor( FL_BLACK );
  m_TrackerLandmarkTransform.SetTranslation( 
               m_TrackerLandmarkTransformToBeSet.GetTranslation(), 0.1, 10000 );
}

void NeedleBiopsy::RequestRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                             "NeedleBiopsy::RequestRegistration called ... \n" )
  m_StateMachine.PushInput( m_RequestRegistrationInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::RegistrationProcessing()
{
  LandmarkPointContainerType::iterator it1, it2;
  for( it1 = m_ImageLandmarksContainer.begin(), 
       it2 = m_TrackerLandmarksContainer.begin(); 
       it1 != m_ImageLandmarksContainer.end(), 
       it2 != m_TrackerLandmarksContainer.end(); 
       it1 ++ , it2 ++ )
    {
    m_LandmarkRegistration->RequestAddImageLandmarkPoint( *it1);
    m_LandmarkRegistration->RequestAddTrackerLandmarkPoint( *it2 );  
    }
  m_LandmarkRegistration->RequestComputeTransform();
  m_LandmarkRegistration->RequestGetRMSError();
  m_LandmarkRegistration->RequestGetTransform();
}

void NeedleBiopsy::RequestStartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                            "NeedleBiopsy::RequestStartTracking called ... \n" )
  m_StateMachine.PushInput( m_RequestStartTrackingInput );
  m_StateMachine.ProcessInputs();
}
void NeedleBiopsy::StartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                         "NeedleBiopsy::StartTrackingProcessing called ... \n" )
  m_Tracker->AttachObjectToTrackerTool( TrackerToolPort->value(), 0, m_Needle );

  m_Tracker->AttachObjectToTrackerTool( 
                                     TrackerToolPort->value(), 0, m_NeedleTip );
  m_Tracker->RequestStartTracking();
  m_PulseGenerator->RequestStart();   
  /** We don't have observer for tracker, we are actively reading the 
    * transform right now, how to get the failure condition */
  m_StateMachine.PushInput( m_StartTrackingSuccessInput ); 
  this->TrackingButton->label( "Stop" );
  this->ControlGroup->redraw();
  Fl::check();
  
}
/** Callback function for PulseEvent(), 
  * intend to actively read tracker tool transform */
void NeedleBiopsy::Tracking()
{
  igstkLogMacro( DEBUG,  "Pulse Events...\n" )
  igstk::Transform transform;
  m_Tracker->GetToolTransform( TrackerToolPort->value(), 0, transform );
  
  ImageSpatialObjectType::PointType    p;
  p[0] = transform.GetTranslation()[0];
  p[1] = transform.GetTranslation()[1];
  p[2] = transform.GetTranslation()[2];

  if( m_ImageSpatialObject->IsInside( p ) )
    {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( p, index);
    ResliceImage( index );
    }
  else
    {
    igstkLogMacro( DEBUG,  "Tracker tool outside of image...\n" )
    }
}

void NeedleBiopsy::RequestStopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                             "NeedleBiopsy::RequestStopTracking called ... \n" )
  m_StateMachine.PushInput( m_RequestStopTrackingInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::StopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "NeedleBiopsy::StopTracking called ... \n" )
  /** We don't have observer for tracker, we are actively reading 
    * the transform right now */
  m_Tracker->RequestStopTracking();
  m_PulseGenerator->RequestStop();
  // FIXME, How to get the failure condition
  m_StateMachine.PushInput( m_StopTrackingSuccessInput );
  this->TrackingButton->label( "Tracking" );
  this->ControlGroup->redraw();
  Fl::check();
}

void NeedleBiopsy::RequestResliceImage()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                              "NeedleBiopsy::RequestResliceImage called ... \n")
  this->ResliceImage(); // Take out the state machine logic from here
}

void NeedleBiopsy::ResliceImage()
{
  m_ImageRepresentationAxial->RequestSetSliceNumber( 
                    static_cast< unsigned int >( this->AxialSlider->value() ) );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( 
                 static_cast< unsigned int >( this->SagittalSlider->value() ) );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( 
                  static_cast< unsigned int >( this->CoronalSlider->value() ) );

  m_ImageRepresentationAxial3D->RequestSetSliceNumber( 
                    static_cast< unsigned int >( this->AxialSlider->value() ) );
  m_ImageRepresentationSagittal3D->RequestSetSliceNumber( 
                 static_cast< unsigned int >( this->SagittalSlider->value() ) );
  m_ImageRepresentationCoronal3D->RequestSetSliceNumber( 
                  static_cast< unsigned int >( this->CoronalSlider->value() ) );

  this->ViewerGroup->redraw();
  Fl::check();
}

void NeedleBiopsy::ResliceImage ( IndexType index )
{
  m_ImageRepresentationAxial->RequestSetSliceNumber( index[2] );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( index[0] );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( index[1] );

  m_ImageRepresentationAxial3D->RequestSetSliceNumber( index[2] );
  m_ImageRepresentationSagittal3D->RequestSetSliceNumber( index[0] );
  m_ImageRepresentationCoronal3D->RequestSetSliceNumber( index[1] );

  this->AxialSlider->value( index[2] );
  this->SagittalSlider->value( index[0] );
  this->CoronalSlider->value( index[1] );

  this->ViewerGroup->redraw();
  Fl::check();
}

/** This method should be invoked by the State Machine 
 *  only when the Image has been loaded and the Patient
 *  name has been verified */
void NeedleBiopsy::ConnectImageRepresentationProcessing()
{
  m_Annotation2D->RequestAddAnnotationText( 2, "Subject: " + m_PatientName );

  m_Annotation2D->RequestAddAnnotationText( 0, "Georgetown ISIS Center" );

  m_ImageRepresentationAxial->RequestSetImageSpatialObject( 
                                           m_ImageSpatialObject );
  m_ImageRepresentationSagittal->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );
  m_ImageRepresentationCoronal->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );

  m_ImageRepresentationAxial3D->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );
  m_ImageRepresentationSagittal3D->RequestSetImageSpatialObject( 
                                           m_ImageSpatialObject );
  m_ImageRepresentationCoronal3D->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );
 
  m_ImageRepresentationAxial->RequestSetOrientation( 
                                               ImageRepresentationType::Axial );
  m_ImageRepresentationSagittal->RequestSetOrientation( 
                                            ImageRepresentationType::Sagittal );
  m_ImageRepresentationCoronal->RequestSetOrientation( 
                                             ImageRepresentationType::Coronal );

  m_ImageRepresentationAxial3D->RequestSetOrientation( 
                                               ImageRepresentationType::Axial );
  m_ImageRepresentationSagittal3D->RequestSetOrientation( 
                                            ImageRepresentationType::Sagittal );
  m_ImageRepresentationCoronal3D->RequestSetOrientation( 
                                             ImageRepresentationType::Coronal );

  this->DisplayAxial->RequestAddObject( m_ImageRepresentationAxial );
  this->DisplayAxial->RequestAddObject( m_PickedPointRepresentation );
  this->DisplayAxial->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->DisplayAxial->RequestAddAnnotation2D( m_Annotation2D );

  this->DisplaySagittal->RequestAddObject( m_ImageRepresentationSagittal );
  this->DisplaySagittal->RequestAddObject(
                                          m_PickedPointRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->DisplaySagittal->RequestAddAnnotation2D( m_Annotation2D );

  this->DisplayCoronal->RequestAddObject( m_ImageRepresentationCoronal );
  this->DisplayCoronal->RequestAddObject( m_PickedPointRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->DisplayCoronal->RequestAddAnnotation2D( m_Annotation2D );

  this->Display3D->RequestAddObject( m_ImageRepresentationAxial3D );
  this->Display3D->RequestAddObject( m_ImageRepresentationSagittal3D );
  this->Display3D->RequestAddObject( m_ImageRepresentationCoronal3D );
  this->Display3D->RequestAddObject( m_PickedPointRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->Display3D->RequestAddAnnotation2D( m_Annotation2D );


  this->DisplayAxial->RequestAddObject( m_TargetRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_TargetRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_TargetRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_TargetRepresentation->Copy() );

  this->DisplayAxial->RequestAddObject( m_EntryRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_EntryRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_EntryRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_EntryRepresentation->Copy() );

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
  
  // Request information about the slices. The answers will be 
  // received in the form of events.
  this->m_ImageRepresentationAxial->RequestGetSliceNumberBounds();
  this->m_ImageRepresentationSagittal->RequestGetSliceNumberBounds();
  this->m_ImageRepresentationCoronal->RequestGetSliceNumberBounds();

  this->DisplayAxial->AddObserver( igstk::TransformModifiedEvent(), 
                                   m_ViewPickerObserver );
  this->DisplaySagittal->AddObserver( igstk::TransformModifiedEvent(), 
                                   m_ViewPickerObserver );
  this->DisplayCoronal->AddObserver( igstk::TransformModifiedEvent(), 
                                   m_ViewPickerObserver );

}

void NeedleBiopsy::SetAxialSliderBoundsProcessing()
{
  igstkLogMacro( DEBUG, "SetAxialSliderBounds() " << "\n");
  /** Initialize the slider */
  const unsigned int min = m_AxialBoundsInputToBeSet.minimum;
  const unsigned int max = m_AxialBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
  m_ImageRepresentationAxial->RequestSetSliceNumber( slice );
  m_ImageRepresentationAxial3D->RequestSetSliceNumber( slice );
  this->AxialSlider->minimum( min );
  this->AxialSlider->maximum( max );
  this->AxialSlider->value( slice );  
  this->AxialSlider->activate();

  this->ViewerGroup->redraw();
  Fl::check();
}


void NeedleBiopsy::SetSagittalSliderBoundsProcessing()
{
  igstkLogMacro( DEBUG, "SetSagittalSliderBounds() " << "\n");
  /** Initialize the slider */
  const unsigned int min = m_SagittalBoundsInputToBeSet.minimum;
  const unsigned int max = m_SagittalBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( slice );
  m_ImageRepresentationSagittal3D->RequestSetSliceNumber( slice );
  this->SagittalSlider->minimum( min );
  this->SagittalSlider->maximum( max );
  this->SagittalSlider->value( slice );  
  this->SagittalSlider->activate();


  this->ViewerGroup->redraw();
  Fl::check();
}


void NeedleBiopsy::SetCoronalSliderBoundsProcessing()
{
  igstkLogMacro( DEBUG, "SetCoronalSliderBounds() " << "\n");
  const unsigned int min = m_CoronalBoundsInputToBeSet.minimum;
  const unsigned int max = m_CoronalBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( slice );
  m_ImageRepresentationCoronal3D->RequestSetSliceNumber( slice );
  this->CoronalSlider->minimum( min );
  this->CoronalSlider->maximum( max );
  this->CoronalSlider->value( slice );  
  this->CoronalSlider->activate();


  this->ViewerGroup->redraw();
  Fl::check();
}

void NeedleBiopsy
::GetLandmarkRegistrationTransform( const itk::EventObject & event )
{
  if ( igstk::TransformModifiedEvent().CheckEvent( &event ) )
    {
    igstk::TransformModifiedEvent *tmevent = 
                                     ( igstk::TransformModifiedEvent *) & event;
    m_ImageToTrackerTransform = tmevent->Get();
    
    m_Tracker->SetPatientTransform( m_ImageToTrackerTransform );
    
    igstkLogMacro( DEBUG, 
                 "Registration Transform" << m_ImageToTrackerTransform << "\n");
    m_StateMachine.PushInput( m_RegistrationSuccessInput );
    }
  else
    {
    m_StateMachine.PushInput( m_RegistrationFailureInput );
    }
}

void NeedleBiopsy
::GetLandmarkRegistrationRMSError( const itk::EventObject & event )
{
  if ( igstk::DoubleTypeEvent().CheckEvent( &event ) )
    {
    igstk::DoubleTypeEvent *tmevent = 
                                     ( igstk::DoubleTypeEvent *) & event;
    m_LandmarkRegistrationRMSError  = tmevent->Get();
    
    igstkLogMacro( DEBUG, 
     "Registration Error" << m_LandmarkRegistrationRMSError << "\n");
    }
}

void NeedleBiopsy::DrawPickedPoint( const itk::EventObject & event)
{
  if ( igstk::TransformModifiedEvent().CheckEvent( &event ) )
    {
    igstk::TransformModifiedEvent *tmevent = 
                                     ( igstk::TransformModifiedEvent *) & event;
    
    ImageSpatialObjectType::PointType    p;
    p[0] = tmevent->Get().GetTranslation()[0];
    p[1] = tmevent->Get().GetTranslation()[1];
    p[2] = tmevent->Get().GetTranslation()[2];
    
    if( m_ImageSpatialObject->IsInside( p ) )
      {
      m_ImageLandmarkTransformToBeSet = tmevent->Get();

      std::cout<< m_ImageLandmarkTransformToBeSet << std::endl;
      
      m_PickedPoint->RequestSetTransform( m_ImageLandmarkTransformToBeSet );
      ImageSpatialObjectType::IndexType index;
      m_ImageSpatialObject->TransformPhysicalPointToIndex( p, index);
      igstkLogMacro( DEBUG, index <<"\n")
      ResliceImage( index );
      }
    else
      {
      igstkLogMacro( DEBUG,  "Picked point outside image...\n" )
      }
    }
}

void NeedleBiopsy::EvaluatingRegistrationErrorProcessing()
{
  igstkLogMacro (         DEBUG, "Evaluating registration error....\n" )
  igstkLogMacro2( m_Logger, DEBUG, "Evaluating registration error....\n" )
  char temp[255];
  double error = m_LandmarkRegistrationRMSError;
  sprintf( temp, "Registration error (RMS) = %f\n", error );
  std::string msg = temp;
  msg += "Accept this registration result?";
  int i = fl_choice( msg.c_str(), NULL, "Yes", "No" );
  if ( i==1 )
    {
    this->RegistrationError->value( error );
    m_StateMachine.PushInput( m_RegistrationErrorAcceptedInput );
    }
  else
    {
    m_StateMachine.PushInput( m_RegistrationErrorRejectedInput );
    }
  
}

void NeedleBiopsy::ResetRegistrationProcessing()
{
  igstkLogMacro (         DEBUG, "Reset registration....\n" )
  igstkLogMacro2( m_Logger, DEBUG, "Reset registration....\n" )
  m_LandmarkRegistration->RequestResetRegistration();  
  this->RegistrationError->value( 0.0 );
}

void NeedleBiopsy::RequestSetTargetPoint()
{
  igstkLogMacro ( DEBUG, "NeedleBiopsy::RequestSetTargetPoint called....\n" )
  igstkLogMacro2( m_Logger, DEBUG, 
                            "NeedleBiopsy::RequestSetTargetPoint called....\n" )
  m_TargetTransform = m_ImageLandmarkTransformToBeSet;
  m_StateMachine.PushInput( m_RequestSetTargetPointInput );
  m_StateMachine.ProcessInputs();
}
void NeedleBiopsy::RequestSetEntryPoint()
{
  igstkLogMacro ( DEBUG, "NeedleBiopsy::RequestSetEntryPoint called....\n" )
  igstkLogMacro2( m_Logger, DEBUG, 
                             "NeedleBiopsy::RequestSetEntryPoint called....\n" )
  m_EntryTransform = m_ImageLandmarkTransformToBeSet;
  m_StateMachine.PushInput( m_RequestSetEntryPointInput );
  m_StateMachine.ProcessInputs();
}

void NeedleBiopsy::DrawTargetPointProcessing()
{
  m_TargetPoint->RequestSetTransform( m_TargetTransform );
}

void NeedleBiopsy::DrawPathProcessing()
{
  m_TargetPoint->RequestSetTransform( m_TargetTransform );
  m_EntryPoint->RequestSetTransform( m_EntryTransform );

  m_Path->Clear();
  
  TubePointType p;
  igstk::Transform::VectorType v;
    
  v = m_EntryTransform.GetTranslation();
  p.SetPosition( v[0], v[1], v[2]);
  p.SetRadius( 2 );
  m_Path->AddPoint( p );

  v = m_TargetTransform.GetTranslation();
  p.SetPosition( v[0], v[1], v[2]);
  p.SetRadius( 2.1 ); //FIXME
  m_Path->AddPoint( p );

  igstk::Transform transform;
  transform.SetToIdentity( 1e300 );
  m_Path->RequestSetTransform( transform );

  this->DisplayAxial->RequestRemoveObject( m_PathRepresentationAxial );
  this->DisplaySagittal->RequestRemoveObject( m_PathRepresentationSagittal );
  this->DisplayCoronal->RequestRemoveObject( m_PathRepresentationCoronal );
  this->Display3D->RequestRemoveObject( m_PathRepresentation3D );

  m_PathRepresentationAxial->RequestSetTubeObject( NULL );
  m_PathRepresentationAxial->RequestSetTubeObject( m_Path );
  m_PathRepresentationAxial->SetColor( 0.0, 1.0, 0.0 );
  m_PathRepresentationAxial->SetOpacity( 0.5 );
  m_PathRepresentationSagittal->RequestSetTubeObject( NULL );
  m_PathRepresentationSagittal->RequestSetTubeObject( m_Path );
  m_PathRepresentationSagittal->SetColor( 0.0, 1.0, 0.0 );
  m_PathRepresentationSagittal->SetOpacity( 0.5 );
  m_PathRepresentationCoronal->RequestSetTubeObject( NULL );
  m_PathRepresentationCoronal->RequestSetTubeObject( m_Path );
  m_PathRepresentationCoronal->SetColor( 0.0, 1.0, 0.0 );
  m_PathRepresentationCoronal->SetOpacity( 0.5 );
  m_PathRepresentation3D->RequestSetTubeObject( NULL );
  m_PathRepresentation3D->RequestSetTubeObject( m_Path );
  m_PathRepresentation3D->SetColor( 0.0, 1.0, 0.0 );
  m_PathRepresentation3D->SetOpacity( 0.5 );

  this->DisplayAxial->RequestAddObject( m_PathRepresentationAxial );
  this->DisplaySagittal->RequestAddObject( m_PathRepresentationSagittal );
  this->DisplayCoronal->RequestAddObject( m_PathRepresentationCoronal );
  this->Display3D->RequestAddObject( m_PathRepresentation3D );
  
}


void NeedleBiopsy::RequestReset()
{
  igstkLogMacro( DEBUG, "NeedleBiopsy::RequestReset is called... \n" )
  if ( fl_choice( "Do you really want to reset the program?", 
                                                           NULL, "Yes", "No" ) )
    { 
    this->Reset(); // Took out the state machine logic
    }
}

void NeedleBiopsy::Reset()
{
  // Reset method not provided here
}
