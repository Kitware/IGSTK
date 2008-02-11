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
#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"

#include "PolarisTrackerConfigurationGUI.h"
#include "AuroraTrackerConfigurationGUI.h"
#include "MicronTrackerConfigurationGUI.h"

/** Constructor: Initializes all internal variables. */
NeedleBiopsy::NeedleBiopsy()
{

  /** Setup logger, for all other igstk components. */
  m_Logger   = LoggerType::New();
  this->GetLogger()->SetTimeStampFormat( itk::LoggerBase::HUMANREADABLE );
  this->GetLogger()->SetHumanReadableFormat("%Y %b %d, %H:%M:%S");
  this->GetLogger()->SetPriorityLevel( LoggerType::INFO );

  /** Direct the application log message to the std::cout
    * and FLTK text display */
  itk::StdStreamLogOutput::Pointer m_LogCoutOutput
                                           = itk::StdStreamLogOutput::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  /** Direct the igstk components log message to the file. */
  itk::StdStreamLogOutput::Pointer m_LogFileOutput
                                           = itk::StdStreamLogOutput::New();
  std::ofstream m_LogFile;
  std::string   logFileName;
  logFileName = "logNeedleBiopsy"
  + itksys::SystemTools::GetCurrentDateTime( "_%Y_%m_%d_%H_%M_%S" ) + ".txt";
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    }
  else
    {
    //FIXME. should we return or not
    igstkLogMacro( DEBUG, "Problem opening Log file:"
                                                    << logFileName << "\n" );
    return;
    }

  /** Initialize all member variables and set logger */
  m_ImageReader           = ImageReaderType::New();
  m_LandmarkRegistration  = RegistrationType::New();
  m_Annotation            = igstk::Annotation2D::New();
  m_WorldReference        = igstk::AxesObject::New();
  m_TrackerInitializerList.clear();

  m_NeedleTip                   = EllipsoidType::New();
  m_NeedleTipRepresentation     = EllipsoidRepresentationType::New();
  m_NeedleTip->SetRadius( 5, 5, 5 );
  m_NeedleTipRepresentation->RequestSetEllipsoidObject( m_NeedleTip );
  m_NeedleTipRepresentation->SetColor(1.0,0.0,0.0);
  m_NeedleTipRepresentation->SetOpacity(1.0);

  m_Needle                    = CylinderType::New();
  m_NeedleRepresentation      = CylinderRepresentationType::New();
  m_Needle->SetRadius( 1.5 );   //   1.5 mm
  m_Needle->SetHeight( 100 );   // 200.0 mm
  m_NeedleRepresentation->RequestSetCylinderObject( m_Needle );
  m_NeedleRepresentation->SetColor(0.0,1.0,0.0);
  m_NeedleRepresentation->SetOpacity(1.0);

  m_TargetPoint                 = EllipsoidType::New();
  m_TargetRepresentation        = EllipsoidRepresentationType::New();
  m_TargetPoint->SetRadius( 6, 6, 6 );
  m_TargetRepresentation->RequestSetEllipsoidObject( m_TargetPoint );
  m_TargetRepresentation->SetColor( 1.0, 0.0, 0.0);
  m_TargetRepresentation->SetOpacity( 0.6 );

  m_EntryPoint                  = EllipsoidType::New();
  m_EntryRepresentation         = EllipsoidRepresentationType::New();
  m_EntryPoint->SetRadius( 6, 6, 6 );
  m_EntryRepresentation->RequestSetEllipsoidObject( m_EntryPoint );
  m_EntryRepresentation->SetColor( 0.0, 0.0, 1.0);
  m_EntryRepresentation->SetOpacity( 0.6 );

  m_FiducialPoint                 = EllipsoidType::New();
  m_FiducialRepresentation        = EllipsoidRepresentationType::New();
  m_FiducialPoint->SetRadius( 6, 6, 6 );
  m_FiducialRepresentation->RequestSetEllipsoidObject( m_FiducialPoint );
  m_FiducialRepresentation->SetColor( 0.0, 1.0, 0.0);
  m_FiducialRepresentation->SetOpacity( 0.6 );


  m_Path                       = PathType::New();
  TubePointType point;
  point.SetPosition( 0, 0, 0);
  point.SetRadius( 2 );
  m_Path->AddPoint( point );
  m_Path->AddPoint( point );

  m_PathRepresentation.clear();
  for( int i=0; i<4; i++ )
    {
    PathRepresentationType::Pointer rep  = PathRepresentationType::New();
    rep->RequestSetTubeObject( m_Path );
    rep->SetColor( 0.0, 1.0, 0.0);
    rep->SetOpacity( 0.4 );
    m_PathRepresentation.push_back( rep );
    }


  m_ViewPickerObserver = LoadedObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this, &NeedleBiopsy::Picking );

  m_ViewResliceObserver = LoadedObserverType::New();
  m_ViewResliceObserver->SetCallbackFunction( this,
                                                &NeedleBiopsy::ResliceImage);

  m_TrackerConfigurationObserver = LoadedObserverType::New();
  m_TrackerConfigurationObserver->SetCallbackFunction( this,
                                    &NeedleBiopsy::RequestInitializeTracker);

  m_ImageRepresentation.clear();
  for (int i=0; i<6; i++)
    {
    ImageRepresentationType::Pointer rep = ImageRepresentationType::New();
    m_ImageRepresentation.push_back( rep );
    }

}

/** Destructor */
NeedleBiopsy::~NeedleBiopsy()
{
}


int NeedleBiopsy::RequestLoadImage()
{
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  if ( directoryname != NULL )
    {
    igstkLogMacro( DEBUG,
      "Set ImageReader directory: " << directoryname << "\n" )
    m_ImageDir = directoryname;
    m_ImageReader->RequestSetDirectory( directoryname );

    igstkLogMacro( DEBUG, "ImageReader loading images... \n" )
    m_ImageReader->RequestReadImage();

    CTImageObserver::Pointer  m_CTImageObserver = CTImageObserver::New();
    m_ImageReader->AddObserver(igstk::CTImageReader::ImageModifiedEvent(),
                                                      m_CTImageObserver);

    m_ImageReader->RequestGetImage();

    if(m_CTImageObserver->GotCTImage())
      {
      igstkLogMacro(          DEBUG, "Image loaded...\n" )
      m_ImageSpatialObject = m_CTImageObserver->GetCTImage();
      this->ConnectImageRepresentation();
      this->ReadTreatmentPlan();
      return 1;
      }
    else
      {
      igstkLogMacro(          DEBUG, "Reading image failure...\n" )
      return 0;
      }
    }
  else
    {
    igstkLogMacro(          DEBUG, "No directory is selected\n" )
    return 0;
    }

}

/** This method should be invoked by the State Machine
*  only when the Image has been loaded and the Patient
*  name has been verified */
void NeedleBiopsy::ConnectImageRepresentation()
{
  m_Annotation->RequestSetAnnotationText( 2, "Georgetown ISIS Center" );
  for( int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestAddAnnotation2D( m_Annotation );
    }

  for( int i=0; i<6; i++)
    {
    m_ImageRepresentation[i]->RequestSetImageSpatialObject(
      m_ImageSpatialObject );
    }

  m_ImageRepresentation[0]->RequestSetOrientation(
    ImageRepresentationType::Axial );
  m_ImageRepresentation[1]->RequestSetOrientation(
    ImageRepresentationType::Sagittal );
  m_ImageRepresentation[2]->RequestSetOrientation(
    ImageRepresentationType::Coronal );

  m_ImageRepresentation[3]->RequestSetOrientation(
    ImageRepresentationType::Axial );
  m_ImageRepresentation[4]->RequestSetOrientation(
    ImageRepresentationType::Sagittal );
  m_ImageRepresentation[5]->RequestSetOrientation(
    ImageRepresentationType::Coronal );

  for ( int i=0; i<3; i++)
    {
    ViewerGroup->m_Views[i]->RequestRemoveObject( m_ImageRepresentation[i] );
    ViewerGroup->m_Views[3]->RequestRemoveObject( m_ImageRepresentation[i+3] );
    ViewerGroup->m_Views[i]->RequestAddObject( m_ImageRepresentation[i] );
    ViewerGroup->m_Views[3]->RequestAddObject( m_ImageRepresentation[i+3] );
    }

  for ( int i=0; i<4; i++)
    {
    igstk::View::Pointer view =  ViewerGroup->m_Views[i];
    view->RequestAddObject( m_NeedleTipRepresentation->Copy() );
    view->RequestAddObject( m_NeedleRepresentation->Copy() );
    view->RequestAddObject( m_TargetRepresentation->Copy() );
    view->RequestAddObject( m_EntryRepresentation->Copy() );
    view->RequestAddObject( m_FiducialRepresentation->Copy() );
    view->RequestAddObject( m_PathRepresentation[i] );
    }

  /** Setting up the sence graph */
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
  for( int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestSetTransformAndParent(
      transform, m_WorldReference );
    }

  m_ImageSpatialObject->RequestSetTransformAndParent(
    transform, m_WorldReference );

  m_EntryPoint->RequestSetTransformAndParent( transform, m_WorldReference );
  m_TargetPoint->RequestSetTransformAndParent( transform, m_WorldReference );
  m_FiducialPoint->RequestSetTransformAndParent( transform, m_WorldReference );
  m_Path->RequestSetTransformAndParent( transform, m_WorldReference );

  /** Reset and enable the view */
  for( int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestResetCamera();
    ViewerGroup->m_Views[i]->SetRefreshRate( 30 );
    ViewerGroup->m_Views[i]->RequestStart();
    ViewerGroup->m_Displays[i]->RequestEnableInteractions();
    }

  // Request information about the slices. The answers will be
  // received in the form of events.
  SliceBoundsObserver::Pointer boundsObs = SliceBoundsObserver::New();
  for ( int i=0; i<3; i++)
    {
    m_ImageRepresentation[i]->AddObserver(
      igstk::IntegerBoundsEvent(), boundsObs);

    m_ImageRepresentation[i]->RequestGetSliceNumberBounds();

    if( boundsObs->GotSliceBounds() )
      {
      const unsigned int min = boundsObs->GetSliceBounds().minimum;
      const unsigned int max = boundsObs->GetSliceBounds().maximum;
      const unsigned int slice =
        static_cast< unsigned int > ( (min + max) / 2.0 );
      m_ImageRepresentation[i]->RequestSetSliceNumber( slice );
      m_ImageRepresentation[i+3]->RequestSetSliceNumber( slice );
      ViewerGroup->m_Sliders[i]->minimum( min );
      ViewerGroup->m_Sliders[i]->maximum( max );
      ViewerGroup->m_Sliders[i]->value( slice );
      ViewerGroup->m_Sliders[i]->activate();
      boundsObs->Reset();
      }
    }

  /** Adding observer for picking event */
  for ( int i=0; i<3; i++)
    {
    ViewerGroup->m_Views[i]->AddObserver(
      igstk::CoordinateSystemTransformToEvent(),
      m_ViewPickerObserver );
    }

  /** Adding observer for slider bar reslicing event*/
  ViewerGroup->AddObserver( igstk::QuadrantViews::ReslicingEvent(),
    m_ViewResliceObserver );
}

void NeedleBiopsy::ReadTreatmentPlan()
{
  igstk::TreatmentPlanIO * reader = new igstk::TreatmentPlanIO;

  m_PlanFilename = m_ImageDir + "_TreatmentPlan.igstk";

  m_Plan = new igstk::TreatmentPlan;

  if (itksys::SystemTools::FileExists( m_PlanFilename.c_str()))
    {
    reader->SetFileName( m_PlanFilename );
    if ( reader->RequestRead( ) )
      {
      m_Plan = reader->GetTreatmentPlan();
      }
    }

  // Populate the choice box
  TPlanPointList->clear();
  TPlanPointList->add( "Entry" );
  TPlanPointList->add( "Target" );
  char buf[10];
  for( int i = 0; i < m_Plan->m_FiducialPoints.size(); i++ )
    {
    sprintf( buf, "Fiducial%i", i+1 );
    TPlanPointList->add( buf );
    }

  // Setting object position according to treatment plan
  m_EntryPoint->RequestSetTransformAndParent( 
    PointToTransform( m_Plan->m_EntryPoint ), m_WorldReference);

  m_TargetPoint->RequestSetTransformAndParent( 
    PointToTransform( m_Plan->m_TargetPoint ), m_WorldReference);

  this->UpdatePath();

  TPlanPointList->value(0);
  ChangeSelectedTPlanPoint();
}

void NeedleBiopsy::WriteTreatmentPlan()
{
  igstk::TreatmentPlanIO * writer = new igstk::TreatmentPlanIO;
  writer->SetFileName( m_PlanFilename );
  writer->SetTreatmentPlan( m_Plan );
  writer->RequestWrite();
}


void NeedleBiopsy::ChangeSelectedTPlanPoint()
{
  ImageSpatialObjectType::PointType    point;
  int choice = TPlanPointList->value();
  if( choice == 0 )
    {
    point = m_Plan->m_EntryPoint;
    }
  else if ( choice == 1 )
    {
    point = m_Plan->m_TargetPoint;
    }
  else
    {
    point = m_Plan->m_FiducialPoints[ choice-2];
    m_FiducialPoint->RequestSetTransformAndParent(
      PointToTransform(point), m_WorldReference );
    }

  char buf[50];
  sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
  m_Annotation->RequestSetAnnotationText(0, buf);
  m_Annotation->RequestSetFontColor(0, 0, 0, 1.0);
  m_Annotation->RequestSetFontSize(0, 12);

  if( m_ImageSpatialObject->IsInside( point ) )
    {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);
    igstkLogMacro( DEBUG, index <<"\n");
    ResliceImage( index );
    }
  else
    {
    igstkLogMacro( DEBUG,  "This point is not defined in the image...\n" )
    }

}

void NeedleBiopsy::RequestConnectToTracker()
{
  switch( ConnectToTrackerBtn->value() )
    {
    case 0:
      {
      igstk::TrackerConfiguration config = igstk::TrackerConfiguration();
      config.SetTrackerType( igstk::TrackerConfiguration::Polaris );
      PolarisTrackerConfigurationGUI * gui;
      m_TrackerConfigurationGUI = gui = new PolarisTrackerConfigurationGUI();
      m_TrackerConfigurationGUI->SetConfiguration( config );
      m_TrackerConfigurationGUI->RemoveAllObservers();
      m_TrackerConfigurationGUI->AddObserver(
        igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
        m_TrackerConfigurationObserver);
      break;
      }
    case 1:
      {
      igstk::TrackerConfiguration config = igstk::TrackerConfiguration();
      config.SetTrackerType( igstk::TrackerConfiguration::Aurora );
      AuroraTrackerConfigurationGUI * gui;
      m_TrackerConfigurationGUI = gui = new AuroraTrackerConfigurationGUI();
      m_TrackerConfigurationGUI->SetConfiguration( config );
      m_TrackerConfigurationGUI->RemoveAllObservers();
      m_TrackerConfigurationGUI->AddObserver(
        igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
        m_TrackerConfigurationObserver);
      break;
      }
    case 2:
      {
      igstk::TrackerConfiguration config = igstk::TrackerConfiguration();
      config.SetTrackerType( igstk::TrackerConfiguration::Micron );
      MicronTrackerConfigurationGUI * gui;
      m_TrackerConfigurationGUI = gui = new MicronTrackerConfigurationGUI();
      m_TrackerConfigurationGUI->SetConfiguration( config );
      m_TrackerConfigurationGUI->RemoveAllObservers();
      m_TrackerConfigurationGUI->AddObserver(
        igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
        m_TrackerConfigurationObserver);
      break;
      }
    }

}

void NeedleBiopsy::RequestInitializeTracker(const itk::EventObject & event)
{
  typedef igstk::TrackerConfigurationGUIBase  GUIType;
  if ( GUIType::ConfigurationEvent().CheckEvent( &event ) )
    {
    GUIType::ConfigurationEvent *confEvent =
                                   ( GUIType::ConfigurationEvent *) & event;

    igstk::TrackerConfiguration  tc = confEvent->Get();

    igstk::TrackerInitializer * initializer = new igstk::TrackerInitializer;
    initializer->SetTrackerConfiguration( & tc );

    if ( initializer->RequestInitializeTracker() )
      { 
      igstk::Tracker::Pointer     tracker = initializer->GetTracker();
      igstk::TrackerTool::Pointer tool = 
                                  initializer->GetNonReferenceToolList()[0];
      igstk::TrackerTool::Pointer refTool = initializer->GetReferenceTool();
      
      // Connect the scene graph with an identity transform first
      igstk::Transform transform;
      transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
      if ( initializer->m_HasReferenceTool )
        {
        refTool->RequestSetTransformAndParent(transform, m_WorldReference);
        }
      else
        {
        tracker->RequestSetTransformAndParent(transform, m_WorldReference);
        }
      m_RegistrationTrackerTool = tool;

      m_TrackerInitializerList.push_back( initializer );
      UpdateTrackerAndTrackerToolList();
      TrackerList->value(m_TrackerInitializerList.size()-1);
      TrackerToolList->value(m_TrackerInitializerList.size()-1);
      }
    }
}

void NeedleBiopsy::UpdateTrackerAndTrackerToolList()
{
  TrackerList->clear();
  TrackerToolList->clear();
  int n = 0;
  std::string s;
  for ( int i=0; i<m_TrackerInitializerList.size(); i++)
    {
      char buf[10];
      _itoa(i+1, buf, 10);
      s = "Tracker ";
      s = s + buf + " [" + 
                  m_TrackerInitializerList[i]->GetTrackerTypeAsString() + "]";
      TrackerList->add( s.c_str() );
      std::vector< igstk::TrackerTool::Pointer > toolList = 
                      m_TrackerInitializerList[i]->GetNonReferenceToolList();
      for ( int j=0; j< toolList.size(); j++)
        {
          char buf[10];
          _itoa(++n, buf, 10);
          s = "Tool ";
          s = s + buf + " [" + 
                  m_TrackerInitializerList[i]->GetTrackerTypeAsString() + "]";
          TrackerToolList->add( s.c_str() );
        }
    }

}

void NeedleBiopsy::RequestDisconnetTracker()
{
  RequestStopTracking();
  int n = TrackerList->value();
  if ( n < m_TrackerInitializerList.size() )
    {
    m_TrackerInitializerList[n]->StopAndCloseTracker();
    m_TrackerInitializerList.erase( m_TrackerInitializerList.begin() + n );
    UpdateTrackerAndTrackerToolList();
    if (m_TrackerInitializerList.size()>0)
      {
        TrackerList->value(0);
        TrackerToolList->value(0);
      }
    ChangeActiveTrackerTool();
    }
}

void NeedleBiopsy::ChangeActiveTrackerTool()
{
  if (m_TrackerInitializerList.size() == 0)
    {
    // Disconnect all observer
    }
  else
    {
    // Hook up approperiate observer
    }

}

void NeedleBiopsy::RequestRegistration()
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
  m_LandmarkRegistration->RequestGetTransformFromTrackerToImage();
}

void NeedleBiopsy::RequestStartTracking()
{
  for (int i=0; i<m_TrackerInitializerList.size(); i++)
    {
    m_TrackerInitializerList[i]->GetTracker()->RequestStartTracking();
    }

  TrackingBtn->label("Stop");
  ControlGroup->redraw();
}

void NeedleBiopsy::RequestStopTracking()
{
  for (int i=0; i<m_TrackerInitializerList.size(); i++)
  {
    m_TrackerInitializerList[i]->GetTracker()->RequestStopTracking();
  }

  TrackingBtn->label("Tracking");
  ControlGroup->redraw();
}

void NeedleBiopsy::ResliceImage( const itk::EventObject & event )
{

  if ( igstk::QuadrantViews::ReslicingEvent().CheckEvent( &event ) )
    {
    igstk::QuadrantViews::ReslicingEvent *resliceEvent =
      ( igstk::QuadrantViews::ReslicingEvent *) & event;
    this->ResliceImage( resliceEvent->Get() );

    }
}

void NeedleBiopsy::ResliceImage ( IndexType index )
{
  m_ImageRepresentation[0]->RequestSetSliceNumber( index[2] );
  m_ImageRepresentation[1]->RequestSetSliceNumber( index[0] );
  m_ImageRepresentation[2]->RequestSetSliceNumber( index[1] );

  m_ImageRepresentation[3]->RequestSetSliceNumber( index[2] );
  m_ImageRepresentation[4]->RequestSetSliceNumber( index[0] );
  m_ImageRepresentation[5]->RequestSetSliceNumber( index[1] );

  ViewerGroup->m_AxialSlider->value( index[2] );
  ViewerGroup->m_SagittalSlider->value( index[0] );
  ViewerGroup->m_CoronalSlider->value( index[1] );

  this->ViewerGroup->redraw();
  Fl::check();
}


void NeedleBiopsy::Picking( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
    {
    typedef igstk::CoordinateSystemTransformToEvent TransformEventType;
    const TransformEventType * tmevent =
      dynamic_cast< const TransformEventType *>( & event );

    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();
    igstk::Transform  transform = transformCarrier.GetTransform();
    ImageSpatialObjectType::PointType point = TransformToPoint( transform );

    if( m_ImageSpatialObject->IsInside( point ) )
      {
      int choice = TPlanPointList->value();

      if( choice == 0 )
        {
        m_EntryPoint->RequestSetTransformAndParent( 
          transform , m_WorldReference );
        m_Plan->m_EntryPoint = point;
        this->UpdatePath();
        }
      else if ( choice == 1 )
        {
        m_TargetPoint->RequestSetTransformAndParent( 
          transform, m_WorldReference );
        m_Plan->m_TargetPoint = point;
        this->UpdatePath();
        }
      else
        {
        m_FiducialPoint->RequestSetTransformAndParent( 
          transform, m_WorldReference );
        m_Plan->m_FiducialPoints[ choice-2] = point;
        }

      char buf[50];
      sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
      m_Annotation->RequestSetAnnotationText(0, buf);
      m_Annotation->RequestSetFontColor(0, 1.0, 0, 0);
      m_Annotation->RequestSetFontSize(0, 12);

      // We don't need to rewrite the file every time we modify it
      this->WriteTreatmentPlan();

      ImageSpatialObjectType::IndexType index;
      m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);
      igstkLogMacro( DEBUG, index <<"\n")
      ResliceImage( index );
      }
    else
      {
      igstkLogMacro( DEBUG,  "Picked point outside image...\n" )
      }
    }
}


void NeedleBiopsy::UpdatePath()
{
  m_Path->Clear();

  TubePointType point;
  igstk::Transform::VectorType v;

  v = ( PointToTransform( m_Plan->m_EntryPoint) ).GetTranslation();
  point.SetPosition( v[0], v[1], v[2]);
  point.SetRadius( 2 );
  m_Path->AddPoint( point );

  v = ( PointToTransform( m_Plan->m_TargetPoint) ).GetTranslation();
  point.SetPosition( v[0], v[1], v[2]);
  point.SetRadius( 2.1 );
  m_Path->AddPoint( point );


  for (int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestRemoveObject( m_PathRepresentation[i] );
    m_PathRepresentation[i]->RequestSetTubeObject( NULL );
    m_PathRepresentation[i]->RequestSetTubeObject( m_Path );
    m_PathRepresentation[i]->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentation[i]->SetOpacity( 0.5 );
    ViewerGroup->m_Views[i]->RequestAddObject( m_PathRepresentation[i] );
    }

}
