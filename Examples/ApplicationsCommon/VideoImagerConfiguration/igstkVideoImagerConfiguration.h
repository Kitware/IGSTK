/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerConfiguration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVideoImagerConfiguration_h
#define __igstkVideoImagerConfiguration_h

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class VideoImagerConfiguration
*
* \brief Superclass for the different VideoImager configurations.
*
* This class functions as an adapter between GUI/file reader classes and
* the VideoImagerController class. Its sole purpose is to serve as a container for
* the parameters defining the desired VideoImager and tool setup.
*
* In this header file, we also define a base class for all VideoImagers that
* communicate using a serial port.
*
* The VideoImager classes only perform "static" error checking. That is, they check
* the data for adherence to known values (e.g. VideoImager frequency is within the
* manufacturer specified bounds). "Dynamic" error checking is relegated to the
* VideoImagerController (e.g. com port can be opened for serial communication).
*/


/**
 * \class VideoImagerToolConfiguration
 * Generic VideoImager tool settings container with the variables common to all
 * tools.
 */
class VideoImagerToolConfiguration
{
public:
  VideoImagerToolConfiguration();
  VideoImagerToolConfiguration(const VideoImagerToolConfiguration &other);
  virtual ~VideoImagerToolConfiguration();

  void SetFrameDimensions ( unsigned int * );
  void GetFrameDimensions ( unsigned int * ) const;

  void  SetPixelDepth (unsigned int );
  unsigned int GetPixelDepth ( ) const;

  /**
   *Get the VideoImager tool type as a string. The only reason that this method
   *exists is to make the VideoImagerToolConfiguration class an abstract base class.
   */
  virtual std::string GetToolTypeAsString() = 0;

  igstkSetMacro( ToolUniqueIdentifier, std::string );
  igstkGetMacro( ToolUniqueIdentifier, std::string );

protected:

  //the tool's calibration transformation
  std::string m_ToolUniqueIdentifier;

  unsigned int m_FrameDimensions[3];
  unsigned int m_PixelDepth;

};

/**
 * \class VideoImagerConfiguration
 * Abstract base class for all VideoImager configurations.
 */
class VideoImagerConfiguration : public Object
{
public:

  friend class VideoImagerController;

  //standard typedefs
  igstkStandardClassBasicTraitsMacro( VideoImagerConfiguration, igstk::Object )

  /** This event is generated when the frequency was set successfuly. */
  igstkEventMacro( FrequencySetSuccessEvent, IGSTKEvent );

  /**
   * This event is generated when the frequency setting fails
   * (user specified a value that was zero, negative, or greater than the
   * VideoImager's maximal refresh rate).
   */
  igstkLoadedEventMacro( FrequencySetFailureEvent,
                                                  IGSTKErrorEvent, std::string);

  /** This event is generated when a tool was added successfuly. */
  igstkEventMacro( AddToolSuccessEvent, IGSTKEvent );

  /**
   * This event is generated when the tool was not added. This can happen
   * when the internal tool data is invalid (e.g. wireless polaris tool with
   * an empty srom file specification) or if the specified tool does not match
   * the VideoImager type (e.g. TerasonToolConfiguration given to a
   * ImagingSourceVideoImagerConfiguration).
   */
  igstkLoadedEventMacro( AddToolFailureEvent, IGSTKErrorEvent, std::string )

  /**
   * This method sets the data acquisition frequency.
   *
   * The method generates two  events: FrequencySetSuccessEvent and
   * FrequencySetFailureEvent.
   */
  void RequestSetFrequency( double frequency );

  /**
   * This method adds a tool to the VideoImager configuration.
   *
   * NOTE: This tool will not be used as a dynamic reference. If you want to
   *       add the tool as a dynamic reference use the RequestAddReferenceTool()
   *       method.
   * The method generates two  events: AddToolSuccessEvent and
   * AddToolFailureEvent.
   */
  void RequestAddTool( const VideoImagerToolConfiguration *tool );

  /**
   * Each VideoImager has its manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate()=0;

  igstkGetMacro( Frequency, double );


  igstkGetMacro( VideoImagerToolList, std::vector< VideoImagerToolConfiguration * > )

protected:

  VideoImagerConfiguration();
  virtual ~VideoImagerConfiguration();

  virtual void InternalAddTool( const VideoImagerToolConfiguration *tool )=0;

  //the frequency at which the VideoImager is queried for new transforms [Hz]
  double m_Frequency;

  //list of tools we want to connect to the VideoImager (excluding reference tool)
  std::vector< VideoImagerToolConfiguration * > m_VideoImagerToolList;

//  VideoImagerToolConfiguration * m_ReferenceTool;
};

/**
 * \class SocketCommunicatingVideoImagerConfiguration
 * A base class for all VideoImager configurations that require socket
 * communication.
 */
class SocketCommunicatingVideoImagerConfiguration : public VideoImagerConfiguration
{
public:
  igstkSetMacro( SocketPort, unsigned int );
  igstkGetMacro( SocketPort, unsigned int );

protected:
  SocketCommunicatingVideoImagerConfiguration();
  virtual ~SocketCommunicatingVideoImagerConfiguration();

private:
  unsigned int m_SocketPort;
  //std::string m_HostName;
};

/**
 * \class SerialCommunicatingVideoImagerConfiguration
 * A base class for all VideoImager configurations that require serial
 * communication.
 */
class SerialCommunicatingVideoImagerConfiguration : public VideoImagerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( SerialCommunicatingVideoImagerConfiguration,
                                      VideoImagerConfiguration )


  /** This event is generated when the COM port was set successfuly. */
  igstkEventMacro( ComPortSetEvent, IGSTKEvent );


  /**
   * This event is generated when the given COM port is invalid.
   */
  igstkEventMacro( ComPortSetErrorEvent, IGSTKErrorWithStringEvent );


  /**
   * Set the com port to the given value. Generates ComPortSetEvent if
   * successful otherwise ComPortSetErrorEvent.
   */
  void RequestSetCOMPort(igstk::SerialCommunication::PortNumberType portNumber);
  igstkGetMacro( COMPort, igstk::SerialCommunication::PortNumberType );


  /** This event is generated when the baud rate was set successfuly. */
  igstkEventMacro( BaudRateSetEvent, IGSTKEvent );


  /**
   * This event is generated when the given baud rate is invalid.
   */
  igstkEventMacro( BaudRateSetErrorEvent, IGSTKErrorWithStringEvent );


  /**
   * Set the baud rate to the given value. Generates BaudRateSetEvent if
   * successful otherwise BaudRateSetErrorEvent.
   */
  void RequestSetBaudRate( igstk::SerialCommunication::BaudRateType baudRate );
  igstkGetMacro( BaudRate, igstk::SerialCommunication::BaudRateType );


  igstkEventMacro( DataBitsSetEvent, IGSTKEvent );


  igstkEventMacro( DataBitsSetErrorEvent, IGSTKErrorWithStringEvent );

  void RequestSetDataBits( igstk::SerialCommunication::DataBitsType dataBits );
  igstkGetMacro( DataBits, igstk::SerialCommunication::DataBitsType );

  igstkEventMacro( ParitySetEvent, IGSTKEvent );


  igstkEventMacro( ParitySetErrorEvent, IGSTKErrorWithStringEvent );

  void RequestSetParity( igstk::SerialCommunication::ParityType parity);
  igstkGetMacro( Parity, igstk::SerialCommunication::ParityType );

  igstkEventMacro( StopBitsSetEvent, IGSTKEvent );


  igstkEventMacro( StopBitsSetErrorEvent, IGSTKErrorWithStringEvent );


  void RequestSetStopBits( igstk::SerialCommunication::StopBitsType stopBits );
  igstkGetMacro( StopBits, igstk::SerialCommunication::StopBitsType );

  igstkEventMacro( HandshakeSetEvent, IGSTKEvent );


  igstkEventMacro( HandshakeSetErrorEvent, IGSTKErrorWithStringEvent );

  void RequestSetHandshake(igstk::SerialCommunication::HandshakeType handShake);
  igstkGetMacro( Handshake, igstk::SerialCommunication::HandshakeType );

protected:
  SerialCommunicatingVideoImagerConfiguration();
  virtual ~SerialCommunicatingVideoImagerConfiguration();

private:
  igstk::SerialCommunication::PortNumberType m_COMPort;
  igstk::SerialCommunication::BaudRateType m_BaudRate;
  igstk::SerialCommunication::DataBitsType m_DataBits;
  igstk::SerialCommunication::ParityType m_Parity;
  igstk::SerialCommunication::StopBitsType m_StopBits;
  igstk::SerialCommunication::HandshakeType m_Handshake;
};


} // end of name space
#endif
