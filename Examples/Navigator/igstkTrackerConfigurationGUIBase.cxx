/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationGUIBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerConfigurationGUIBase.h"
#include "igstkTrackerConfiguration.h"
#include <FL/fl_ask.h>
#include "Navigator.h"

#include "igstkMicronTrackerConfiguration.h"

#define TRACKER_DEFAULT_REFRESH_RATE 15

namespace igstk
{


TrackerConfigurationGUIBase::TrackerConfigurationGUIBase() :
  m_ConfigurationErrorOccured( false ),
  m_Parent( NULL )
{
}


TrackerConfigurationGUIBase::~TrackerConfigurationGUIBase()
{
}


void 
TrackerConfigurationGUIBase::SetParent( 
  Navigator *parent )
{
  this->m_Parent = parent;
}

int
TrackerConfigurationGUIBase::InitializeAsMicronDefault()
{
  igstk::MicronTrackerConfiguration* trackerConfiguration = 
    new igstk::MicronTrackerConfiguration();
  trackerConfiguration->AddObserver( igstk::TrackerConfiguration::AddToolFailureEvent(), this );
  trackerConfiguration->AddObserver( igstk::TrackerConfiguration::FrequencySetErrorEvent(), this );

  trackerConfiguration->SetCameraCalibrationFileDirectory( "C:/Program Files/Claron Technology/MicronTracker/CalibrationFiles" );
  trackerConfiguration->SetInitializationFile( "C:/Program Files/Claron Technology/MicronTracker/MTDemoCPP.ini" );
  trackerConfiguration->SetTemplatesDirectory( "C:/Program Files/Claron Technology/MicronTracker/Markers" ); 

  //set the tool parameters
  igstk::MicronToolConfiguration tool;  
  tool.SetMarkerName( "C:/Program Files/Claron Technology/MicronTracker/Markers/sPtr" );
  //tool.SetCalibrationFileName( "C:/data/sPtr_Calibration.igstk" );
  trackerConfiguration->RequestAddTool( &tool );
  if( this->m_ConfigurationErrorOccured )
  {
    trackerConfiguration->Delete();
    trackerConfiguration = NULL;
    return 0;
  }

//  m_AddedToolsList->add(tool.GetMarkerName().c_str());
//  m_AddedToolsList->value(0);

  //set the tool parameters
  igstk::MicronToolConfiguration reftool;
  
  reftool.SetMarkerName( "C:/Program Files/Claron Technology/MicronTracker/Markers/abdomen" );

  trackerConfiguration->RequestAddReference( &reftool );
  if( this->m_ConfigurationErrorOccured )
  {
    trackerConfiguration->Delete();
    return 0;
  }

  trackerConfiguration->RequestSetFrequency( TRACKER_DEFAULT_REFRESH_RATE );
  if( this->m_ConfigurationErrorOccured )
  {
    trackerConfiguration->Delete();
    return 0;
  }

 this->m_Parent->SetTrackerConfiguration( trackerConfiguration );

 return 1;

}

void 
TrackerConfigurationGUIBase::UpdateParentConfiguration()
{
  igstk::TrackerConfiguration *trackerConfiguration = GetConfiguration();
  if( trackerConfiguration )
  {
    this->m_Parent->SetTrackerConfiguration( trackerConfiguration );
  }
}


void 
TrackerConfigurationGUIBase::Execute( itk::Object *caller, 
                                      const itk::EventObject & event )
{
  std::string msg;

  const igstk::TrackerConfiguration::FrequencySetErrorEvent *evt1 = 
    dynamic_cast<const igstk::TrackerConfiguration::FrequencySetErrorEvent*>( &event );

  if( evt1 )
  {
    msg = evt1->Get();
  }
  else 
  {
    const igstk::TrackerConfiguration::AddToolFailureEvent *evt2 = 
      dynamic_cast<const igstk::TrackerConfiguration::AddToolFailureEvent*>( &event );
    if( evt2 )
    {
      msg = evt2->Get();
    }
  }
  
  if( !msg.empty() )
  {
    m_ConfigurationErrorOccured = true;
    fl_alert( msg.c_str() );
    fl_beep( FL_BEEP_ERROR );
  }
}


void 
TrackerConfigurationGUIBase::Execute( const itk::Object *caller, 
                                      const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}


} // end of name space
