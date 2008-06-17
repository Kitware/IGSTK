#include "igstkMicronTrackerConfiguration.h"

namespace igstk
{                  //highest referesh rate for the micron tracker family (Sx60)
const double MicronTrackerConfiguration::MAXIMAL_REFERESH_RATE = 48;


MicronTrackerConfiguration::MicronTrackerConfiguration() :
  m_CameraCalibrationFileDirectory(""),
  m_InitializationFile(""),
  m_TemplatesDirectory("")
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


MicronTrackerConfiguration::~MicronTrackerConfiguration()
{
}

double
MicronTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void 
MicronTrackerConfiguration::InternalAddTool( const
  TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;

  const MicronToolConfiguration *wirelessTool = 
    dynamic_cast<const MicronToolConfiguration *>( tool );

  if( wirelessTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }
  if( wirelessTool->GetMarkerName().empty() )
  {
    fe.Set( "Marker name not specified for micron tool." );
    this->InvokeEvent( fe );
    return;
  }
  if( !isReference )
  {
    this->m_TrackerToolList.push_back( new MicronToolConfiguration( 
                                                              *wirelessTool ) );
  }
  else
  {
    if (this->m_ReferenceTool)
      delete this->m_ReferenceTool;
    this->m_ReferenceTool = new MicronToolConfiguration( *wirelessTool );
  }
  this->InvokeEvent( AddToolSuccessEvent() );
}


MicronToolConfiguration::MicronToolConfiguration() : m_MarkerName( "" ), m_CalibrationFileName( "" )
{
}


MicronToolConfiguration::MicronToolConfiguration( const 
  MicronToolConfiguration &other ) : TrackerToolConfiguration( other )
{
  this->m_MarkerName = other.m_MarkerName;
  this->m_CalibrationFileName = other.m_CalibrationFileName;
}


MicronToolConfiguration::~MicronToolConfiguration() 
{
}


std::string 
MicronToolConfiguration::GetToolTypeAsString()
{
  return "MicronToolConfiguration";
}

} // end of name space
