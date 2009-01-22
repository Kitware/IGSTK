/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicatingTrackerConfigurationXMLFileReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <itksys/SystemTools.hxx>

#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"

namespace igstk
{


void 
SerialCommunicatingTrackerConfigurationXMLFileReader::StartElement( 
  const char * name, 
  const char **atts )
{
  //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
  //start of serial_communication data
  if( itksys::SystemTools::Strucmp( name, "serial_communication" ) == 0 ) 
    {
    if( !m_ReadingTrackerConfiguration )
      {
      throw FileFormatException( 
        "\"serial_communication\" tag not nested in \"tracking_system\" tag." );
      }
    this->m_ReadingSerialCommunication = true;
    }          //com port
  else if( itksys::SystemTools::Strucmp( name, "com_port" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "baud_rate" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "data_bits" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "parity" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "stop_bits" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "hand_shake" ) == 0 )
    {
    if( !m_ReadingSerialCommunication )
      {
      throw FileFormatException( "Tag not nested correctly in xml structure." );
      }
    }
}


void 
SerialCommunicatingTrackerConfigurationXMLFileReader::EndElement( 
const char *name )
{
  //let the super class try to analyze the current tag
  Superclass::EndElement( name );
  
  //end of serial_communication section
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "serial_communication" ) == 0 )
    {
    this->m_ReadingSerialCommunication = false;
    //see that we got all the data, perhaps replace with a more
    //specific message with regard to the missing tag(s)
    if( !( m_HaveComPort && m_HaveBaudRate && m_HaveDataBits && m_HaveParity &&
           m_HaveStopBits && m_HaveHandshake ) )
      {
      throw 
        FileFormatException( std::string( 
          "Missing tag(s) in serial communication section." ) );
      }
    }
  //com_port tag
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "com_port" ) == 0 )
    {
    ProcessComport();
    this->m_HaveComPort = true;
    }                //baud_rate tag
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "baud_rate" ) == 0 )
    {
    ProcessBaudRate();
    this->m_HaveBaudRate = true;
    }              //data_bits tag
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "data_bits" ) == 0 )
    {
    ProcessDataBits();
    this->m_HaveDataBits = true;
    }              //parity tag
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "parity" ) == 0 )
    {
    ProcessParity();
    this->m_HaveParity = true;
    }              //stop_bits tag
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "stop_bits" ) == 0 )
    {
    ProcessStopBits();
    this->m_HaveStopBits = true;
    }              //hand_shake tag
  if( m_ReadingSerialCommunication &&
      itksys::SystemTools::Strucmp( name, "hand_shake" ) == 0 )
    {
    ProcessHandShake();
    this->m_HaveHandshake = true;
    }
}


void 
SerialCommunicatingTrackerConfigurationXMLFileReader::ProcessComport() 
throw ( FileFormatException )
{
  unsigned comPort;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>comPort;

  if( !instr.eof() )
    {
    throw FileFormatException( 
    "Error in com port number, possibly non numeric values" );
    }
  this->m_COMPort = 
    static_cast<igstk::SerialCommunication::PortNumberType>( comPort );
}


void 
SerialCommunicatingTrackerConfigurationXMLFileReader::ProcessBaudRate() 
throw ( FileFormatException )
{
  unsigned baudRate;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>baudRate;

  if( !instr.eof() )
    {
    throw FileFormatException( 
    "Error in baud rate, possibly non numeric values" );
    }
  switch( baudRate )
    {
    case igstk::SerialCommunication::BaudRate9600:
      this->m_BaudRate = igstk::SerialCommunication::BaudRate9600;
      break;
    case igstk::SerialCommunication::BaudRate19200:
      this->m_BaudRate = igstk::SerialCommunication::BaudRate19200;
      break;
    case igstk::SerialCommunication::BaudRate38400:
      this->m_BaudRate = igstk::SerialCommunication::BaudRate38400;
      break;
    case igstk::SerialCommunication::BaudRate57600:
      this->m_BaudRate = igstk::SerialCommunication::BaudRate57600;
      break;
    case igstk::SerialCommunication::BaudRate115200:
      this->m_BaudRate = igstk::SerialCommunication::BaudRate115200;
      break;
    default:
      throw FileFormatException( 
        "Error in baud rate, invalid value." );
    }
}


void 
SerialCommunicatingTrackerConfigurationXMLFileReader::ProcessDataBits() 
throw ( FileFormatException )
{
  unsigned dataBits;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>dataBits;

  if( !instr.eof() )
    {
    throw FileFormatException( 
    "Error in data bits, possibly non numeric values" );
    }
  if( dataBits == igstk::SerialCommunication::DataBits7 ||
      dataBits == igstk::SerialCommunication::DataBits8 )
    {
    this->m_DataBits = 
      static_cast<igstk::SerialCommunication::DataBitsType>( dataBits );
    }
  else
    {
    throw FileFormatException( "Error in data bits, invalid value." );
    }
}
  

void 
SerialCommunicatingTrackerConfigurationXMLFileReader::ProcessParity() 
throw ( FileFormatException )
{
  if( this->m_CurrentTagData.length() > 1 )
    {
    throw FileFormatException( 
    "Error in parity value, should be a single charecter." );
    }
  switch( this->m_CurrentTagData[0] )
    {
    case igstk::SerialCommunication::NoParity:
      this->m_Parity = igstk::SerialCommunication::NoParity;
      break;
    case igstk::SerialCommunication::OddParity:
      this->m_Parity = igstk::SerialCommunication::OddParity;
      break;
    case igstk::SerialCommunication::EvenParity:
      this->m_Parity = igstk::SerialCommunication::EvenParity;
      break;
    default:
      throw FileFormatException( "Error in parity, invalid value." );
    }
}
  

void 
SerialCommunicatingTrackerConfigurationXMLFileReader::ProcessStopBits() 
throw ( FileFormatException )
{
  unsigned stopBits;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>stopBits;

  if( !instr.eof() )
    {
    throw FileFormatException( 
      "Error in stop bits, possibly non numeric values" );
    }
  if( stopBits == igstk::SerialCommunication::StopBits1 ||
      stopBits == igstk::SerialCommunication::StopBits2 )
    {
    this->m_StopBits = 
      static_cast<igstk::SerialCommunication::StopBitsType>( stopBits );
    }
  else
    {
    throw FileFormatException( 
      "Error in stop bits, invalid value." );
    }
}
  

void 
SerialCommunicatingTrackerConfigurationXMLFileReader::ProcessHandShake() 
throw ( FileFormatException )
{
  unsigned handshake;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>handshake;

  if( !instr.eof() )
    {
    throw FileFormatException( 
      "Error in handshake data, possibly non numeric values" );
    }
  if( handshake == igstk::SerialCommunication::HandshakeOn ||
      handshake == igstk::SerialCommunication::HandshakeOff )
    {
    this->m_Handshake = 
      static_cast<igstk::SerialCommunication::HandshakeType>( handshake );
    }
  else
    {
    throw FileFormatException( "Error in handshake data, invalid value." );
    }
}

} //namespace
