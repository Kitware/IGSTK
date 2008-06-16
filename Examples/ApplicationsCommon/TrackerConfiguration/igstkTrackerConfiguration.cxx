#include "igstkTrackerConfiguration.h"

namespace igstk
{
  
  
TrackerConfiguration::TrackerConfiguration() : m_ReferenceTool( NULL )
{
}


TrackerConfiguration::~TrackerConfiguration()
{
  std::vector< TrackerToolConfiguration * >::iterator it;
  std::vector< TrackerToolConfiguration * >::iterator endIt = 
    this->m_TrackerToolList.end();

  for(it=this->m_TrackerToolList.begin(); it!=endIt; it++)
  {    
    delete (*it);
  }
  this->m_TrackerToolList.clear();
  delete this->m_ReferenceTool;  
}


void
TrackerConfiguration::RequestSetFrequency( double frequency )
{
  if( frequency<=0 || frequency>GetMaximalRefreshRate() )
  {
    std::ostringstream msg;
    msg.str("");
    msg<<"Invalid tracker frequency specified.";
    msg<< "Valid values are in [0,"<<GetMaximalRefreshRate()<<"].";

    FrequencySetFailureEvent evt;
    evt.Set( msg.str() );
    this->InvokeEvent( evt );
  }
  else
  {
    this->m_Frequency = frequency;
     this->InvokeEvent( FrequencySetSuccessEvent() );
  }
}


void 
TrackerConfiguration::RequestAddTool( const TrackerToolConfiguration *tool )
{
  InternalAddTool( tool, false );
}


void 
TrackerConfiguration::RequestAddReference( const TrackerToolConfiguration *tool )
{
  InternalAddTool( tool, true );
}


SerialCommunicatingTrackerConfiguration::SerialCommunicatingTrackerConfiguration()
{
  igstk::SerialCommunication::PortNumberType DEFAULT_COM_PORT = 
    igstk::SerialCommunication::PortNumber0;
  igstk::SerialCommunication::BaudRateType DEFAULT_BAUD_RATE =
    igstk::SerialCommunication::BaudRate115200;
  igstk::SerialCommunication::DataBitsType DEFAULT_DATA_BITS =
    igstk::SerialCommunication::DataBits8;
  igstk::SerialCommunication::ParityType DEFAULT_PARITY =
    igstk::SerialCommunication::NoParity;
  igstk::SerialCommunication::StopBitsType DEFAULT_STOP_BITS =
    igstk::SerialCommunication::StopBits1;
  igstk::SerialCommunication::HandshakeType DEFAULT_HANDSHAKE = 
    igstk::SerialCommunication::HandshakeOff;

  m_COMPort   = DEFAULT_COM_PORT;
  m_BaudRate = DEFAULT_BAUD_RATE;
  m_DataBits = DEFAULT_DATA_BITS;
  m_Parity = DEFAULT_PARITY;
  m_StopBits = DEFAULT_STOP_BITS;
  m_Handshake = DEFAULT_HANDSHAKE;
}


SerialCommunicatingTrackerConfiguration::~SerialCommunicatingTrackerConfiguration()
{
}


TrackerToolConfiguration::TrackerToolConfiguration() 
{
  this->m_CalibrationTransform.SetToIdentity(
    igstk::TimeStamp::GetLongestPossibleTime());
};


TrackerToolConfiguration::TrackerToolConfiguration(const 
  TrackerToolConfiguration &other)
{
  this->m_CalibrationTransform = other.m_CalibrationTransform;
}


TrackerToolConfiguration::~TrackerToolConfiguration()
{
}

} // end of name space
