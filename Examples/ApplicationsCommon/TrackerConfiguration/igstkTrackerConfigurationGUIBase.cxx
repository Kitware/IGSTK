#include "igstkTrackerConfigurationGUIBase.h"
#include "igstkTrackerConfiguration.h"
#include <FL/fl_ask.h>
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
