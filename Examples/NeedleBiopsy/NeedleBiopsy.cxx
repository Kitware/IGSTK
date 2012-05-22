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

/** -----------------------------------------------------------------
*     Constructor
*  -----------------------------------------------------------------
*/
NeedleBiopsy::NeedleBiopsy() : m_LogFile()
{

  /** Setup logger, for all igstk components. */
  m_Logger   = LoggerType::New();
  this->GetLogger()->SetTimeStampFormat( itk::LoggerBase::HUMANREADABLE );
  this->GetLogger()->SetHumanReadableFormat("%Y %b %d, %H:%M:%S");
  this->GetLogger()->SetPriorityLevel( LoggerType::INFO );

  /** Direct the application log message to the std::cout */
  itk::StdStreamLogOutput::Pointer m_LogCoutOutput
                                           = itk::StdStreamLogOutput::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  /** Direct the igstk components log message to the file. */
  itk::StdStreamLogOutput::Pointer m_LogFileOutput
                                           = itk::StdStreamLogOutput::New();
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
    //Return if fail to open the log file
    igstkLogMacro( DEBUG, "Problem opening Log file:"
                                                    << logFileName << "\n" );
    return;
    }

  /** Initialize all member variables  */
  
  m_ImageReader           = ImageReaderType::New();
  m_LandmarkRegistration  = RegistrationType::New();
  m_Annotation            = igstk::Annotation2D::New();
  m_WorldReference        = igstk::AxesObject::New();
  m_TrackerInitializerList.clear();
  m_Plan                  = new igstk::TreatmentPlan;

  /** Setting up spatial objects and their representations */
  m_NeedleTip                   = EllipsoidType::New();
  m_NeedleTipRepresentation     = EllipsoidRepresentationType::New();
  m_NeedleTip->SetRadius( 5, 5, 5 );
  m_NeedleTipRepresentation->RequestSetEllipsoidObject( m_NeedleTip );
  m_NeedleTipRepresentation->SetColor(1.0,0.0,0.0);
  m_NeedleTipRepresentation->SetOpacity(1.0);

  m_Needle                    = CylinderType::New();
  m_NeedleRepresentation      = CylinderRepresentationType::New();
  m_Needle->SetRadius( 1.5 ); 
  m_Needle->SetHeight( 100 );
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

  /** Creating observers and their call back functions */

  /** This observes picking event from view */
  m_ViewPickerObserver = LoadedObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this, &NeedleBiopsy::Picking );

  /** This observes reslicing events from FourQuadrantView class */
  m_ViewResliceObserver = LoadedObserverType::New();
  m_ViewResliceObserver->SetCallbackFunction( this,
                                                &NeedleBiopsy::ResliceImage);

  /** 
   *  This observer catches the tracker configuration sent out by
   *  TrackerConfiguration GUI
   */
  m_TrackerConfigurationObserver = LoadedObserverType::New();
  m_TrackerConfigurationObserver->SetCallbackFunction( this,
                                    &NeedleBiopsy::RequestInitializeTracker);

  /** 
   *  This observer listens to the TrackerToolTransformUpdateEvent from
   *  TrackerTool class, notice this event doesn't carry any payload, it
   *  only functions as a ticker here to trigger image representation class
   *  to do image reslicing according to the current tooltip location.
   *  Refer to:
   *  NeedleBiopsy::Tracking()
   */
  m_TrackerToolUpdateObserver    = LoadedObserverType::New();
  m_TrackerToolUpdateObserver->SetCallbackFunction( this,
                                                 &NeedleBiopsy::Tracking);

  /** Create image slice representations  */
  m_ImageRepresentation.clear();
  for (unsigned int i=0; i<6; i++)
    {
    ImageRepresentationType::Pointer rep = ImageRepresentationType::New();
    m_ImageRepresentation.push_back( rep );
    }

}

/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
NeedleBiopsy::~NeedleBiopsy()
{
  m_TrackerInitializerList.clear();
  m_TrackerToolList.clear();

}


/** -----------------------------------------------------------------
*      Loading images
* This methods asks for folder directory contains a single dicom
* series. If it loads the image successfully, it will try loading
* the previously saved treatment plan, in the same parent directory
* which should be named "folderName_TreatmentPlan.igstk".
* See also:
* Method:
*        ReadTreatmentPlan()
*        WriteTreatmentPlan()
* Class:
*        igstkTreatmentPlan
*        igstkTreatmentPlanIO
*  -----------------------------------------------------------------
*/
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


    /** 
    * IGSTK uses event for inter-components communication.
    * Event/observer model is used to replace the normal Get() method.
    * CTImageObserver here is defined by Macro in header file:
    * igstkObserverObjectMacro( CTImage,
    *                           igstk::CTImageReader::ImageModifiedEvent,
    *                           igstk::CTImageSpatialObject);
    * Refer to igstkMacros.h for more detail about this macro.
    */
    CTImageObserver::Pointer  m_CTImageObserver = CTImageObserver::New();
    m_ImageReader->AddObserver(igstk::CTImageReader::ImageModifiedEvent(),
                                                      m_CTImageObserver);

    m_ImageReader->RequestGetImage(); // This will invoke the event

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

/** -----------------------------------------------------------------
*  This method should be invoked only when the Image has been loaded
*  -----------------------------------------------------------------
*/
void NeedleBiopsy::ConnectImageRepresentation()
{
  /** Setting up annotation and added to four views  */
  m_Annotation->RequestSetAnnotationText( 2, "Georgetown ISIS Center" );
  for( int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestAddAnnotation2D( m_Annotation );
    }

  /**
  * Pass image spatial object to image slice representation and set
  * the desired slice orientation for each representations, and then 
  * add them to the views
  */
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

  /**
  *  Add all the spatial object to the views
  *  Notice the Copy() method is used for most of the representation
  *  classes but not the path. This is because the underlying geometry
  *  is fixed for those spatial object once it's created.But the path
  *  will change it's shape, so we need to keep seperate pointer for
  *  path representation in each view
  */
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

  /** 
  *  Here we connect the scene graph
  *  Here we created a virtual world reference system(as the root) and
  *  attached all the objects as its children.
  *  This is for the convenience in the following implementation. You 
  *  use any spatial object, view, tracker, or tracker tool as a 
  *  reference system in IGSTK. And you can create your own class to
  *  use the coordinate system API by using this macro:
  *     igstkCoordinateSystemClassInterfaceMacro()
  *  Refer to:
  *      igstkCoordinateSystemInterfaceMacros.h
  *  Class:
  *      igstkCoordinateSystem
  *      igstkCoordinateSystemDelegator
  */
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

  m_Needle->RequestSetTransformAndParent( transform, m_WorldReference );
  m_NeedleTip->RequestSetTransformAndParent( transform, m_WorldReference );

  /** Reset and enable the view */
  for( int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestResetCamera();
    ViewerGroup->m_Views[i]->SetRefreshRate( 30 );
    ViewerGroup->m_Views[i]->RequestStart();
    ViewerGroup->m_Displays[i]->RequestEnableInteractions();
    }

  /** 
   *  Request information about the slice bounds. The answers will be
   *  received in the form of events. This will be used to initialize
   *  the reslicing sliders and initial slice position
   */
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

/** -----------------------------------------------------------------
*  Here we read the treatment plan. In this simple example
*  we assume there is one entry point, one target point, and at least
*  3 fiducial points. Here is a sample format
---------------------------------------------------------------------
# Entry point
0.820425  -143.635  -186
# Target point
54.268    -108.513  -191
# Fiducial points
98.4887   -152.976  -181
-1.89214  -148.996  -191
-59.2006  -190.563  -191
--------------------------------------------------------------------
* Refer to class:
*        igstkTreatmentPlan
*        igstkTreatmentPlanIO
*  -----------------------------------------------------------------
*/
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

  /** Populate the choice box */
  TPlanPointList->clear();
  TPlanPointList->add( "Entry" );
  TPlanPointList->add( "Target" );

  m_TrackerLandmarksContainer.clear();
  char buf[10];
  for( unsigned int i = 0; i < m_Plan->m_FiducialPoints.size(); i++ )
    {
    sprintf( buf, "Fiducial%i", i+1 );
    TPlanPointList->add( buf );
    RegistrationType::LandmarkTrackerPointType p;
    m_TrackerLandmarksContainer.push_back(p);
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

/** -----------------------------------------------------------------
*  Overwrite the current treatment plan to the file
*  Refer to class:
*        igstkTreatmentPlan
*        igstkTreatmentPlanIO
*---------------------------------------------------------------------
*/
void NeedleBiopsy::WriteTreatmentPlan()
{
  igstk::TreatmentPlanIO * writer = new igstk::TreatmentPlanIO;
  writer->SetFileName( m_PlanFilename );
  writer->SetTreatmentPlan( m_Plan );
  writer->RequestWrite();
}

/** -----------------------------------------------------------------
*  When changing the selection in the choice box, this function 
*  reslices images to the current point location, and also show the
*  position in the annotation in blue. Whenever a point is selected
*  in the choice box, the picking event will update this point's location
*  See also:
*         NeedleBiopsy::Picking
*---------------------------------------------------------------------
*/
void NeedleBiopsy::ChangeSelectedTPlanPoint()
{
  if ( TPlanPointList->size() == 0)
    {
    return;
    }
  
  /** 
   * Check which point is selected, the first two are entry and target 
   * point
   */
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

  /** Display point position as annotation */
  char buf[50];
  sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
  m_Annotation->RequestSetAnnotationText(0, buf);
  m_Annotation->RequestSetFontColor(0, 0, 0, 1.0);
  m_Annotation->RequestSetFontSize(0, 12);

  /** Reslice image to the selected point position */
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


/** -----------------------------------------------------------------
*  Choose to connect to a tracker
*  A tracker specific GUI is initialized, and the observer is hooked
*  to it to catch the configuration. When the "confirm" button is
*  pressed, an event loaded with tracker configuration will be sent out.
*  And it will trigger the RequestInitializeTracker(), which does the 
*  actual tracker initialization
*---------------------------------------------------------------------
*/
void NeedleBiopsy::RequestConnectToTracker()
{
  RequestStopTracking();

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

/** -----------------------------------------------------------------
*  Call back function for ConfigurationEvent observer
*---------------------------------------------------------------------
*/
void NeedleBiopsy::RequestInitializeTracker(const itk::EventObject & event)
{
  typedef igstk::TrackerConfigurationGUIBase  GUIType;
  if ( GUIType::ConfigurationEvent().CheckEvent( &event ) )
    {
    GUIType::ConfigurationEvent *confEvent =
                                   ( GUIType::ConfigurationEvent *) & event;

    igstk::TrackerConfiguration  tc = confEvent->Get();

    m_TrackerInitializer = new igstk::TrackerInitializer;
    m_TrackerInitializer->SetTrackerConfiguration( & tc );

    if ( m_TrackerInitializer->RequestInitializeTracker() )
      { 
      igstk::Tracker::Pointer    tracker = m_TrackerInitializer->GetTracker();
      igstk::TrackerTool::Pointer tool  = 
                          m_TrackerInitializer->GetNonReferenceToolList()[0];
      igstk::TrackerTool::Pointer refTool = 
                                     m_TrackerInitializer->GetReferenceTool();
      
      /** Connect the scene graph with an identity transform first */
      igstk::Transform transform;
      transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
      if ( m_TrackerInitializer->HasReferenceTool() )
        {
        refTool->RequestSetTransformAndParent(transform, m_WorldReference);
        }
      else
        {
        tracker->RequestSetTransformAndParent(transform, m_WorldReference);
        }

      /** Now who the registration window and initialize it */
      RegistrationWindow->show();
      FiducialNumber->clear();
      m_TrackerLandmarksContainer.clear();
      char buf[8];
      for ( unsigned int i=0; i<m_Plan->m_FiducialPoints.size(); i++)
        {
          sprintf( buf, "%d", i+1 );
          FiducialNumber->add(buf);
          RegistrationType::LandmarkTrackerPointType p;
          m_TrackerLandmarksContainer.push_back(p);
        }

      /** 
       *  Set the tracker and image fiducial landmark to the first point
       *  and reslice the image to show this fiducial point
       */
      FiducialNumber->value(0);
      TPlanPointList->value(2);
      ChangeSelectedTPlanPoint();
      }
    }
}

/** -----------------------------------------------------------------
*  Call this method every time we successfully connect or disconnect to
*  a tracker. This updates the available tracker and tracker tool
*  in the GUI list as well as internal pointer list.
*---------------------------------------------------------------------
*/
void NeedleBiopsy::UpdateTrackerAndTrackerToolList()
{
  TrackerList->clear();
  TrackerToolList->clear();
  m_TrackerToolList.clear();
  int n = 0;
  std::string s;
  for ( unsigned int i=0; i<m_TrackerInitializerList.size(); i++)
    {
      char buf[10];
      sprintf(buf, "%d", i+1);
      s = "Tracker ";
      s = s + buf + " [" + 
                  m_TrackerInitializerList[i]->GetTrackerTypeAsString() + "]";
      TrackerList->add( s.c_str() );
      std::vector< igstk::TrackerTool::Pointer > toolList = 
                      m_TrackerInitializerList[i]->GetNonReferenceToolList();
      for ( unsigned int j=0; j< toolList.size(); j++)
        {
          char buff[10];
          sprintf(buff,"%d", ++n);
          s = "Tool ";
          s = s + buff + " [" +
                  m_TrackerInitializerList[i]->GetTrackerTypeAsString() + "]";
          TrackerToolList->add( s.c_str() );
          m_TrackerToolList.push_back( toolList[j] );
        }
    }

}

/** -----------------------------------------------------------------
*  Disconnect a tracker. After disconnecting it, set the active tool
*  to the first available tool
*---------------------------------------------------------------------
*/
void NeedleBiopsy::RequestDisconnetTracker()
{
  RequestStopTracking();
  unsigned int n = TrackerList->value();
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

/** -----------------------------------------------------------------
*  Hot switch the active tool, the one that drives the reslicing and
*  needle display. The reslicing is done by using an observer listening
*  to the TrackerToolTransformUpdateEvent, and the call back function
*  will get the tool tip position in the image space and reslice image 
*  to that point. Refer to:
*       NeedleBiopsy::Tracking
*  First, we stop the tracking and disconnect the observer from other tools
*  Second, we connect observer to the selected tool
*  Finally, we attach spatial objects representing the needle to this tool
*           and restart the tracking again
*---------------------------------------------------------------------
*/
void NeedleBiopsy::ChangeActiveTrackerTool()
{
  RequestStopTracking();
  itksys::SystemTools::Delay( 500 );

  if (m_TrackerToolList.size() != 0)
    {
      for (unsigned int i=0; i<m_TrackerToolList.size(); i++)
      {
        m_TrackerToolList[i]->RemoveAllObservers();
      }
      m_ActiveTool = m_TrackerToolList[ TrackerToolList->value()];

      m_ActiveTool->AddObserver(
      igstk::TrackerToolTransformUpdateEvent(), m_TrackerToolUpdateObserver);
      
      igstk::Transform transform;
      transform.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
      m_Needle->RequestDetachFromParent();
      m_NeedleTip->RequestDetachFromParent();
      m_Needle->RequestSetTransformAndParent( transform, m_ActiveTool);
      m_NeedleTip->RequestSetTransformAndParent( transform, m_ActiveTool);
    }
  RequestStartTracking();
}

/** -----------------------------------------------------------------
*  Call back functions for registration window. Every time user clicks
*  on the set fiducial point button, it will read from the recently
*  initialized tracker's first non-reference tool. The reading will 
*  serve as a tracker landmark point for registration. 
*  This will automatic jump to the next fiducial point for user to set
*---------------------------------------------------------------------
*/
void NeedleBiopsy::SetTrackerFiducialPoint()
{
  igstk::TrackerTool::Pointer tool = 
                           m_TrackerInitializer->GetNonReferenceToolList()[0];

  typedef igstk::TransformObserver ObserverType;
  ObserverType::Pointer transformObserver = ObserverType::New();
  transformObserver->ObserveTransformEventsFrom( tool );
  transformObserver->Clear();
  tool->RequestComputeTransformTo( m_WorldReference );
  if ( transformObserver->GotTransform() )
    {
    int n = FiducialNumber->value();
    int m = FiducialNumber->size();
    m_TrackerLandmarksContainer[n] = 
                       TransformToPoint( transformObserver->GetTransform() );
    std::cout << m_TrackerLandmarksContainer[n] <<"\n";
    if ( n < m )
    {
      FiducialNumber->value(n+1);
      TPlanPointList->value(n+3);
      ChangeSelectedTPlanPoint();
    }
    } 
}

/** -----------------------------------------------------------------
*  Pair points 3D landmark registration
*  Image landmarks are from treatment plan. Refer to:
*         ReadTreatmentPlan
*         Picking
* tracker landmarks are set in the registration window. Refer to:
*         SetTrackerFiducialPoint
*---------------------------------------------------------------------
*/
void NeedleBiopsy::RequestRegistration()
{
  m_LandmarkRegistration->RequestResetRegistration();
  for( unsigned int i=0; i< m_TrackerLandmarksContainer.size(); i++)
    {
    m_LandmarkRegistration->RequestAddImageLandmarkPoint( 
                                               m_Plan->m_FiducialPoints[i] );
    m_LandmarkRegistration->RequestAddTrackerLandmarkPoint( 
                                            m_TrackerLandmarksContainer[i] );
    }

  m_LandmarkRegistration->RequestComputeTransform();

  igstk::TransformObserver::Pointer lrtcb = igstk::TransformObserver::New();
  lrtcb->ObserveTransformEventsFrom( m_LandmarkRegistration );
  lrtcb->Clear();

  m_LandmarkRegistration->RequestGetTransformFromTrackerToImage();

  if( lrtcb->GotTransform() )
    {
      RegistrationErrorObserver::Pointer lRmscb =  
                                            RegistrationErrorObserver::New();
      m_LandmarkRegistration->AddObserver( igstk::DoubleTypeEvent(), lRmscb );
      m_LandmarkRegistration->RequestGetRMSError();
      if( lRmscb->GotRegistrationError() )
      {
        std::cout << "\nRegistration Error" << 
                                  lRmscb->GetRegistrationError() << "\n";
      }

    igstk::Transform transform = lrtcb->GetTransform();
    std::cout << transform << "\n";
    if ( m_TrackerInitializer->HasReferenceTool() )
    {
      m_TrackerInitializer->GetReferenceTool()
        ->RequestSetTransformAndParent(transform, m_WorldReference);
    }
    else
    {
      m_TrackerInitializer->GetTracker()
        ->RequestSetTransformAndParent(transform, m_WorldReference);
    }
    
    /** 
     * Only when a tracker is registered with the image, it will show up
     * on the gui as available. Otherwise it will be disconnected
     */
    m_TrackerInitializerList.push_back( m_TrackerInitializer );
    UpdateTrackerAndTrackerToolList();
    TrackerList->value(m_TrackerInitializerList.size()-1);
    TrackerToolList->value(m_TrackerInitializerList.size()-1);
    ChangeActiveTrackerTool();
    RequestStartTracking();
    }
  else
  {
    m_TrackerInitializer->StopAndCloseTracker();
  }

}

/** -----------------------------------------------------------------
*  Start tracking of all the connected trackers  
*---------------------------------------------------------------------
*/
void NeedleBiopsy::RequestStartTracking()
{
  for (unsigned int i=0; i<m_TrackerInitializerList.size(); i++)
    {
    m_TrackerInitializerList[i]->GetTracker()->RequestStartTracking();
    }

  TrackingBtn->label("Stop");
  TrackingBtn->value(1);
  ControlGroup->redraw();
}

/** -----------------------------------------------------------------
*  Stop tracking of all the connected trackers  
*---------------------------------------------------------------------
*/
void NeedleBiopsy::RequestStopTracking()
{
  for (unsigned int i=0; i<m_TrackerInitializerList.size(); i++)
  {
    m_TrackerInitializerList[i]->GetTracker()->RequestStopTracking();
  }

  TrackingBtn->label("Tracking");
  TrackingBtn->value(0);
  ControlGroup->redraw();
}

/** -----------------------------------------------------------------
*  Callback function for observer listening to the slider bar
*  ReslicingEvent
*---------------------------------------------------------------------
*/
void NeedleBiopsy::ResliceImage( const itk::EventObject & event )
{

  if ( igstk::QuadrantViews::ReslicingEvent().CheckEvent( &event ) )
    {
    igstk::QuadrantViews::ReslicingEvent *resliceEvent =
      ( igstk::QuadrantViews::ReslicingEvent *) & event;
    this->ResliceImage( resliceEvent->Get() );

    }
}

/** -----------------------------------------------------------------
*  Methods for reslicing images given an index number  
*---------------------------------------------------------------------
*/
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


/** -----------------------------------------------------------------
*  Callback function for picking event.
*  Upon receiving a valid picking event, this method will change the 
*  value of currently selected treatment plan point, reslice the image
*  to that location, update the annotation with the new point position,
*  and write the modified treatment plan to the file.
*---------------------------------------------------------------------
*/
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

      /** Update annotation */
      char buf[50];
      sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
      m_Annotation->RequestSetAnnotationText(0, buf);
      m_Annotation->RequestSetFontColor(0, 1.0, 0, 0);
      m_Annotation->RequestSetFontSize(0, 12);

      /** Wirte the updated plan to file */
      this->WriteTreatmentPlan();

      /** Reslice image */
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

/** -----------------------------------------------------------------
*  Every time we modify the entry point or target point, we need to
*  rebuild the geometry of the path, and add them to the View again
*---------------------------------------------------------------------
*/
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


  for (unsigned int i=0; i<4; i++)
    {
    ViewerGroup->m_Views[i]->RequestRemoveObject( m_PathRepresentation[i] );
    m_PathRepresentation[i]->RequestSetTubeObject( NULL );
    m_PathRepresentation[i]->RequestSetTubeObject( m_Path );
    m_PathRepresentation[i]->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentation[i]->SetOpacity( 0.5 );
    ViewerGroup->m_Views[i]->RequestAddObject( m_PathRepresentation[i] );
    }

}

/** -----------------------------------------------------------------
*  Callback function for TrackerToolTransformUpdateEvent
*  This function computes the transform of the tooltip location in
*  the image space (WorldReference is essential image coordinate system
*  since image is attached to WorldReference using an identity transform),
*  and reslice image to that tip location
*---------------------------------------------------------------------
*/

void NeedleBiopsy::Tracking(const itk::EventObject & event )
{
  if ( igstk::TrackerToolTransformUpdateEvent().CheckEvent( &event ) )
  {
    typedef igstk::TransformObserver ObserverType;
    ObserverType::Pointer transformObserver = ObserverType::New();
    transformObserver->ObserveTransformEventsFrom( m_ActiveTool );
    transformObserver->Clear();
    m_ActiveTool->RequestComputeTransformTo( m_WorldReference );
    if ( transformObserver->GotTransform() )
      {
      ImageSpatialObjectType::PointType point = 
                      TransformToPoint( transformObserver->GetTransform() );

      if( m_ImageSpatialObject->IsInside( point ) )
      { 
        ImageSpatialObjectType::IndexType index;
        m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);
        ResliceImage( index );
        }
      }
  }
}
