/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicatingTrackerConfigurationXMLFileReader_h
#define __igstkSerialCommunicatingTrackerConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkTrackerConfigurationXMLFileReaderBase.h"


namespace igstk
{
/** \class SerialCommunicatingTrackerConfigurationXMLFileReader
 * 
 *  \brief This class is an abstract reader for xml files containing a 
 *         tracker configuration that uses serial communication. 
 * 
 *         This class is an abstract reader for xml files containing a 
 *         tracker configuration that uses serial communication. 
 *         The specific type of tracker is defined by the subclass.
 */
class SerialCommunicatingTrackerConfigurationXMLFileReader : 
   public TrackerConfigurationXMLFileReaderBase 
{
public:

  //standard typedefs
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader    Self;
  typedef TrackerConfigurationXMLFileReaderBase                   Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( SerialCommunicatingTrackerConfigurationXMLFileReader, 
                TrackerConfigurationXMLFileReaderBase );

  /**
   * Method called when a new xml tag start is encountered.
   */
  virtual void StartElement( const char * name, const char **atts ); 

  /**
   * Method called when an xml tag end is encountered.
   */
  virtual void EndElement( const char *name ); 


protected:
  //this is the constructor that is called by the factory to 
  //create a new object
  SerialCommunicatingTrackerConfigurationXMLFileReader() : 
         TrackerConfigurationXMLFileReaderBase(),
         m_ReadingSerialCommunication( false ),
         m_HaveComPort( false ),  
         m_HaveBaudRate( false ),  
         m_HaveDataBits( false ),  
         m_HaveParity( false ),  
         m_HaveStopBits( false ),  
         m_HaveHandshake( false ) { }

  virtual ~SerialCommunicatingTrackerConfigurationXMLFileReader() { }

  void ProcessComport() throw ( FileFormatException );
  void ProcessBaudRate() throw ( FileFormatException );
  void ProcessDataBits() throw ( FileFormatException );
  void ProcessParity() throw ( FileFormatException );
  void ProcessStopBits() throw ( FileFormatException );
  void ProcessHandShake() throw ( FileFormatException );


  bool m_ReadingSerialCommunication;

  igstk::SerialCommunication::PortNumberType m_COMPort;
  igstk::SerialCommunication::BaudRateType   m_BaudRate;
  igstk::SerialCommunication::DataBitsType   m_DataBits;
  igstk::SerialCommunication::ParityType     m_Parity;
  igstk::SerialCommunication::StopBitsType   m_StopBits;
  igstk::SerialCommunication::HandshakeType  m_Handshake;

  bool m_HaveComPort;
  bool m_HaveBaudRate;
  bool m_HaveDataBits;
  bool m_HaveParity;
  bool m_HaveStopBits;
  bool m_HaveHandshake;

private:
  SerialCommunicatingTrackerConfigurationXMLFileReader( 
    const SerialCommunicatingTrackerConfigurationXMLFileReader & other );


  const SerialCommunicatingTrackerConfigurationXMLFileReader & operator=( 
    const SerialCommunicatingTrackerConfigurationXMLFileReader & right );
};


}
#endif //__igstkSerialCommunicatingTrackerConfigurationXMLFileReader_h
