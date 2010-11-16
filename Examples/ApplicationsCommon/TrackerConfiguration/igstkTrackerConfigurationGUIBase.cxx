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
#include <FL/fl_ask.H>
#include "NeedleBiopsyUnimodality.h"

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
  NeedleBiopsyUnimodality *parent )
{
  this->m_Parent = parent;
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

  const igstk::TrackerConfiguration::FrequencySetFailureEvent *evt1 = 
    dynamic_cast<const igstk::TrackerConfiguration::FrequencySetFailureEvent*>( &event );

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
    fl_alert("%s\n", msg.c_str() );
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
