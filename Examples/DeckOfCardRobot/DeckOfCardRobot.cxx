/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    DeckOfCardRobot.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "DeckOfCardRobot.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"
#include "igstkEvents.h"
#include "itkImageFileReader.h"

#define PI 3.1415926

namespace igstk
{

/** Constructor: Initializes all internal variables. */
DeckOfCardRobot::DeckOfCardRobot():m_StateMachine(this)
{  
  /** Setup logger, for all other igstk components. */
  logger   = LoggerType::New();

  /** Direct the application log message to the std::cout 
    * and FLTK text display */
  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  Fl_Text_Buffer * textBuffer = new Fl_Text_Buffer();
  this->m_LogWindow->buffer( textBuffer );
  m_LogFLTKOutput = FLTKTextLogOutput::New();
  m_LogFLTKOutput->SetStream( *m_LogWindow );
  this->GetLogger()->AddLogOutput( m_LogFLTKOutput );

  /** Direct the igstk components log message to the file. */
  m_LogFileOutput = LogOutputType::New();
  //FIXME. use a date/time file name
  std::string logFileName = "logDeckOfCardRobot.txt";
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    logger->AddLogOutput( m_LogFileOutput );
    }
  else
    {
    //FIXME. should we return or not
    igstkLogMacro( DEBUG, "Problem opening Log file: " << logFileName << "\n" );
    return;
    }

  /** Initialize all member variables and set logger */
  m_ImageReader = ImageReaderType::New();

  m_Robot = new RobotCommunication();

  m_P1.Fill( 0 );
  m_P2.Fill( 0 );

  for (int i=0; i<3; i++)
    {
    m_Translation[i] = 0.0;
    m_Rotation[i] = 0.0;
    }

  //m_ImageToRobotTransform->SetToIdentity();    //FIXME
  
  m_Annotation2D = Annotation2D::New();
  
  Transform transform;
  transform.SetToIdentity( -1 );

  m_PickedPoint                   = EllipsoidType::New();
  m_PickedPoint->RequestSetTransform( transform );
  m_PickedPointRepresentation     = EllipsoidRepresentationType::New();
  m_PickedPoint->SetRadius( 4, 4, 4 );  
  m_PickedPointRepresentation->RequestSetEllipsoidObject( m_PickedPoint );
  m_PickedPointRepresentation->SetColor(1.0,1.0,0.0);
  m_PickedPointRepresentation->SetOpacity(1.0);

  m_NeedleTip                   = EllipsoidType::New();
  m_NeedleTip->RequestSetTransform( transform );
  m_NeedleTipRepresentation     = EllipsoidRepresentationType::New();
  m_NeedleTip->SetRadius( 4, 4, 4 );  
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

  m_NeedleHolder                    = CylinderType::New();
  m_NeedleHolder->RequestSetTransform( transform );
  m_NeedleHolderRepresentation      = CylinderRepresentationType::New();
  m_NeedleHolder->SetRadius( 8 );   //   16 mm
  m_NeedleHolder->SetHeight( 45 );   // 45 mm
  m_NeedleHolderRepresentation->RequestSetCylinderObject( m_NeedleHolder );
  m_NeedleHolderRepresentation->SetColor(1.0,1.0,0.0);
  m_NeedleHolderRepresentation->SetOpacity(0.5);

  m_Box                   = BoxType::New();
  m_Box->RequestSetTransform( transform );
  m_BoxRepresentation     = BoxRepresentationType::New();
  m_Box->SetSize( 38, 38, 1 );
  m_BoxRepresentation->RequestSetBoxObject( m_Box );
  m_BoxRepresentation->SetColor(1.0, 0.0, 1.0);
  m_BoxRepresentation->SetOpacity( 0.5 );

  m_TargetPoint                 = EllipsoidType::New();
  m_TargetPoint->RequestSetTransform( transform );
  m_TargetRepresentation        = EllipsoidRepresentationType::New();
  m_TargetPoint->SetRadius( 3, 3, 3 );
  m_TargetRepresentation->RequestSetEllipsoidObject( m_TargetPoint );
  m_TargetRepresentation->SetColor( 1.0, 0.0, 0.0);
  m_TargetRepresentation->SetOpacity( 1 );
 
  m_EntryPoint                  = EllipsoidType::New();
  m_EntryPoint->RequestSetTransform( transform );
  m_EntryRepresentation         = EllipsoidRepresentationType::New();
  m_EntryPoint->SetRadius( 3, 3, 3 );
  m_EntryRepresentation->RequestSetEllipsoidObject( m_EntryPoint );
  m_EntryRepresentation->SetColor( 0.0, 0.0, 1.0);
  m_EntryRepresentation->SetOpacity( 1 );

  m_Path                       = PathType::New();
  TubePointType p;
  p.SetPosition( 0, 0, 0);
  p.SetRadius( 1 );
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
  m_PathRepresentation3D->SetOpacity( 1 );

  m_ViewPickerObserver = ObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this, 
                                            &DeckOfCardRobot::DrawPickedPoint );

  this->DisplayAxial->RequestSetOrientation( igstk::View2D::Axial );
  this->DisplaySagittal->RequestSetOrientation( igstk::View2D::Sagittal );
  this->DisplayCoronal->RequestSetOrientation( igstk::View2D::Coronal );

  m_ImageRepresentationAxial    = ImageRepresentationType::New();
  m_ImageRepresentationSagittal = ImageRepresentationType::New();
  m_ImageRepresentationCoronal  = ImageRepresentationType::New();
  m_ImageRepresentation3D       = VolumeRepresentationType::New();
  m_ImageRepresentationOblique  = ObliqueRepresentationType::New();

  /** Set logger */
  if(0) // Temporary disable this logger
    {
    m_ImageReader->SetLogger( logger );
    this->DisplayAxial->SetLogger( logger );
    this->DisplaySagittal->SetLogger( logger );
    this->DisplayCoronal->SetLogger( logger );
    this->Display3D->SetLogger( logger );
    m_ImageRepresentationAxial->SetLogger( logger );
    m_ImageRepresentationSagittal->SetLogger( logger );
    m_ImageRepresentationCoronal->SetLogger( logger );
    m_ImageRepresentation3D->SetLogger( logger );
    m_ImageRepresentationOblique->SetLogger( logger );
    }

  this->ObserveAxialBoundsInput(    m_ImageRepresentationAxial    );
  this->ObserveSagittalBoundsInput( m_ImageRepresentationSagittal );
  this->ObserveCoronalBoundsInput(  m_ImageRepresentationCoronal  );

  /** Initialize  Machine */
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( WaitingForDICOMDirectory   );
  igstkAddStateMacro( ImageReady                 );
  igstkAddStateMacro( AttemptingRegistration     );
  igstkAddStateMacro( LandmarkRegistrationReady  );
  igstkAddStateMacro( TargetPointReady           );
  igstkAddStateMacro( PathReady                  );
  igstkAddStateMacro( AttemptingConnectToRobot   );
  igstkAddStateMacro( RobotConnected             );
  igstkAddStateMacro( AttemptingHomeRobot        );
  igstkAddStateMacro( RobotHomed                 );
  igstkAddStateMacro( AttemptingTargetingRobot   );
  igstkAddStateMacro( RobotReady                 );


  igstkAddInputMacro( RequestLoadImage             );
  igstkAddInputMacro( LoadImageSuccess             );
  igstkAddInputMacro( LoadImageFailure             );

  igstkAddInputMacro( RequestRegistration          );
  igstkAddInputMacro( RegistrationSuccess          );
  igstkAddInputMacro( RegistrationFailure          );

  igstkAddInputMacro( RequestSetTargetPoint        );
  igstkAddInputMacro( RequestSetEntryPoint         );

  igstkAddInputMacro( RequestConnectToRobot        );
  igstkAddInputMacro( ConnectToRobotSuccess        );
  igstkAddInputMacro( ConnectToRobotFailure        );

  igstkAddInputMacro( RequestHomeRobot             );
  igstkAddInputMacro( HomeRobotSuccess             );
  igstkAddInputMacro( HomeRobotFailure             );

  igstkAddInputMacro( RequestTargetingRobot        );  
  igstkAddInputMacro( TargetingRobotSuccess        );
  igstkAddInputMacro( TargetingRobotFailure        );

  igstkAddInputMacro( AxialBounds                  );
  igstkAddInputMacro( SagittalBounds               );
  igstkAddInputMacro( CoronalBounds                );

  /** Load and Displayimage */
  igstkAddTransitionMacro( Initial, RequestLoadImage, 
                                          WaitingForDICOMDirectory, LoadImage );
  igstkAddTransitionMacro( ImageReady, RequestLoadImage, 
                                          WaitingForDICOMDirectory, LoadImage );
  igstkAddTransitionMacro( WaitingForDICOMDirectory, LoadImageSuccess, 
                                       ImageReady, ConnectImageRepresentation );
  igstkAddTransitionMacro( WaitingForDICOMDirectory, LoadImageFailure, 
                                                                  Initial, No );

  /** Receive Slice Information from the Image Representation */
  igstkAddTransitionMacro( ImageReady, AxialBounds, 
                                      ImageReady, SetAxialSliderBounds );
  igstkAddTransitionMacro( ImageReady, SagittalBounds, 
                                   ImageReady, SetSagittalSliderBounds );
  igstkAddTransitionMacro( ImageReady, CoronalBounds, 
                                    ImageReady, SetCoronalSliderBounds );
    
  /** Registration */
  igstkAddTransitionMacro( ImageReady, RequestRegistration, 
                                         AttemptingRegistration, Registration );
  
  igstkAddTransitionMacro( AttemptingRegistration, RegistrationSuccess, 
                                                LandmarkRegistrationReady, No );

  igstkAddTransitionMacro( AttemptingRegistration, RegistrationFailure, 
                                                               ImageReady, No );

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

  /** Robot commanding */
  igstkAddTransitionMacro( Initial, RequestConnectToRobot, 
                            AttemptingConnectToRobot, ConnectToRobot ); //FIXME
  igstkAddTransitionMacro( PathReady, RequestConnectToRobot, 
                                     AttemptingConnectToRobot, ConnectToRobot );

  igstkAddTransitionMacro( AttemptingConnectToRobot, ConnectToRobotSuccess, 
                                                           RobotConnected, No );

  igstkAddTransitionMacro( AttemptingConnectToRobot, ConnectToRobotFailure, 
                                                                PathReady, No );

  igstkAddTransitionMacro( RobotConnected, RequestHomeRobot, 
                                              AttemptingHomeRobot, HomeRobot );

  igstkAddTransitionMacro( AttemptingHomeRobot, HomeRobotSuccess, 
                                                              RobotHomed, No );

  igstkAddTransitionMacro( AttemptingHomeRobot, HomeRobotFailure, 
                                                          RobotConnected, No );

  igstkAddTransitionMacro( RobotHomed, RequestTargetingRobot, 
                                    AttemptingTargetingRobot, TargetingRobot );

  igstkAddTransitionMacro( AttemptingTargetingRobot, TargetingRobotSuccess, 
                                                              RobotReady, No );

  igstkAddTransitionMacro( AttemptingTargetingRobot, TargetingRobotFailure, 
                                                   RobotConnected, HomeRobot );

  igstkAddTransitionMacro( RobotHomed, RequestHomeRobot, 
                                               AttemptingHomeRobot, HomeRobot );
  igstkAddTransitionMacro( RobotReady, RequestHomeRobot, 
                                               AttemptingHomeRobot, HomeRobot );
  igstkAddTransitionMacro( RobotReady, RequestTargetingRobot, 
                                     AttemptingTargetingRobot, TargetingRobot );

  igstkAddTransitionMacro( RobotConnected, RequestSetTargetPoint, 
                                                    RobotConnected, DrawPath );
  igstkAddTransitionMacro( RobotConnected, RequestSetEntryPoint, 
                                                    RobotConnected, DrawPath );
  igstkAddTransitionMacro( RobotHomed, RequestSetTargetPoint, 
                                                        RobotHomed, DrawPath );
  igstkAddTransitionMacro( RobotHomed, RequestSetEntryPoint, 
                                                        RobotHomed, DrawPath );
  igstkAddTransitionMacro( RobotReady, RequestSetTargetPoint, 
                                                        RobotReady, DrawPath );
  igstkAddTransitionMacro( RobotReady, RequestSetEntryPoint, 
                                                        RobotReady, DrawPath );
  
  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();

  std::ofstream ofile;
  ofile.open("DemoApplicationStateMachineDiagram.dot");
 
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );

  ofile.close();

}

/** Destructor */
DeckOfCardRobot::~DeckOfCardRobot()
{
 
}

/** Method to be invoked when no operation is required */
void DeckOfCardRobot::NoProcessing()
{
}


void DeckOfCardRobot::RequestLoadImage()
{
  igstkLogMacro2( logger, DEBUG, 
                    "DeckOfCardRobot::RequestLoadImageProcessing called...\n" )
  m_StateMachine.PushInput( m_RequestLoadImageInput );
  m_StateMachine.ProcessInputs();
}

void DeckOfCardRobot::LoadImageProcessing()
{
  igstkLogMacro2( logger, DEBUG, 
                           "DeckOfCardRobot::LoadImageProcessing called...\n" )
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  if ( directoryname != NULL )
    {
    igstkLogMacro( DEBUG, 
                      "Set ImageReader directory: " << directoryname << "\n" )
    igstkLogMacro2( logger, DEBUG, 
                            "m_ImageReader->RequestSetDirectory called...\n" )
    m_ImageReader->RequestSetDirectory( directoryname ); 

    igstkLogMacro( DEBUG, "ImageReader loading images... \n" )
    igstkLogMacro2( logger, DEBUG, 
                              "m_ImageReader->RequestReadImage called... \n" )
    m_ImageReader->RequestReadImage();

    ImageSpatialObjectObserver::Pointer imageSpatialObjectObserver 
                                           = ImageSpatialObjectObserver::New();
    m_ImageReader->AddObserver( ImageReaderType::ImageModifiedEvent(),
                                                  imageSpatialObjectObserver );
    m_ImageReader->RequestGetImage();

    if(!imageSpatialObjectObserver->GotImageSpatialObject())
      {
      igstkLogMacro(          DEBUG, "Cannot read image\n" )
      igstkLogMacro2( logger, DEBUG, "Cannot read image\n" )
      m_StateMachine.PushInput( m_LoadImageFailureInput);
      }
    else
      {
      igstkLogMacro(          DEBUG, "Image Loaded\n" )
      igstkLogMacro2( logger, DEBUG, "Image Loaded\n" )
      m_StateMachine.PushInput( m_LoadImageSuccessInput);
      m_ImageSpatialObject = 
                           imageSpatialObjectObserver->GetImageSpatialObject();
      }
    }
  else
    {
    igstkLogMacro(          DEBUG, "No directory is selected\n" )
    igstkLogMacro2( logger, DEBUG, "No directory is selected\n" )
    m_StateMachine.PushInput( m_LoadImageFailureInput );
    }
}

void DeckOfCardRobot::RequestSetROI()
{
  m_P1 = m_P2;
  // Get the picked point
  ImageSpatialObjectType::PointType    p;
  p[0] = m_ImageLandmarkTransformToBeSet.GetTranslation()[0];
  p[1] = m_ImageLandmarkTransformToBeSet.GetTranslation()[1];
  p[2] = m_ImageLandmarkTransformToBeSet.GetTranslation()[2];

  ITKImageType::IndexType index;
  m_ImageSpatialObject->TransformPhysicalPointToIndex( p, index);
  m_P2 = index;
}

void DeckOfCardRobot::RequestRegistration()
{
  igstkLogMacro2( logger, DEBUG, 
                         "DeckOfCardRobot::RequestRegistration called ... \n" )
  m_StateMachine.PushInput( m_RequestRegistrationInput );
  m_StateMachine.ProcessInputs();
}

void DeckOfCardRobot::RegistrationProcessing()
{
  ITKImageType::IndexType start;
  ITKImageType::SizeType  size;
  start = m_P1;

  for (int i=0; i<3; i++)
    {
    if ( start[i] > m_P2[i] )
      {
      start[i] = m_P2[i];
      }
    size[i] = abs( m_P2[i] - m_P1[i] );
    }

  m_Registration = new DOCR_Registration( m_ImageSpatialObject,start, size);
  if ( m_Registration->compute() )
    {
    m_ImageToRobotTransform = m_Registration->m_transform; 
    m_RobotToImageTransform = DOCR_Registration::TransformType::New();
    m_ImageToRobotTransform->GetInverse( m_RobotToImageTransform );
    
    m_RobotTransformToBeSet.SetTranslationAndRotation(
                                        m_RobotToImageTransform->GetOffset(), 
                                        m_RobotToImageTransform->GetVersor(),
                                        m_Registration->m_meanRegistrationError,
                                        -1);

    this->RegistrationError->value( m_Registration->m_meanRegistrationError );
    m_Needle->RequestSetTransform( m_RobotTransformToBeSet ); 
    m_NeedleTip->RequestSetTransform( m_RobotTransformToBeSet );
    m_NeedleHolder->RequestSetTransform( m_RobotTransformToBeSet );
    m_Box->RequestSetTransform( m_RobotTransformToBeSet );
    m_RobotTransform = m_RobotTransformToBeSet;
    m_RobotCurrentTransform = m_RobotTransformToBeSet;
    Fl::wait(0.01);
    igstk::PulseGenerator::CheckTimeouts();
    igstkPushInputMacro( RegistrationSuccess );
    }
  else
    {
    igstkPushInputMacro( RegistrationFailure );
    }
}

void DeckOfCardRobot::RequestResliceImage()
{
  igstkLogMacro2( logger, DEBUG, 
                          "DeckOfCardRobot::RequestResliceImage called ... \n")
  this->ResliceImage(); // Take out the state machine logic from here
}

void DeckOfCardRobot::ResliceImage()
{
  m_ImageRepresentationAxial->RequestSetSliceNumber( 
                    static_cast< unsigned int >( this->AxialSlider->value() ) );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( 
                 static_cast< unsigned int >( this->SagittalSlider->value() ) );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( 
                  static_cast< unsigned int >( this->CoronalSlider->value() ) );

  this->ViewerGroup->redraw();
  Fl::check();
}

void DeckOfCardRobot::ResliceImage ( ITKImageType::IndexType index )
{
  m_ImageRepresentationAxial->RequestSetSliceNumber( index[2] );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( index[0] );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( index[1] );

  this->AxialSlider->value( index[2] );
  this->SagittalSlider->value( index[0] );
  this->CoronalSlider->value( index[1] );

  this->ViewerGroup->redraw();
  Fl::check();
}

/** This method should be invoked by the State Machine 
 *  only when the Image has been loaded and the Patient
 *  name has been verified */
void DeckOfCardRobot::ConnectImageRepresentationProcessing()
{
  m_Annotation2D->RequestAddAnnotationText( 0, "Georgetown ISIS Center" );

  m_ImageRepresentationAxial->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );
  m_ImageRepresentationSagittal->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );
  m_ImageRepresentationCoronal->RequestSetImageSpatialObject( 
                                            m_ImageSpatialObject );
  m_ImageRepresentation3D->RequestSetImageSpatialObject( m_ImageSpatialObject );
 
  m_ImageRepresentationAxial->RequestSetOrientation( 
                                               ImageRepresentationType::Axial );
  m_ImageRepresentationSagittal->RequestSetOrientation( 
                                            ImageRepresentationType::Sagittal );
  m_ImageRepresentationCoronal->RequestSetOrientation( 
                                             ImageRepresentationType::Coronal );

  this->DisplayAxial->RequestAddObject( m_ImageRepresentationAxial );
  this->DisplayAxial->RequestAddObject( m_PickedPointRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject( m_NeedleHolderRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject( m_BoxRepresentation->Copy() );
  this->DisplayAxial->RequestAddAnnotation2D( m_Annotation2D );

  this->DisplaySagittal->RequestAddObject( m_ImageRepresentationSagittal );
  this->DisplaySagittal->RequestAddObject(
                                          m_PickedPointRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( 
                                         m_NeedleHolderRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( m_BoxRepresentation->Copy() );
  this->DisplaySagittal->RequestAddAnnotation2D( m_Annotation2D );

  this->DisplayCoronal->RequestAddObject( m_ImageRepresentationCoronal );
  this->DisplayCoronal->RequestAddObject( m_PickedPointRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_NeedleRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( 
                                         m_NeedleHolderRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject( m_BoxRepresentation->Copy() );
  this->DisplayCoronal->RequestAddAnnotation2D( m_Annotation2D );

  this->Display3D->RequestAddObject( m_ImageRepresentation3D );
  this->Display3D->RequestAddObject( m_PickedPointRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_NeedleTipRepresentation->Copy() );
  this->Display3D->RequestAddObject( m_NeedleRepresentation->Copy() );
  CylinderRepresentationType::Pointer needleHolderRep 
                                    = m_NeedleHolderRepresentation->Copy();
  needleHolderRep->SetOpacity(1.0);
  this->Display3D->RequestAddObject( needleHolderRep );
  BoxRepresentationType::Pointer boxRep = m_BoxRepresentation->Copy();
  boxRep->SetOpacity(1.0);
  this->Display3D->RequestAddObject( boxRep );
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

  this->SetROI->activate();

  //Add the picker observer
  this->DisplayAxial->AddObserver( TransformModifiedEvent(), 
                                   m_ViewPickerObserver );
  this->DisplaySagittal->AddObserver( TransformModifiedEvent(), 
                                   m_ViewPickerObserver );
  this->DisplayCoronal->AddObserver( TransformModifiedEvent(), 
                                   m_ViewPickerObserver );

}

  
void DeckOfCardRobot::SetAxialSliderBoundsProcessing()
{
  igstkLogMacro( DEBUG, "SetAxialSliderBounds() " << "\n");
  /** Initialize the slider */
  const unsigned int min = m_AxialBoundsInputToBeSet.minimum;
  const unsigned int max = m_AxialBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
  m_ImageRepresentationAxial->RequestSetSliceNumber( slice );
  this->AxialSlider->minimum( min );
  this->AxialSlider->maximum( max );
  this->AxialSlider->value( slice );  
  this->AxialSlider->activate();

  this->ViewerGroup->redraw();
  Fl::check();
}
    

void DeckOfCardRobot::SetSagittalSliderBoundsProcessing()
{
  igstkLogMacro( DEBUG, "SetSagittalSliderBounds() " << "\n");
  /** Initialize the slider */
  const unsigned int min = m_SagittalBoundsInputToBeSet.minimum;
  const unsigned int max = m_SagittalBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
  m_ImageRepresentationSagittal->RequestSetSliceNumber( slice );
  this->SagittalSlider->minimum( min );
  this->SagittalSlider->maximum( max );
  this->SagittalSlider->value( slice );  
  this->SagittalSlider->activate();

  this->ViewerGroup->redraw();
  Fl::check();
}
    

void DeckOfCardRobot::SetCoronalSliderBoundsProcessing()
{
  igstkLogMacro( DEBUG, "SetCoronalSliderBounds() " << "\n");
  const unsigned int min = m_CoronalBoundsInputToBeSet.minimum;
  const unsigned int max = m_CoronalBoundsInputToBeSet.maximum; 
  const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
  m_ImageRepresentationCoronal->RequestSetSliceNumber( slice );
  this->CoronalSlider->minimum( min );
  this->CoronalSlider->maximum( max );
  this->CoronalSlider->value( slice );  
  this->CoronalSlider->activate();

  this->ViewerGroup->redraw();
  Fl::check();
}


void DeckOfCardRobot::DrawPickedPoint( const itk::EventObject & event)
{
  if ( TransformModifiedEvent().CheckEvent( &event ) )
    {
    TransformModifiedEvent *tmevent = ( TransformModifiedEvent *) & event;
    
    ImageSpatialObjectType::PointType    p;
    p[0] = tmevent->Get().GetTranslation()[0];
    p[1] = tmevent->Get().GetTranslation()[1];
    p[2] = tmevent->Get().GetTranslation()[2];
    
    if( m_ImageSpatialObject->IsInside( p ) )
      {
      m_ImageLandmarkTransformToBeSet = tmevent->Get();
      
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

void DeckOfCardRobot::RequestSetTargetPoint()
{
  igstkLogMacro ( DEBUG, "DeckOfCardRobot::RequestSetTargetPoint called....\n" )
  igstkLogMacro2( logger, DEBUG, 
                         "DeckOfCardRobot::RequestSetTargetPoint called....\n" )
  m_TargetTransform = m_ImageLandmarkTransformToBeSet;
  m_StateMachine.PushInput( m_RequestSetTargetPointInput );
  m_StateMachine.ProcessInputs();
}
void DeckOfCardRobot::RequestSetEntryPoint()
{
  igstkLogMacro ( DEBUG, "DeckOfCardRobot::RequestSetEntryPoint called....\n" )
  igstkLogMacro2( logger, DEBUG, 
                          "DeckOfCardRobot::RequestSetEntryPoint called....\n" )
  m_EntryTransform = m_ImageLandmarkTransformToBeSet;
  m_StateMachine.PushInput( m_RequestSetEntryPointInput );
  m_StateMachine.ProcessInputs();
}

void DeckOfCardRobot::DrawTargetPointProcessing()
{
  m_TargetPoint->RequestSetTransform( m_TargetTransform );
}

void DeckOfCardRobot::DrawPathProcessing()
{
  m_TargetPoint->RequestSetTransform( m_TargetTransform );
  m_EntryPoint->RequestSetTransform( m_EntryTransform );

  float a = 0.0;
  if( this->CalculateRobotMovement() )
    {
    a = 0.4;
    m_Path->Clear();
    //m_Annotation2D->RequestAddAnnotationText( 1, "" ); // FIXME

    TubePointType p;
    Transform::VectorType v;

    v = m_EntryTransform.GetTranslation();
    p.SetPosition( v[0], v[1], v[2]);
    p.SetRadius( 1 );
    m_Path->AddPoint( p );

    v = m_TargetTransform.GetTranslation();
    p.SetPosition( v[0], v[1], v[2]);
    p.SetRadius( 1.01 ); //FIXME
    m_Path->AddPoint( p );

    this->DisplayAxial->RequestRemoveObject( m_PathRepresentationAxial );
    this->DisplaySagittal->RequestRemoveObject( m_PathRepresentationSagittal );
    this->DisplayCoronal->RequestRemoveObject( m_PathRepresentationCoronal );
    this->Display3D->RequestRemoveObject( m_PathRepresentation3D );

    m_PathRepresentationAxial->RequestSetTubeObject( NULL );
    m_PathRepresentationAxial->RequestSetTubeObject( m_Path );
    m_PathRepresentationAxial->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentationAxial->SetOpacity( a );
    m_PathRepresentationSagittal->RequestSetTubeObject( NULL );
    m_PathRepresentationSagittal->RequestSetTubeObject( m_Path );
    m_PathRepresentationSagittal->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentationSagittal->SetOpacity( a );
    m_PathRepresentationCoronal->RequestSetTubeObject( NULL );
    m_PathRepresentationCoronal->RequestSetTubeObject( m_Path );
    m_PathRepresentationCoronal->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentationCoronal->SetOpacity( a );
    m_PathRepresentation3D->RequestSetTubeObject( NULL );
    m_PathRepresentation3D->RequestSetTubeObject( m_Path );
    m_PathRepresentation3D->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentation3D->SetOpacity( 1 );

    this->DisplayAxial->RequestAddObject( m_PathRepresentationAxial );
    this->DisplaySagittal->RequestAddObject( m_PathRepresentationSagittal );
    this->DisplayCoronal->RequestAddObject( m_PathRepresentationCoronal );
    this->Display3D->RequestAddObject( m_PathRepresentation3D );

    AnimateRobotMove( m_RobotCurrentTransform, m_RobotTransformToBeSet, 20);
    //m_NeedleTip->RequestSetTransform( m_RobotTransformToBeSet );
    //m_Needle->RequestSetTransform( m_RobotTransformToBeSet ); 
    //m_NeedleHolder->RequestSetTransform( m_RobotTransformToBeSet );
    m_RobotCurrentTransform = m_RobotTransformToBeSet;

    Transform::VectorType  vect = m_TargetTransform.GetTranslation()
                                    - m_RobotCurrentTransform.GetTranslation();
    double distance = vect.GetNorm();
    this->TargetDistance->value( distance );
    this->NeedleSlider->maximum( (int) (distance+5) );
    this->NeedleSlider->value( 0 );
    this->NeedleSlider->activate();

    //this->CreateObliqueView();

    this->ViewerGroup->redraw();
    Fl::check();
    }
  else
    {
    m_PathRepresentationAxial->SetOpacity( a );
    m_PathRepresentationSagittal->SetOpacity( a );
    m_PathRepresentationCoronal->SetOpacity( a );
    m_PathRepresentation3D->SetOpacity( a );

    m_NeedleTip->RequestSetTransform( m_RobotTransform );
    m_Needle->RequestSetTransform( m_RobotTransform ); 
    m_NeedleHolder->RequestSetTransform( m_RobotTransform );
    m_RobotCurrentTransform = m_RobotTransform;
    this->NeedleSlider->maximum(0);
    this->NeedleSlider->value( 0 );
    this->NeedleSlider->deactivate();
    // FIXME
    //m_Annotation2D->RequestAddAnnotationText( 1, "Unreachable position" );

    //this->DisableObliqueView();

    this->ViewerGroup->redraw();
    Fl::check();
    std::cout << "Path not reachable by robot" << std::endl;
    }
}

void DeckOfCardRobot::RequestConnectToRobot()
{
  igstkLogMacro2( logger, DEBUG, 
                "DeckOfCardRobot::RequestConnectToRobot called...\n" )
                igstkPushInputMacro( RequestConnectToRobot );
  m_StateMachine.ProcessInputs();
}

void DeckOfCardRobot::RequestHomeRobot()
{
  igstkLogMacro2( logger, DEBUG, 
                "DeckOfCardRobot::RequestHomeRobot called...\n" )
                igstkPushInputMacro( RequestHomeRobot );
  m_StateMachine.ProcessInputs();
}

void DeckOfCardRobot::RequestTargetingRobot()
{
  igstkLogMacro2( logger, DEBUG, 
                "DeckOfCardRobot::RequestTargetingRobot called...\n" )
                igstkPushInputMacro( RequestTargetingRobot );
  m_StateMachine.ProcessInputs();
}

void DeckOfCardRobot::ConnectToRobotProcessing()
{
  igstkLogMacro2( logger, DEBUG, 
               "DeckOfCardRobot::ConnectToRobotProcessing called...\n" )
  m_StateMachine.PushInputBoolean( m_Robot->Init(), 
                    m_ConnectToRobotSuccessInput, m_ConnectToRobotFailureInput);
}

void DeckOfCardRobot::HomeRobotProcessing()
{
  igstkLogMacro2( logger, DEBUG, 
               "DeckOfCardRobot::HomeRobotProcessing called...\n" )
  m_StateMachine.PushInputBoolean( m_Robot->Home(), 
                              m_HomeRobotSuccessInput, m_HomeRobotFailureInput);
  
  // FIXME --> Needle State Machine Logic, success/failure
  m_Needle->RequestSetTransform( m_RobotTransform ); 
  m_NeedleTip->RequestSetTransform( m_RobotTransform );
  m_NeedleHolder->RequestSetTransform( m_RobotTransform );
  m_RobotCurrentTransform = m_RobotTransform;
  Fl::wait(0.01);
  igstk::PulseGenerator::CheckTimeouts();
  //  AnimateRobotMove( m_RobotCurrentTransform, m_RobotTransform, 20 );
  //  Fl::wait(0.01);
  //  igstk::PulseGenerator::CheckTimeouts();

}

void DeckOfCardRobot::TargetingRobotProcessing()
{

  igstkLogMacro2( logger, DEBUG, 
               "DeckOfCardRobot::TargetingRobotProcessing called...\n" )

  std::cout << "Robot translation: " << m_Translation[0] << "," 
               << m_Translation[1] << std::endl;
  std::cout << "Robot rotation: " << m_Rotation[0] << "," 
               << m_Rotation[1] << std::endl;
  
  m_StateMachine.PushInputBoolean( 
                   m_Robot->MoveRobotCoordinates( m_Translation, m_Rotation ), 
                  m_TargetingRobotSuccessInput, m_TargetingRobotFailureInput);

  // FIXME --> Needle State Machine Logic, success/failure
  m_Needle->RequestSetTransform( m_RobotTransformToBeSet ); 
  m_NeedleTip->RequestSetTransform( m_RobotTransformToBeSet );
  m_NeedleHolder->RequestSetTransform( m_RobotTransformToBeSet );
  m_RobotCurrentTransform = m_RobotTransformToBeSet;
  Fl::wait(0.01);
  igstk::PulseGenerator::CheckTimeouts();

//  AnimateRobotMove( m_RobotCurrentTransform, m_RobotTransformToBeSet, 20 );
//  m_RobotCurrentTransform = m_RobotTransformToBeSet;
//  Fl::wait(0.01);
//  igstk::PulseGenerator::CheckTimeouts();
}

bool DeckOfCardRobot::CalculateRobotMovement()
{
  Transform               transform;
  Transform::VersorType   rotation;
  Transform::VectorType   translation, pVect1, pVect2, pProject, axis;
  double                  angle;
  DOCR_Registration::TransformType::InputPointType rPE, rPT, pIntersect;

  for (int i=0; i<3; i++)
    {
    rPE[i] = m_EntryTransform.GetTranslation()[i];
    rPT[i] = m_TargetTransform.GetTranslation()[i];
    }

  // In Robot coordinate system
  rPE = m_ImageToRobotTransform->TransformPoint( rPE );
  rPT = m_ImageToRobotTransform->TransformPoint( rPT );

  // Calculate the intersect of a line with a plane => robot translation
  // Planed Path equation:  p = rPT + u * ( rPE - rPT)
  // Robot Plane equation:  p * normal = k;  => normal = {0,0,1}, k = 0
  // u = (k - rPT * normal) / ( (rPE - rPT) * normal)
  // u = -rPT[2]/ (rPE[2] - rPT[2])
  double u = - rPT[2] / ( rPE[2] - rPT[2]);
  pIntersect = rPT + ( rPE - rPT ) * u;

  // Calculate robot rotation axis-angle
  pVect1[0] = 0;  pVect1[1] = 0;  pVect1[2] = 1;  // Needle Axis
  pVect2 = rPE - rPT;                             // Path vector
  pVect2.Normalize();
  if ( pVect1 * pVect2 < 0)
    {
    pVect2 *= -1;
    }

  angle = acos( pVect1 * pVect2 );
  axis = itk::CrossProduct( pVect1, pVect2);
  axis.Normalize();

  // Compose the new robot transform
  for (int i=0; i<3; i++)
    {
    translation[i] = pIntersect[i];  // Translation in robot space
    }

  // Translate angle-axis into quaternion
  rotation.Set( axis, angle );
  rotation.Normalize();

  // Transform it to CT coordinate system
  rotation = m_RobotTransform.GetRotation()*rotation;
  translation = m_RobotTransform.GetRotation().Transform(translation);
  translation += m_RobotTransform.GetTranslation();

  m_RobotTransformToBeSet.SetTranslationAndRotation( translation, rotation, 
    0.1, -1);

  /** Robot Movement Code-------------------------------------------------*/
  // Robot translational movement
  // No flip necessary in case of correct needle holder orientation:
  //     "R" must point towards Robot, i.e. top-level fiducial has to be in
  //     needle holder space with NEGATIVE x and POSITIVE y
  m_Translation[0] = pIntersect[0]; 
  m_Translation[1] = pIntersect[1]; 
  m_Translation[2] = 0;

  /************************************************************************/
  /* Projection Angle                                                     */
  /************************************************************************/
  
  // Rotation along X axis
  pProject = pVect2; 
  pProject[0] = 0;        // Projection to YZ plane
  pProject.Normalize();
  m_Rotation[0] = acos( pVect1 * pProject) * 180 /PI;
  if ( pProject[1] > 0)
    {
    m_Rotation[0] *= -1;
    }

  // Rotation along Y axis
  pProject = pVect2;
  pProject[1] = 0;        // Projection to XZ plane
  pProject.Normalize();
  m_Rotation[1] = acos( pVect1 * pProject) * 180 /PI;
  if ( pProject[0] < 0)
    {
    m_Rotation[1] *= -1;
    }
  
  /************************************************************************/
  /* Euler Angle                                                          */
  /************************************************************************/
  
  if(0)
  {
  // Translate angle-axis to two rotation around X and Y axis
  int rotateXfirst = false;  //Order of rotation Tx*Ty  or Ty*Tx
  double c = cos( angle );
  double t  = 1-c;
  double s = sin( angle );
  double x, y, z;
  x = axis[0];  y=axis[1]; z=axis[2];

  if ( rotateXfirst )
    {
    m_Rotation[0] =  atan2( t*y*z + x*s,  t*y*y + c );
    m_Rotation[1] =  atan2( t*x*z + y*s,  t*x*x + c );
    }
    else
    {
      m_Rotation[0] = - atan2( t*y*z - x*s,  t*y*y + c );
      m_Rotation[1] = - atan2( t*x*z - y*s,  t*x*x + c );
    }
  
  for (int i=0; i<3; i++)
    {
    m_Rotation[i] = m_Rotation[i] * 180 / PI;  // To degree
    if ( m_Rotation[i] > 90)
      {
      m_Rotation[i] = m_Rotation[i] - 180;    // Flip the rotation angle
      }
    }
  }  
  
  // Output Result
  std::cout << "Robot translation: " << m_Translation[0] << "," 
                                     << m_Translation[1] << std::endl;
  std::cout << "Robot rotation: " << m_Rotation[0] << "," 
                                  << m_Rotation[1] << std::endl;
  /** ----------------------------------------------------------------*/


  // Test if the move is reachable
  m_Reachable = true;
  for (int i=0; i<3; i++)
    {
    if ( ( fabs (m_Translation[i]) > 19 ) || ( fabs (m_Rotation[i]) > 30 ) )
      {
      m_Reachable = false;
      break;
      }
    }
  return m_Reachable;
}

void DeckOfCardRobot::RequestInsertNeedle()
{
  // Calculate new transform to be set...
  Transform::VectorType vect, translation;
  
  vect = m_TargetTransform.GetTranslation()
       - m_RobotCurrentTransform.GetTranslation();
  vect.Normalize();

  translation = m_RobotCurrentTransform.GetTranslation() 
              + vect * this->NeedleSlider->value();
  
  m_NeedleTransformToBeSet = m_RobotCurrentTransform;
  m_NeedleTransformToBeSet.SetTranslation( translation, 0.1, -1);

  m_Needle->RequestSetTransform( m_NeedleTransformToBeSet );
  m_NeedleTip->RequestSetTransform( m_NeedleTransformToBeSet );

  // Update target distance
  Transform::VectorType vect2 = m_TargetTransform.GetTranslation()
                              - m_EntryTransform.GetTranslation();
  double distance2 = vect2.GetNorm();

  vect2 = m_TargetTransform.GetTranslation()
        - m_NeedleTransformToBeSet.GetTranslation();

 double distance = vect2.GetNorm(); 
  if( ( vect * vect2) < 0 )
    {
    distance *= -1;
    }

  if ( distance > distance2 )
    {
    TargetDistance->textcolor( FL_BLACK );
    }
  else if ( distance > 10 )
    {
    TargetDistance->textcolor( FL_YELLOW );
    }
  else if ( distance > 0 )
    {
    TargetDistance->textcolor( FL_BLUE );
    }
  else
    {
    TargetDistance->textcolor( FL_RED );
    }
  this->TargetDistance->value( distance );

  // Calculate index
  ImageSpatialObjectType::PointType    p;
  p[0] = translation[0];
  p[1] = translation[1];
  p[2] = translation[2];
  ImageSpatialObjectType::IndexType index;
  m_ImageSpatialObject->TransformPhysicalPointToIndex( p, index);

  ResliceImage( index );

  if ( this->DisplayOblique->visible())
    {
    m_ImageRepresentationOblique->RequestSetOriginPointOnThePlane( p );
    m_ImageRepresentationOblique->RequestReslice();  
    this->DisplayOblique->Update();
    }
}

void DeckOfCardRobot::CreateObliqueView()
{
  Transform::VectorType o, v1, v2, p;
  o.Fill( 0 );
  o = m_RobotCurrentTransform.GetRotation().Transform(o);
  o += m_RobotCurrentTransform.GetTranslation();

  v1.Fill( 0 );
  v1[0] = 1;
  v1 = m_RobotCurrentTransform.GetRotation().Transform(v1);
  v1 += m_RobotCurrentTransform.GetTranslation();
  v1 -= o;
  v1.Normalize();

  v2.Fill( 0 );
  v2[1] = -1;
  v2 = m_RobotCurrentTransform.GetRotation().Transform(v2);
  v2 += m_RobotCurrentTransform.GetTranslation();
  v2 -= o;
  v2.Normalize();

  // Set oblique plane parameters
  m_ImageRepresentationOblique->RequestSetImageSpatialObject( 
                                                         m_ImageSpatialObject );
  ObliqueRepresentationType::PointType  point;
  point[0] = o[0];
  point[1] = o[1];
  point[2] = o[2];

  m_ImageRepresentationOblique->RequestSetOriginPointOnThePlane( point );
  m_ImageRepresentationOblique->RequestSetVector1OnThePlane( v1 );
  m_ImageRepresentationOblique->RequestSetVector2OnThePlane( v2 );
  this->DisplayOblique->RequestAddObject( m_ImageRepresentationOblique );
  m_ImageRepresentationOblique->RequestReslice(); 
  
  this->DisplayOblique->RequestResetCamera();
  this->DisplayOblique->Update();
  this->DisplayOblique->RequestEnableInteractions();
  this->DisplayOblique->RequestSetRefreshRate( 30 ); // 30 Hz
  this->DisplayOblique->RequestStart();  
  
}

void DeckOfCardRobot::DisableObliqueView()
{
  m_ImageRepresentationOblique->RequestSetImageSpatialObject( NULL );
  this->DisplayOblique->RequestRemoveObject( m_ImageRepresentationOblique );
  this->DisplayOblique->RequestDisableInteractions();
  this->DisplayOblique->RequestStop();
}

void DeckOfCardRobot::AnimateRobotMove( Transform TCurrent, Transform TToBeSet, 
                                                                    int steps )
{
  m_Needle->RequestSetTransform( TCurrent );
  m_NeedleTip->RequestSetTransform( TCurrent );
  m_NeedleHolder->RequestSetTransform( TCurrent );
  Fl::wait(0.1);
  igstk::PulseGenerator::CheckTimeouts();

  // Calculate new transform to be set...
  Transform             transform;
  Transform::VectorType vect, axis, translation;
  Transform::VersorType versor, rotation;
  double                angle;
  vect = TToBeSet.GetTranslation() - TCurrent.GetTranslation();
  double distance = vect.GetNorm();
  vect.Normalize();

  versor = TToBeSet.GetRotation() / TCurrent.GetRotation();
  versor.Normalize();
  angle = versor.GetAngle();
  axis = versor.GetAxis();
  axis.Normalize();

  for (int i=0; i<=steps; i++)
    {
    // Interpolate quaternions
    rotation.Set( axis, angle * i/steps );
    rotation.Normalize();
    rotation = rotation * TCurrent.GetRotation();

    // Interpolate translation
    translation = TCurrent.GetTranslation() + vect * (distance * i/steps);
    
    transform.SetTranslationAndRotation( translation, rotation, 0.1, -1 );

    m_NeedleTip->RequestSetTransform( transform );
    m_Needle->RequestSetTransform( transform ); 
    m_NeedleHolder->RequestSetTransform( transform );

    Fl::wait(0.1);
    igstk::PulseGenerator::CheckTimeouts();
    }
}
} // end of namespace
