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
NeedleBiopsy::NeedleBiopsy()
{  

  /** Setup logger, for all other igstk components. */
  m_Logger   = LoggerType::New();
  this->GetLogger()->SetTimeStampFormat( itk::LoggerBase::HUMANREADABLE );
  this->GetLogger()->SetHumanReadableFormat("%Y %b %d, %H:%M:%S");
  this->GetLogger()->SetPriorityLevel( LoggerType::INFO );

  /** Direct the application log message to the std::cout 
    * and FLTK text display */
  itk::StdStreamLogOutput::Pointer m_LogCoutOutput = itk::StdStreamLogOutput::New(); 
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  /** Direct the igstk components log message to the file. */
  itk::StdStreamLogOutput::Pointer m_LogFileOutput = itk::StdStreamLogOutput::New(); 
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
    igstkLogMacro( DEBUG, "Problem opening Log file: " << logFileName << "\n" );
    return;
    }

  /** Initialize all member variables and set logger */
  m_ImageReader = ImageReaderType::New();
  m_LandmarkRegistration = RegistrationType::New(); 
  m_Annotation2D = igstk::Annotation2D::New();
  

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

  m_Path                       = PathType::New();
  TubePointType p;
  p.SetPosition( 0, 0, 0);
  p.SetRadius( 2 );
  m_Path->AddPoint( p );
  m_Path->AddPoint( p );

  m_PathRepresentation.clear();
  for (int i=0; i<4; i++)
  {
    PathRepresentationType::Pointer rep  = PathRepresentationType::New();
    rep->RequestSetTubeObject( m_Path );
    rep->SetColor( 0.0, 1.0, 0.0);
    rep->SetOpacity( 0.4 );
    m_PathRepresentation.push_back( rep );
  }


  m_ViewPickerObserver = LoadedObserverType::New();
  m_ViewPickerObserver->SetCallbackFunction( this, 
                                               &NeedleBiopsy::Picking );

  m_ViewResliceObserver = LoadedObserverType::New();
  m_ViewResliceObserver->SetCallbackFunction( this, &NeedleBiopsy::ResliceImage);

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
      this->ConnectImageRepresentationProcessing();
      return EXIT_SUCCESS;
    }
    else
    {
      igstkLogMacro(          DEBUG, "Reading image failure...\n" )
      return EXIT_FAILURE;      
    }
  }
  else
  {
    igstkLogMacro(          DEBUG, "No directory is selected\n" )
    return EXIT_FAILURE;
  }

}


void NeedleBiopsy::RequestInitializeTracker()
{
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


void NeedleBiopsy::ResliceImage( const itk::EventObject & event )
{

}

void NeedleBiopsy::ResliceImage()
{
  for (int i=0; i<3; i++)
  {
    m_ImageRepresentation[i]->RequestSetSliceNumber( 
      static_cast< unsigned int >( ViewerGroup->Sliders[i]->value() ) );
    m_ImageRepresentation[i+3]->RequestSetSliceNumber( 
      static_cast< unsigned int >( ViewerGroup->Sliders[i]->value() ) );
  }
 
  this->ViewerGroup->redraw();
  Fl::check();
}

void NeedleBiopsy::ResliceImage ( IndexType index )
{
  m_ImageRepresentation[0]->RequestSetSliceNumber( index[2] );
  m_ImageRepresentation[1]->RequestSetSliceNumber( index[0] );
  m_ImageRepresentation[2]->RequestSetSliceNumber( index[1] );

  m_ImageRepresentation[3]->RequestSetSliceNumber( index[2] );
  m_ImageRepresentation[4]->RequestSetSliceNumber( index[0] );
  m_ImageRepresentation[5]->RequestSetSliceNumber( index[1] );
  
  ViewerGroup->AxialSlider->value( index[2] );
  ViewerGroup->SagittalSlider->value( index[0] );
  ViewerGroup->CoronalSlider->value( index[1] );

  this->ViewerGroup->redraw();
  Fl::check();
}

/** This method should be invoked by the State Machine 
 *  only when the Image has been loaded and the Patient
 *  name has been verified */
void NeedleBiopsy::ConnectImageRepresentationProcessing()
{
  igstk::Transform transform;
  transform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  m_Annotation2D->RequestSetAnnotationText( 0, "Georgetown ISIS Center" );
  ViewerGroup->Views[3]->RequestAddAnnotation2D( m_Annotation2D );

  for ( int i=0; i<6; i++)
  {
    m_ImageRepresentation[i]->RequestSetImageSpatialObject( m_ImageSpatialObject );
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
    ViewerGroup->Views[i]->RequestRemoveObject( m_ImageRepresentation[i] );
    ViewerGroup->Views[3]->RequestRemoveObject( m_ImageRepresentation[i+3] );
    ViewerGroup->Views[i]->RequestAddObject( m_ImageRepresentation[i] );
    ViewerGroup->Views[3]->RequestAddObject( m_ImageRepresentation[i+3] );
  }

  for ( int i=0; i<4; i++)
  {
    ViewerGroup->Views[i]->RequestAddObject( m_NeedleTipRepresentation->Copy() );
    ViewerGroup->Views[i]->RequestAddObject( m_NeedleRepresentation->Copy() );
    ViewerGroup->Views[i]->RequestAddObject( m_TargetRepresentation->Copy() );
    ViewerGroup->Views[i]->RequestAddObject( m_EntryRepresentation->Copy() );
    ViewerGroup->Views[i]->RequestAddObject( m_PathRepresentation[i] );
    ViewerGroup->Views[i]->RequestSetTransformAndParent( transform, m_ImageSpatialObject );
  }

  m_EntryPoint->RequestSetTransformAndParent( transform, m_ImageSpatialObject );
  m_TargetPoint->RequestSetTransformAndParent( transform, m_ImageSpatialObject );
  m_Path->RequestSetTransformAndParent( transform, m_ImageSpatialObject );

  for ( int i=0; i<4; i++)
  {
    ViewerGroup->Views[i]->RequestResetCamera();
    ViewerGroup->Views[i]->SetRefreshRate( 30 );
    ViewerGroup->Views[i]->RequestStart();
    ViewerGroup->Displays[i]->RequestEnableInteractions();
  }
  
  // Request information about the slices. The answers will be 
  // received in the form of events.
  SliceBoundsObserver::Pointer boundsObs = SliceBoundsObserver::New(); 
  for ( int i=0; i<3; i++)
  {
    m_ImageRepresentation[i]->AddObserver(igstk::IntegerBoundsEvent(), boundsObs);
    m_ImageRepresentation[i]->RequestGetSliceNumberBounds();
    if (boundsObs->GotSliceBounds())
    {
      const unsigned int min = boundsObs->GetSliceBounds().minimum;
      const unsigned int max = boundsObs->GetSliceBounds().maximum; 
      const unsigned int slice = static_cast< unsigned int > ( (min + max) / 2.0 );
      m_ImageRepresentation[i]->RequestSetSliceNumber( slice );
      m_ImageRepresentation[i+3]->RequestSetSliceNumber( slice );
      ViewerGroup->Sliders[i]->minimum( min );
      ViewerGroup->Sliders[i]->maximum( max );
      ViewerGroup->Sliders[i]->value( slice );  
      ViewerGroup->Sliders[i]->activate();
      boundsObs->Reset();
    }
  }
  
  for ( int i=0; i<3; i++)
  {
    ViewerGroup->Views[i]->AddObserver( igstk::TransformModifiedEvent(), 
                                                        m_ViewPickerObserver );
  }

  ViewerGroup->AddObserver( igstk::QuadrantViews::ReslicingEvent(), 
                                                       m_ViewResliceObserver );
}







void NeedleBiopsy::Picking( const itk::EventObject & event)
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
      //m_ImageLandmarkTransformToBeSet = tmevent->Get();

    //std::cout<< m_ImageLandmarkTransformToBeSet << std::endl;
      
#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
      m_PickedPoint->RequestSetTransform( m_ImageLandmarkTransformToBeSet );
#endif
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


void NeedleBiopsy::UpdatePath()
{
#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
  m_TargetPoint->RequestSetTransform( m_TargetTransform );
  m_EntryPoint->RequestSetTransform( m_EntryTransform );
#endif

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
#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
  m_Path->RequestSetTransform( transform );
#endif

  for (int i=0; i<4; i++)
  {
    ViewerGroup->Views[i]->RequestRemoveObject( m_PathRepresentation[i] );
    m_PathRepresentation[i]->RequestSetTubeObject( NULL );
    m_PathRepresentation[i]->RequestSetTubeObject( m_Path );
    m_PathRepresentation[i]->SetColor( 0.0, 1.0, 0.0 );
    m_PathRepresentation[i]->SetOpacity( 0.5 );
    ViewerGroup->Views[i]->RequestAddObject( m_PathRepresentation[i] );
  }

}
