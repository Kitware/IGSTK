/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FourViewsTrackingWithCTImplementation.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "FourViewsTrackingWithCTImplementation.h"

#include "FL/Fl_File_Chooser.H"

#include "igstkImageSpatialObject.h"
#include "igstkImageSpatialObjectRepresentation.h"

#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkCTImageSpatialObjectRepresentation.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
FourViewsTrackingWithCTImplementation::FourViewsTrackingWithCTImplementation( void )
{
  m_Tracker = TrackerType::New();

  m_Logger = LoggerType::New();
  m_LogOutput = LogOutputType::New();
  m_LogFile.open("logFourViewsTrackingWithCT.txt");
  if( !m_LogFile.fail() )
    {
    m_LogOutput->SetStream( m_LogFile );
    }
  else
    {
    std::cerr << "Problem opening Log file, using cerr instead " << std::endl;
    m_LogOutput->SetStream( std::cerr );
    }

  m_Logger->AddLogOutput( m_LogOutput );

  // add stdout for debug purposes
  LogOutputType::Pointer coutLogOutput = LogOutputType::New();
  coutLogOutput->SetStream( std::cout );
  m_Logger->AddLogOutput( coutLogOutput );
  m_Logger->SetPriorityLevel( LoggerType::DEBUG );
  m_Tracker->SetLogger( m_Logger );

  m_Communication = CommunicationType::New();
  m_Communication->SetLogger( m_Logger );
  m_Communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  m_Communication->SetParity( igstk::SerialCommunication::NoParity );
  m_Communication->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  m_Communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  m_Communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  m_Communication->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );
  m_Tracker->SetCommunication(m_Communication);

  m_Communication->OpenCommunication();

  //m_Tracker->Open();
  //m_Tracker->Initialize();

  // Set up the four quadrant views
  this->Display3D->RequestResetCamera();
  this->Display3D->Update();
  this->Display3D->RequestEnableInteractions();
  this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
  this->Display3D->RequestStart();

  this->DisplayAxial->RequestResetCamera();
  this->DisplayAxial->Update();
  this->DisplayAxial->RequestEnableInteractions();
  this->DisplayAxial->RequestSetRefreshRate( 60 ); // 60 Hz
  this->DisplayAxial->RequestStart();

  this->DisplayCoronal->RequestResetCamera();
  this->DisplayCoronal->Update();
  this->DisplayCoronal->RequestEnableInteractions();
  this->DisplayCoronal->RequestSetRefreshRate( 60 ); // 60 Hz
  this->DisplayCoronal->RequestStart();

  this->DisplaySagittal->RequestResetCamera();
  this->DisplaySagittal->Update();
  this->DisplaySagittal->RequestEnableInteractions();
  this->DisplaySagittal->RequestSetRefreshRate( 60 ); // 60 Hz
  this->DisplaySagittal->RequestStart();

  m_Tracking = false;
}

/** Destructor */
FourViewsTrackingWithCTImplementation::~FourViewsTrackingWithCTImplementation( void )
{
  m_Tracker->Reset();
  m_Tracker->StopTracking();
  m_Tracker->Close();
}

void FourViewsTrackingWithCTImplementation::EnableTracking( void )
{
  m_Tracking = true;
  m_Tracker->StartTracking();
}

void FourViewsTrackingWithCTImplementation::DisableTracking( void )
{
  m_Tracker->Reset();
  m_Tracker->StopTracking();
  m_Tracking = false;
}

void FourViewsTrackingWithCTImplementation::AddCylinder( igstk::CylinderObjectRepresentation * cylinderRepresentation )
{
  this->Display3D->RequestAddObject(       cylinderRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject(    cylinderRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject(  cylinderRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( cylinderRepresentation->Copy() );
}

void FourViewsTrackingWithCTImplementation::AddEllipsoid( igstk::EllipsoidObjectRepresentation * ellipsoidRepresentation )
{
  this->Display3D->RequestAddObject(       ellipsoidRepresentation->Copy() );
  this->DisplayAxial->RequestAddObject(    ellipsoidRepresentation->Copy() );
  this->DisplayCoronal->RequestAddObject(  ellipsoidRepresentation->Copy() );
  this->DisplaySagittal->RequestAddObject( ellipsoidRepresentation->Copy() );
}

void FourViewsTrackingWithCTImplementation::AttachObjectToTrack( igstk::SpatialObject * objectToTrack )
{
  const unsigned int toolPort = 0;
  const unsigned int toolNumber = 0;
  m_Tracker->AttachObjectToTrackerTool( toolPort, toolNumber, objectToTrack );
}

void FourViewsTrackingWithCTImplementation::LoadImage( void )
{
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");

  if( directoryname != NULL )
    {

    CTImageSpatialObject::Pointer imageObject = CTImageSpatialObject::New();

    CTImageReader::Pointer  reader = CTImageReader::New();
    reader->RequestSetDirectory(directoryname);
    reader->RequestReadImage();

    Friends::ImageReaderToImageSpatialObject  *connector;
    connector->ConnectImage(reader,imageObject);
    }
}

} // end of namespace
