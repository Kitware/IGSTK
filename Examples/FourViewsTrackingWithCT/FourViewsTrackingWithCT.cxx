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


namespace igstk
{

/** Constructor: Initializes all internal variables. */
FourViewsTrackingWithCT::FourViewsTrackingWithCT( void ):m_StateMachine(this)
{  
  /** Setup Logger */
  m_Logger = LoggerType::New();

  m_LogCoutOutput = LogOutputType::New();
  m_LogCoutOutput->SetStream( std::cerr );
  m_Logger->AddLogOutput( m_LogCoutOutput );

  m_LogFileOutput = LogOutputType::New();
  m_LogFile.open("logFourViewsTrackingWithCT.txt");
  if( !m_LogFile.fail() )
    {
    m_LogFileOutput->SetStream( m_LogFile );
    m_Logger->AddLogOutput( m_LogFileOutput );
    }
  else
    {
    igstkLogMacro( DEBUG, "Problem opening Log file, log output to cerr only " );
    }

  /*
  this->Display3D->RequestResetCamera();
  this->Display3D->Update();
  this->Display3D->RequestEnableInteractions();
  this->Display3D->RequestSetRefreshRate( 60 ); // 60 Hz
  this->Display3D->RequestStart();
  */

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

  /** Initialize State Machine */
  //
  //m_StateMachine.SelectInitialState( m_InitialState );
  //m_StateMachine.SetReadyToRun();

}

/** Destructor */
FourViewsTrackingWithCT::~FourViewsTrackingWithCT( void )
{
 
}

void FourViewsTrackingWithCT::RequestRegisterPatientInfo( void )
{
  const char *patientName = fl_input("Patient Name:", "");
  m_PatientNameToBeSet = patientName;

  if( patientName )
    {
    // State Machine not implemented yet
    RegisterPatientInfo();
    }
  else
    {
    }
}

void FourViewsTrackingWithCT::RegisterPatientInfo()
{
  m_PatientName = m_PatientNameToBeSet;
}

void FourViewsTrackingWithCT::RequestLoadImage( void )
{
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  if ( directoryname != NULL )
    {
    //State Machine
    this->LoadImage( directoryname ); // Temp call, for testing only
    }
  else
    {
    //State Machine
    }
}

void FourViewsTrackingWithCT::LoadImage( const char * directoryname )
{
  m_ImageReader = ImageReaderType::New();
  m_ImageReader->RequestSetDirectory( directoryname );
  m_ImageReader->RequestReadImage();

  CTImageSpatialObjectRepresentation::Pointer  imageOR = CTImageSpatialObjectRepresentation::New();
  imageOR->RequestSetImageSpatialObject( m_ImageReader->GetOutput() );

  DisplayAxial->RequestAddObject( imageOR->Copy() );
  DisplayAxial->RequestResetCamera();
}


void FourViewsTrackingWithCT::RequestInitializeTracker()
{
}

void FourViewsTrackingWithCT::RequestStartTracking()
{
}

void FourViewsTrackingWithCT::RequestStopTracking()
{
}


} // end of namespace
