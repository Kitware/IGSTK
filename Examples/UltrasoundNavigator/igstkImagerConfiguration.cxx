/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkImagerConfiguration.h"

namespace igstk
{
  
  
ImagerConfiguration::ImagerConfiguration()//: m_ReferenceTool( NULL )
{
}


ImagerConfiguration::~ImagerConfiguration()
{
  std::vector< ImagerToolConfiguration * >::iterator it;
  std::vector< ImagerToolConfiguration * >::iterator endIt = 
    this->m_ImagerToolList.end();

  for(it=this->m_ImagerToolList.begin(); it!=endIt; it++)
  {    
    delete (*it);
  }
  this->m_ImagerToolList.clear();
}


void
ImagerConfiguration::RequestSetFrequency( double frequency )
{
  if( frequency<=0 || frequency>GetMaximalRefreshRate() )
  {
    std::ostringstream msg;
    msg.str("");
    msg<<"Invalid imager frequency specified.";
    msg<<"Valid values are in [0,"<<GetMaximalRefreshRate()<<"].";

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
ImagerConfiguration::RequestAddTool( const ImagerToolConfiguration *tool )
{
  InternalAddTool( tool );
}



ImagerToolConfiguration::ImagerToolConfiguration() 
{
 
};


ImagerToolConfiguration::ImagerToolConfiguration(const 
  ImagerToolConfiguration &other)
{
  this->m_ToolUniqueIdentifier = other.m_ToolUniqueIdentifier;
  this->m_FrameDimensions[0] = other.m_FrameDimensions[0];
  this->m_FrameDimensions[1] = other.m_FrameDimensions[1];
  this->m_FrameDimensions[2] = other.m_FrameDimensions[2];
  this->m_PixelDepth = other.m_PixelDepth;
}

ImagerToolConfiguration::~ImagerToolConfiguration()
{
}

void 
ImagerToolConfiguration::GetFrameDimensions(unsigned int* dims) const
{
  dims[0] = m_FrameDimensions[0];
  dims[1] = m_FrameDimensions[1];
  dims[2] = m_FrameDimensions[2];
}

void 
ImagerToolConfiguration::SetFrameDimensions(unsigned int* dims)
{
  this->m_FrameDimensions[0] = dims[0];
  this->m_FrameDimensions[1] = dims[1];
  this->m_FrameDimensions[2] = dims[2];
}

void  
ImagerToolConfiguration::SetPixelDepth (unsigned int value)
{
  m_PixelDepth = value;
}
  
unsigned int 
ImagerToolConfiguration::GetPixelDepth ( void ) const
{
  return this->m_PixelDepth;
}

SocketCommunicatingImagerConfiguration::SocketCommunicatingImagerConfiguration()
{
  m_SocketPort = 0;
}


SocketCommunicatingImagerConfiguration::~SocketCommunicatingImagerConfiguration()
{
}

SerialCommunicatingImagerConfiguration::SerialCommunicatingImagerConfiguration()
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


SerialCommunicatingImagerConfiguration::~SerialCommunicatingImagerConfiguration()
{
}

void 
SerialCommunicatingImagerConfiguration::RequestSetCOMPort(
  igstk::SerialCommunication::PortNumberType portNumber)
{
  switch( portNumber ) 
  {
  case igstk::SerialCommunication::PortNumber0:
  case igstk::SerialCommunication::PortNumber1:
  case igstk::SerialCommunication::PortNumber2:
  case igstk::SerialCommunication::PortNumber3:
  case igstk::SerialCommunication::PortNumber4:
  case igstk::SerialCommunication::PortNumber5:
  case igstk::SerialCommunication::PortNumber6:
  case igstk::SerialCommunication::PortNumber7:
    this->m_COMPort = portNumber;
    this->InvokeEvent( ComPortSetEvent() );
    break;
  default:
    ComPortSetErrorEvent errorEvt;
    std::ostringstream msg;
    msg<<"Invalid COM port ("<<portNumber<<"). Supported ports are ";
    msg<<"in the range ["<<igstk::SerialCommunication::PortNumber0<<",";
    msg<<igstk::SerialCommunication::PortNumber7<<"].";
    errorEvt.Set( msg.str() );
    this->InvokeEvent( errorEvt );
  }
}


void 
SerialCommunicatingImagerConfiguration::RequestSetBaudRate( 
  igstk::SerialCommunication::BaudRateType baudRate)
{
  switch( baudRate ) 
  {
  case igstk::SerialCommunication::BaudRate9600:
  case igstk::SerialCommunication::BaudRate19200:
  case igstk::SerialCommunication::BaudRate38400:
  case igstk::SerialCommunication::BaudRate57600:
  case igstk::SerialCommunication::BaudRate115200:
    this->m_BaudRate = baudRate;
    this->InvokeEvent( BaudRateSetEvent() );
    break;
  default:
    BaudRateSetErrorEvent errorEvt;
    std::ostringstream msg;
    msg<<"Invalid baud rate ("<<baudRate<<")";
    errorEvt.Set( msg.str() );
    this->InvokeEvent( errorEvt );
  }
}


void 
SerialCommunicatingImagerConfiguration::RequestSetDataBits( 
  igstk::SerialCommunication::DataBitsType dataBits)
{
  switch( dataBits ) 
  {
  case igstk::SerialCommunication::DataBits7:
  case igstk::SerialCommunication::DataBits8:
    this->m_DataBits = dataBits;
    this->InvokeEvent( DataBitsSetEvent() );
    break;
  default:
    DataBitsSetErrorEvent errorEvt;
    std::ostringstream msg;
    msg<<"Invalid data bits value ("<<dataBits<<")";
    errorEvt.Set( msg.str() );
    this->InvokeEvent( errorEvt );
  }
}

void 
SerialCommunicatingImagerConfiguration::RequestSetParity( 
  igstk::SerialCommunication::ParityType parity)
{
  switch( parity ) 
  {
  case igstk::SerialCommunication::NoParity:
  case igstk::SerialCommunication::OddParity:
  case igstk::SerialCommunication::EvenParity:
    this->m_Parity = parity;
    this->InvokeEvent( ParitySetEvent() );
  default:
    ParitySetErrorEvent errorEvt;
    std::ostringstream msg;
    msg<<"Invalid parity value ("<<parity<<")";
    errorEvt.Set( msg.str() );
    this->InvokeEvent( errorEvt );
  }
}

void 
SerialCommunicatingImagerConfiguration::RequestSetStopBits( 
  igstk::SerialCommunication::StopBitsType stopBits)
{
  switch( stopBits ) 
  {
  case igstk::SerialCommunication::StopBits1:
  case igstk::SerialCommunication::StopBits2:
    this->m_StopBits = stopBits;
    this->InvokeEvent( StopBitsSetEvent() );
  default:
    StopBitsSetErrorEvent errorEvt;
    std::ostringstream msg;
    msg<<"Invalid stop bits value ("<<stopBits<<")";
    errorEvt.Set( msg.str() );
    this->InvokeEvent( errorEvt );
  }
}


void 
SerialCommunicatingImagerConfiguration::RequestSetHandshake( 
  igstk::SerialCommunication::HandshakeType handShake)
{
  switch( handShake ) 
  {
  case igstk::SerialCommunication::HandshakeOff:
  case igstk::SerialCommunication::HandshakeOn:
    this->m_Handshake = handShake;
    this->InvokeEvent( HandshakeSetEvent() );
  default:
    HandshakeSetErrorEvent errorEvt;
    std::ostringstream msg;
    msg<<"Invalid handshake value ("<<handShake<<")";
    errorEvt.Set( msg.str() );
    this->InvokeEvent( errorEvt );
  }
}

} // end of name space
