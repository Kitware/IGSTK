/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FourViewsTrackingWithCT.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "FourViewsTrackingWithCT.h"
#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"
#include "igstkEvents.h"


/** Constructor: Initializes all internal variables. */
FourViewsTrackingWithCT::FourViewsTrackingWithCT():m_StateMachine(this)
{  
  /** Setup logger, for all other igstk components. */
  m_Logger   = LoggerType::New();

  /** Direct the application log message to the std::cout 
   *  and FLTK text display. */
  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  Fl_Text_Buffer * textBuffer = new Fl_Text_Buffer();
  this->m_LogWindow->buffer( textBuffer );
  m_LogFLTKOutput = igstk::FLTKTextLogOutput::New();
  m_LogFLTKOutput->SetStream( *m_LogWindow );
  this->GetLogger()->AddLogOutput( m_LogFLTKOutput );

  /** Direct the igstk components log message to the file. */
  m_LogFileOutput = LogOutputType::New();
  //FIXME. use a date/time file name
  std::string logFileName = "logFourViewsTrackingWithCT.txt";
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    m_Logger->AddLogOutput( m_LogFileOutput );
    }
  else
    {
    //FIXME. may not return
    igstkLogMacro( DEBUG, "Problem opening Log file: " << logFileName << "\n" );
    return;
    }

  /** Initialize all member variables and set logger */
  m_ImageReader = ImageReaderType::New();
  m_ImageReader->SetLogger( m_Logger );
  m_CTImageObserver = CTImageObserver::New();
  m_ImageReader->AddObserver(igstk::CTImageReader::ImageModifiedEvent(),
                             m_CTImageObserver);

  m_LandmarkRegistration = RegistrationType::New();
  m_LandmarkRegistration->SetLogger( m_Logger );

  m_LandmarkRegistrationObserver = ObserverType2::New();
  m_LandmarkRegistrationObserver->SetCallbackFunction( this, 
    &FourViewsTrackingWithCT::GetLandmarkRegistrationTransform );

  m_LandmarkRegistration->AddObserver( 
    igstk::CoordinateSystemTransformToEvent(),
    m_LandmarkRegistrationObserver );

  m_LandmarkRegistration->AddObserver( 
    igstk::TransformNotAvailableEvent(), 
    m_LandmarkRegistrationObserver );

  m_LandmarkRegistrationRMSErrorObserver = ObserverType2::New();
  m_LandmarkRegistrationRMSErrorObserver->SetCallbackFunction( this, 
            &FourViewsTrackingWithCT::GetLandmarkRegistrationRMSError );
  m_LandmarkRegistration->AddObserver( igstk::DoubleTypeEvent(), 
            m_LandmarkRegistrationRMSErrorObserver );
 
  //Initialize the registration RMS error 
  m_LandmarkRegistrationRMSError = 0.0;


  m_SerialCommunication = CommunicationType::New();
  m_SerialCommunication->SetLogger( m_Logger );
  m_SerialCommunication->SetPortNumber(igstk::SerialCommunication::PortNumber2);
  m_SerialCommunication->SetParity( igstk::SerialCommunication::NoParity );
  m_SerialCommunication->SetBaudRate(igstk::SerialCommunication::BaudRate115200);
  m_SerialCommunication->SetDataBits(igstk::SerialCommunication::DataBits8 );
  m_SerialCommunication->SetStopBits(igstk::SerialCommunication::StopBits1 );
  m_SerialCommunication->SetHardwareHandshake( 
                                      igstk::SerialCommunication::HandshakeOff);
  m_SerialCommunication->OpenCommunication();
  
  m_Tracker = TrackerType::New();
  m_Tracker->SetLogger( m_Logger );
  m_Tracker->SetCommunication( m_SerialCommunication );

  m_Ellipsoid                   = EllipsoidType::New();
  m_EllipsoidRepresentation     = EllipsoidRepresentationType::New();
  m_Ellipsoid->SetRadius( 5, 5, 5 );  
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
  
  igstk::PivotCalibration::Pointer pivot = igstk::PivotCalibration::New();
  //pivot->RequestSetToolPrincipalAxis( 0.0, 1.0, 0.0);
  //pivot->RequestSetToolPlaneNormal( 1.0, 0.0, 0.0);

  //FIXME, SpatialObject should have an tip to origin offet
  igstk::Transform::VectorType CylinderTipOffset;
  CylinderTipOffset[0] = 0;   // Tip offset
  CylinderTipOffset[1] = -200;
  CylinderTipOffset[2] = 0;
  
  /** Tool calibration transform */
  igstk::Transform toolCalibrationTransform;
  igstk::Transform::VectorType translation;
  igstk::Transform::VersorType rotation;
 
  rotation.SetRotationAroundX( 3.1415926/2 );
  rotation = rotation.GetConjugate();  
  rotation.SetIdentity();
  
  translation[0] = -18.0;   // Tip offset
  translation[1] = 0.5;
  translation[2] = -157.5;

  toolCalibrationTransform.SetTranslationAndRotation(translation, 
                                                     rotation, 0.1, 10000);
  /* FIXME
  m_Tracker->SetToolCalibrationTransform( TRACKER_TOOL_PORT, 0,
                                          toolCalibrationTransform);
  */

  m_ImageToTrackerTransform.SetToIdentity( 10000 );
  m_ImageLandmarkTransform.SetToIdentity( 10000 );
  m_TrackerLandmarkTransform.SetToIdentity( 10000 );

  m_PulseGenerator = igstk::PulseGenerator::New();
  m_Observer = ObserverType::New();
  m_Observer->SetCallbackFunction( this, & FourViewsTrackingWithCT::Tracking );
  m_PulseGenerator->AddObserver( igstk::PulseEvent(), m_Observer );
  //FIXME, move to request start tracking??
  m_PulseGenerator->RequestSetFrequency( 30 );

  /** Temporary disable this logger */
  //this->DisplayAxial->SetLogger( logger );
  //this->DisplaySagittal->SetLogger( logger );
  //this->DisplayCoronal->SetLogger( logger );

  m_ViewPickerObserver = ObserverType2::New();
  m_ViewPickerObserver->SetCallbackFunction( this, 
                                   &FourViewsTrackingWithCT::DrawPickedPoint );

  this->m_DisplayAxial->AddObserver( 
    igstk::CoordinateSystemTransformToEvent(), m_ViewPickerObserver );

  this->m_DisplaySagittal->AddObserver( 
    igstk::CoordinateSystemTransformToEvent(), m_ViewPickerObserver );

  this->m_DisplayCoronal->AddObserver( 
    igstk::CoordinateSystemTransformToEvent(), m_ViewPickerObserver );

  this->m_DisplayAxial->RequestSetOrientation( igstk::View2D::Axial );
  this->m_DisplaySagittal->RequestSetOrientation( igstk::View2D::Sagittal );
  this->m_DisplayCoronal->RequestSetOrientation( igstk::View2D::Coronal );

  m_ImageRepresentationAxial    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal = ImageRepresentationType::New();
  m_ImageRepresentationCoronal  = ImageRepresentationType::New();
  m_ImageRepresentationAxial3D    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal3D = ImageRepresentationType::New();
  m_ImageRepresentationCoronal3D  = ImageRepresentationType::New();

  m_ImageRepresentationAxial->SetLogger( m_Logger );
  m_ImageRepresentationSagittal->SetLogger( m_Logger );
  m_ImageRepresentationCoronal->SetLogger( m_Logger );
  m_ImageRepresentationAxial3D->SetLogger( m_Logger );
  m_ImageRepresentationSagittal3D->SetLogger( m_Logger );
  m_ImageRepresentationCoronal3D->SetLogger( m_Logger );

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
  igstkAddStateMacro( InitializingTracker        );
  igstkAddStateMacro( TrackerReady               );  
  igstkAddStateMacro( AddingTrackerLandmark      );
  igstkAddStateMacro( TrackerLandmarksReady      );
  igstkAddStateMacro( LandmarkRegistrationReady  );
  igstkAddStateMacro( Tracking                   );


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
                           InitializingTracker, InitializeTracker );
  igstkAddTransitionMacro( InitializingTracker, InitializeTrackerSuccess, 
                           TrackerReady, No );
  igstkAddTransitionMacro( InitializingTracker, InitializeTrackerFailure, 
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
                           TrackerLandmarksReady, Registration );
  igstkAddTransitionMacro( TrackerLandmarksReady, RegistrationSuccess, 
                           LandmarkRegistrationReady, No );
  igstkAddTransitionMacro( TrackerLandmarksReady, RegistrationFailure, 
                           TrackerLandmarksReady, No );

  /** Tracking */
  igstkAddTransitionMacro( LandmarkRegistrationReady, RequestStartTracking, 
                           LandmarkRegistrationReady, StartTracking );
  igstkAddTransitionMacro( LandmarkRegistrationReady, StartTrackingSuccess, 
                           Tracking, No);
  igstkAddTransitionMacro( LandmarkRegistrationReady, StartTrackingFailure, 
                           LandmarkRegistrationReady, No );

  igstkAddTransitionMacro( Tracking, RequestStopTracking, 
                           Tracking, StopTracking );
  igstkAddTransitionMacro( Tracking, StopTrackingSuccess, 
                           LandmarkRegistrationReady, No);
  igstkAddTransitionMacro( Tracking, StopTrackingFailure,
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
FourViewsTrackingWithCT::~FourViewsTrackingWithCT()
{
 
}

/** Method to be invoked when no operation is required */
void FourViewsTrackingWithCT::NoProcessing()
{
}


void FourViewsTrackingWithCT::RequestSetPatientName()
{
  igstkLogMacro2( m_Logger, DEBUG, "FourViewsTrackingWithCT::\
                                 RequestSetPatientName called...\n" )
  m_StateMachine.PushInput( m_RequestSetPatientNameInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::SetPatientNameProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "FourViewsTrackingWithCT::\
                                 GetPatientName called...\n" )
  const char *patientName = fl_input("Patient Name:", "");
  if( patientName != NULL )
    {
    m_PatientName = patientName;
    igstkLogMacro(          DEBUG, "Patient registered as: "
                                    << m_PatientName <<"\n" )
    igstkLogMacro2( m_Logger, DEBUG, "Patient registered as: "
                                    << m_PatientName <<"\n" )
    m_StateMachine.PushInput( m_PatientNameInput );
    }
  else
    {
    m_StateMachine.PushInput( m_PatientNameEmptyInput );
    }
}


void FourViewsTrackingWithCT::RequestLoadImage()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "FourViewsTrackingWithCT::RequestLoadImageProcessing called...\n" )
  m_StateMachine.PushInput( m_RequestLoadImageInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::LoadImageProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "FourViewsTrackingWithCT::LoadImageProcessing called...\n" )
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
      return;
      }

    m_StateMachine.PushInputBoolean( m_ImageReader->FileSuccessfullyRead(), 
                              m_LoadImageSuccessInput, m_LoadImageFailureInput);
    }
  else
    {
    igstkLogMacro(          DEBUG, "No directory is selected\n" )
    igstkLogMacro2( m_Logger, DEBUG, "No directory is selected\n" )
    m_StateMachine.PushInput( m_LoadImageFailureInput );
    }
}

void FourViewsTrackingWithCT::VerifyPatientNameProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
           "FourViewsTrackingWithCT::VerifyPatientNameProcessing called ... \n")
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
    msg += "Do you want to load another image? \
                                       choose \'no\' will overwrite the name\n";
    int i = fl_choice( "%s", msg.c_str(), "Yes", "No", "Cancel");
    if ( i )
      {
      igstkLogMacro (         DEBUG, "Load another image\n" )
      igstkLogMacro2( m_Logger, DEBUG, "Load another image\n" )
      m_StateMachine.PushInput( m_ReloadImageInput );
      }
    else
      {
      m_PatientName = m_ImageReader->GetPatientName();
      igstkLogMacro( DEBUG, 
                    "Patient name is overwritten to:" << m_PatientName << "\n" )
      igstkLogMacro2( m_Logger, DEBUG, 
                    "Patient name is overwritten to:" << m_PatientName << "\n" )
      m_StateMachine.PushInput( m_OverwritePatientNameInput );
      }
    }
}

void FourViewsTrackingWithCT::RequestInitializeTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "FourViewsTrackingWithCT::RequestInitializeTracker called ... \n" )
  m_StateMachine.PushInput( m_RequestInitializeTrackerInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::InitializeTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
          "FourViewsTrackingWithCT::InitializeTrackerProcessing called ... \n" )
  m_Tracker->RequestOpen();
  /* FIXME
  m_Tracker->AttachSROMFileNameToPort( 
                                    TRACKER_TOOL_PORT, TRACKER_TOOL_SROM_FILE );
  m_Tracker->RequestInitialize();
  */
  m_Tracker->RequestStartTracking();
  /* FIXME 
  m_StateMachine.PushInputBoolean( m_Tracker->GetNumberOfTools(), 
             m_InitializeTrackerSuccessInput, m_InitializeTrackerFailureInput );
  */
}

void FourViewsTrackingWithCT::RequestAddImageLandmark()
{
  igstkLogMacro2( m_Logger, DEBUG, 
              "FourViewsTrackingWithCT::RequestAddImageLandmark called ... \n" )
  m_StateMachine.PushInput( m_RequestAddImageLandmarkInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::AddImageLandmarkProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
           "FourViewsTrackingWithCT::AddImageLandmarkProcessing called ... \n" )

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
    igstkLogMacro2( m_Logger, DEBUG, 
                    "Image landmark point added: "<< p << "\n" )
    }
  else
    {
    igstkLogMacro(          DEBUG, "No new image landmark point picked.\n" )
    igstkLogMacro2( m_Logger, DEBUG, "No new image landmark point picked.\n" )
    }

  m_StateMachine.PushInputBoolean( (m_ImageLandmarksContainer.size()>=3),
                    m_EnoughLandmarkPointsInput, m_NeedMoreLandmarkPointsInput);
}

void FourViewsTrackingWithCT::RequestClearImageLandmarks()
{
  igstkLogMacro2( m_Logger, DEBUG, 
            "FourViewsTrackingWithCT::RequestClearImageLandmarks called ... \n")
  m_StateMachine.PushInput( m_RequestClearImageLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::ClearImageLandmarksProcessing()
{
  igstkLogMacro(          DEBUG, "Image landmark points cleared...\n" )
  igstkLogMacro2( m_Logger, DEBUG, "Image landmark points cleared...\n" )
  m_ImageLandmarksContainer.clear();
  this->NumberOfImageLandmarks->value( 0 );
  this->NumberOfImageLandmarks->textcolor( FL_BLACK );
  m_ImageLandmarkTransform.SetTranslation( 
                 m_ImageLandmarkTransformToBeSet.GetTranslation(), 0.1, 10000 );
}

void FourViewsTrackingWithCT::RequestAddTrackerLandmark()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "FourViewsTrackingWithCT::RequestAddTrackerLandmark called ... \n")
  m_StateMachine.PushInput( m_RequestAddTrackerLandmarkInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::AddTrackerLandmarkProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
          "FourViewsTrackingWithCT::AddTrackerLandmarkProcessing called ... \n")
  
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
                                  "Tracker landmark point added: "<< p << "\n" )

    }
  else
    {
    igstkLogMacro(          DEBUG, "No new tracker landmark point reading.\n" )
    igstkLogMacro2( m_Logger, 
                    DEBUG, "No new tracker landmark point reading.\n" )
    }  

  m_StateMachine.PushInputBoolean( 
      ( m_TrackerLandmarksContainer.size() < m_ImageLandmarksContainer.size() ),
                   m_NeedMoreLandmarkPointsInput, m_EnoughLandmarkPointsInput );
}

void FourViewsTrackingWithCT::GetTrackerTransform()
{
  igstkLogMacro2( m_Logger, DEBUG, "Tracker::GetToolTransform called...\n" )
  /* FIXME
  m_Tracker->GetToolTransform( 
                      TRACKER_TOOL_PORT, 0, m_TrackerLandmarkTransformToBeSet );
   */
}

void FourViewsTrackingWithCT::RequestClearTrackerLandmarks()
{
  igstkLogMacro2( m_Logger, DEBUG, 
          "FourViewsTrackingWithCT::RequestClearTrackerLandmarks called ... \n")
  m_StateMachine.PushInput( m_RequestClearTrackerLandmarksInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::ClearTrackerLandmarksProcessing()
{
  igstkLogMacro(          DEBUG, "Tracker landmark points cleared...\n" )
  igstkLogMacro2( m_Logger, DEBUG, "Tracker landmark points cleared...\n" )
  m_TrackerLandmarksContainer.clear();
  this->NumberOfTrackerLandmarks->value( 0 );
  this->NumberOfTrackerLandmarks->textcolor( FL_BLACK );
  m_TrackerLandmarkTransform.SetTranslation( 
               m_TrackerLandmarkTransformToBeSet.GetTranslation(), 0.1, 10000 );
}

void FourViewsTrackingWithCT::RequestRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                  "FourViewsTrackingWithCT::RequestRegistration called ... \n" )
  m_StateMachine.PushInput( m_RequestRegistrationInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::RegistrationProcessing()
{
  LandmarkPointContainerType::iterator it1, it2;
  for( it1 = m_ImageLandmarksContainer.begin(), 
       it2 = m_TrackerLandmarksContainer.begin(); 
       it1 != m_ImageLandmarksContainer.end(), 
       it2 != m_TrackerLandmarksContainer.end();
       it1 ++ , it2 ++ )
    {
    m_LandmarkRegistration->RequestAddImageLandmarkPoint( *it1 );
    m_LandmarkRegistration->RequestAddTrackerLandmarkPoint( *it2 );  
    }
  m_LandmarkRegistration->RequestComputeTransform();
  m_LandmarkRegistration->RequestGetRMSError();
  m_LandmarkRegistration->RequestGetTransformFromTrackerToImage();
}

void FourViewsTrackingWithCT::RequestStartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                 "FourViewsTrackingWithCT::RequestStartTracking called ... \n" )
  m_StateMachine.PushInput( m_RequestStartTrackingInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::StartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
              "FourViewsTrackingWithCT::StartTrackingProcessing called ... \n" )

  /* FIXME
  m_Tracker->AttachObjectToTrackerTool( TRACKER_TOOL_PORT, 0, m_Cylinder );
  m_Tracker->AttachObjectToTrackerTool( TRACKER_TOOL_PORT, 0, m_Ellipsoid );
   */
  m_Tracker->RequestStartTracking();
  m_PulseGenerator->RequestStart();   
  /** We don't have observer for tracker, we are actively reading the transform 
    * right now,how to get the failure condition */
  m_StateMachine.PushInput( m_StartTrackingSuccessInput ); 
  
}

/** Callback function for PulseEvent(), 
  * intend to actively read tracker tool transform */
void FourViewsTrackingWithCT::Tracking()
{
  igstkLogMacro( DEBUG,  "Pulse Events...\n" )
  //m_Tracker->UpdateStatus();
  igstk::Transform transform;
  //transform = m_Ellipsoid->GetTransform();
  /* FIXME
  m_Tracker->GetToolTransform( TRACKER_TOOL_PORT, 0, transform );
  */ 
  //m_Ellipsoid->RequestSetTransform( transform );

  ImageSpatialObjectType::PointType    p;
  p[0] = transform.GetTranslation()[0];
  p[1] = transform.GetTranslation()[1];
  p[2] = transform.GetTranslation()[2];

  igstkLogMacro( DEBUG,  "Tracker tool translation:" << p << "\n" )
  if( m_CTImageObserver->GetCTImage()->IsInside( p ) )
    {
    ImageSpatialObjectType::IndexType index;
    m_CTImageObserver->GetCTImage()
                         ->TransformPhysicalPointToIndex( p, index );
    igstkLogMacro( DEBUG,  "Tracker tool index:" << index << "\n" )
    ResliceImage( index );
    }
  else
    {
    igstkLogMacro( DEBUG,  "Tracker tool outside of image...\n" )
    }
}

void FourViewsTrackingWithCT::RequestStopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                  "FourViewsTrackingWithCT::RequestStopTracking called ... \n" )
  m_StateMachine.PushInput( m_RequestStopTrackingInput );
  m_StateMachine.ProcessInputs();
}

void FourViewsTrackingWithCT::StopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                         "FourViewsTrackingWithCT::StopTracking called ... \n" )
  /** We don't have observer for tracker, we are actively reading the transform 
    * right now
    */
  m_Tracker->RequestStopTracking();
  m_PulseGenerator->RequestStop();
  // FIXME, How to get the failure condition
  m_StateMachine.PushInput( m_StopTrackingSuccessInput ); 
}

void FourViewsTrackingWithCT::RequestResliceImage()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                   "FourViewsTrackingWithCT::RequestResliceImage called ... \n")
  this->ResliceImage(); // Take out the state machine logic from here
}

void FourViewsTrackingWithCT::ResliceImage()
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

  igstk::PulseGenerator::CheckTimeouts();
  this->ViewerGroup->redraw();
  Fl::check();
}

void FourViewsTrackingWithCT::ResliceImage ( IndexType index )
{
  
  igstkLogMacro( DEBUG, "ResliceImage( IndexType index ) called... \n" )
  m_ImageRepresentationAxial->RequestSetSliceNumber( index[2] );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( index[0] );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( index[1] );

  m_ImageRepresentationAxial3D->RequestSetSliceNumber( index[2] );
  m_ImageRepresentationSagittal3D->RequestSetSliceNumber( index[0] );
  m_ImageRepresentationCoronal3D->RequestSetSliceNumber( index[1] );

  igstk::PulseGenerator::CheckTimeouts();

  this->AxialSlider->value( index[2] );
  this->SagittalSlider->value( index[0] );
  this->CoronalSlider->value( index[1] );

  this->ViewerGroup->redraw();
  Fl::check();
}

/** This method should be invoked by the State Machine 
 *  only when the Image has been loaded and the Patient
 *  name has been verified */
void FourViewsTrackingWithCT::ConnectImageRepresentationProcessing()
{
  m_ImageRepresentationAxial->RequestSetImageSpatialObject( 
                                           m_CTImageObserver->GetCTImage() );
  m_ImageRepresentationSagittal->RequestSetImageSpatialObject( 
                                           m_CTImageObserver->GetCTImage() );
  m_ImageRepresentationCoronal->RequestSetImageSpatialObject( 
                                           m_CTImageObserver->GetCTImage() );

  m_ImageRepresentationAxial3D->RequestSetImageSpatialObject( 
                                           m_CTImageObserver->GetCTImage() );
  m_ImageRepresentationSagittal3D->RequestSetImageSpatialObject( 
                                           m_CTImageObserver->GetCTImage() );
  m_ImageRepresentationCoronal3D->RequestSetImageSpatialObject( 
                                           m_CTImageObserver->GetCTImage() );
 
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

  this->m_DisplayAxial->RequestRemoveObject( m_ImageRepresentationAxial );
  this->m_DisplaySagittal->RequestRemoveObject( m_ImageRepresentationSagittal );
  this->m_DisplayCoronal->RequestRemoveObject( m_ImageRepresentationCoronal );
  this->m_Display3D->RequestRemoveObject( m_ImageRepresentationAxial3D );
  this->m_Display3D->RequestRemoveObject( m_ImageRepresentationSagittal3D );
  this->m_Display3D->RequestRemoveObject( m_ImageRepresentationCoronal3D );

  this->m_DisplayAxial->RequestAddObject( m_ImageRepresentationAxial );
  this->m_DisplayAxial->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->m_DisplayAxial->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->m_DisplaySagittal->RequestAddObject( m_ImageRepresentationSagittal );
  this->m_DisplaySagittal->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->m_DisplaySagittal->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->m_DisplayCoronal->RequestAddObject( m_ImageRepresentationCoronal );
  this->m_DisplayCoronal->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->m_DisplayCoronal->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->m_Display3D->RequestAddObject( m_ImageRepresentationAxial3D );
  this->m_Display3D->RequestAddObject( m_ImageRepresentationSagittal3D );
  this->m_Display3D->RequestAddObject( m_ImageRepresentationCoronal3D );
  this->m_Display3D->RequestAddObject( m_EllipsoidRepresentation->Copy() );
  this->m_Display3D->RequestAddObject( m_CylinderRepresentation->Copy() );

  this->m_Display3D->RequestResetCamera();
  // this->Display3D->Update();
  this->m_Display3DWidget->RequestEnableInteractions();
  this->m_Display3D->SetRefreshRate( 30 ); // 30 Hz
  this->m_Display3D->RequestStart();
  
  this->m_DisplaySagittal->RequestResetCamera();
  // this->DisplaySagittal->Update();
  this->m_DisplaySagittalWidget->RequestEnableInteractions();
  this->m_DisplaySagittal->SetRefreshRate( 30 ); // 30 Hz
  this->m_DisplaySagittal->RequestStart();

  this->m_DisplayCoronal->RequestResetCamera();
  // this->DisplayCoronal->Update();
  this->m_DisplayCoronalWidget->RequestEnableInteractions();
  this->m_DisplayCoronal->SetRefreshRate( 30 ); // 30 Hz
  this->m_DisplayCoronal->RequestStart();  

  this->m_DisplayAxial->RequestResetCamera();
  // this->DisplayAxial->Update();
  this->m_DisplayAxialWidget->RequestEnableInteractions();
  this->m_DisplayAxial->SetRefreshRate( 30 ); // 30 Hz
  this->m_DisplayAxial->RequestStart();
  
  // Request information about the slices. The answers will be 
  // received in the form of events.
  this->m_ImageRepresentationAxial->RequestGetSliceNumberBounds();
  this->m_ImageRepresentationSagittal->RequestGetSliceNumberBounds();
  this->m_ImageRepresentationCoronal->RequestGetSliceNumberBounds();

}

  
void FourViewsTrackingWithCT::SetAxialSliderBoundsProcessing()
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
    

void FourViewsTrackingWithCT::SetSagittalSliderBoundsProcessing()
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
    

void FourViewsTrackingWithCT::SetCoronalSliderBoundsProcessing()
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


void FourViewsTrackingWithCT
::GetLandmarkRegistrationTransform( const itk::EventObject & event )
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
    {
    typedef igstk::CoordinateSystemTransformToEvent EventType;

    const EventType *tmevent = dynamic_cast< const EventType * >( & event );

    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();
    m_ImageToTrackerTransform = transformCarrier.GetTransform();
    
    igstkLogMacro( DEBUG, 
                 "Registration Transform" << m_ImageToTrackerTransform << "\n");
    igstkLogMacro( DEBUG, "Registration Transform Inverse" 
                             << m_ImageToTrackerTransform.GetInverse() << "\n");
    m_StateMachine.PushInput( m_RegistrationSuccessInput );
    }
  else
    {
    if( igstk::TransformNotAvailableEvent().CheckEvent( &event ) )
      {
      m_StateMachine.PushInput( m_RegistrationFailureInput );
      }
    }
}

void FourViewsTrackingWithCT
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

void FourViewsTrackingWithCT::DrawPickedPoint( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
    {
    typedef igstk::CoordinateSystemTransformToEvent EventType;

    const EventType *tmevent = dynamic_cast< const EventType * >( & event );

    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();

    igstk::Transform transform = transformCarrier.GetTransform();

    ImageSpatialObjectType::PointType    p;
    p[0] = transform.GetTranslation()[0];
    p[1] = transform.GetTranslation()[1];
    p[2] = transform.GetTranslation()[2];
    
    // FIXME:
    // We should check if GetCTImage() returns a valid image or guarantee
    // that there is always a valid CTImage.
    // Clicking inside a viewport without an image can cause a segfault.
    if( m_CTImageObserver->GetCTImage().IsNull() )
      {
      igstkLogMacro( WARNING,  "NULL CT Image.\n" )
      return;
      }

    if( m_CTImageObserver->GetCTImage()->IsInside( p ) )
      {
      m_ImageLandmarkTransformToBeSet = transform;
#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
      m_Ellipsoid->RequestSetTransform( m_ImageLandmarkTransformToBeSet );
#endif
      ImageSpatialObjectType::IndexType index;
      m_CTImageObserver->GetCTImage()
                          ->TransformPhysicalPointToIndex( p, index);
      igstkLogMacro( DEBUG, index <<"\n")
      ResliceImage( index );
      }
    else
      {
      igstkLogMacro( DEBUG,  "Picked point outside image...\n" )
      }
    }
}

void FourViewsTrackingWithCT::RequestReset()
{
  igstkLogMacro( DEBUG, "FourViewsTrackingWithCT::RequestReset is called...\n" )
  if ( fl_choice( "Do you really want to reset the program?",
                                                        "Yes","No", "Cancel" ) )
    { 
    this->Reset(); // Took out the state machine logic
    }
  
}

void FourViewsTrackingWithCT::Reset()
{
  //FILLME!!!!!!!!
  m_Tracker->RequestStopTracking();
  m_Tracker->RequestClose();
  m_Tracker->RequestReset();

  m_LandmarkRegistration->RequestResetRegistration();

  this->ClearImageLandmarksProcessing();
  this->ClearTrackerLandmarksProcessing();

  this->m_DisplayAxial->RequestStop();
  this->m_DisplaySagittal->RequestStop();
  this->m_DisplayCoronal->RequestStop();
  this->m_Display3D->RequestStop();

  this->m_DisplayAxialWidget->RequestDisableInteractions();
  this->m_DisplaySagittalWidget->RequestDisableInteractions();
  this->m_DisplayCoronalWidget->RequestDisableInteractions();
  this->m_Display3DWidget->RequestDisableInteractions();

  this->m_DisplayAxial->RequestRemoveObject( m_ImageRepresentationAxial );
  this->m_DisplaySagittal->RequestRemoveObject( m_ImageRepresentationSagittal );
  this->m_DisplayCoronal->RequestRemoveObject( m_ImageRepresentationCoronal );


  this->m_Display3D->RequestRemoveObject( m_ImageRepresentationAxial3D );
  this->m_Display3D->RequestRemoveObject( m_ImageRepresentationSagittal3D );
  this->m_Display3D->RequestRemoveObject( m_ImageRepresentationCoronal3D );

  // this->DisplayAxial->Update();
  // this->DisplaySagittal->Update();
  // this->DisplayCoronal->Update();
  // this->Display3D->Update();
  
  this->AxialSlider->deactivate();
  this->SagittalSlider->deactivate();
  this->CoronalSlider->deactivate();

  this->ViewerGroup->redraw();
  Fl::check();

}
