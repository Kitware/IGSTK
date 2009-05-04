/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerConfiguration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagerConfiguration_h
#define __igstkImagerConfiguration_h

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class ImagerConfiguration
* 
* \brief Superclass for the different Imager configurations.
*
* This class functions as an adapter between GUI/file reader classes and
* the ImagerController class. Its sole purpose is to serve as a container for 
* the parameters defining the desired Imager and tool setup. 
*
* In this header file, we also define a base class for all Imagers that 
* communicate using a serial port. 
*
* The Imager classes only perform "static" error checking. That is, they check
* the data for adherence to known values (e.g. Imager frequency is within the
* manufacturer specified bounds). "Dynamic" error checking is relegated to the 
* ImagerController (e.g. com port can be opened for serial communication).
*/


/**
 * \class ImagerToolConfiguration
 * Generic Imager tool settings container with the variables common to all 
 * tools.
 */
class ImagerToolConfiguration
{
public:
  ImagerToolConfiguration();
  ImagerToolConfiguration(const ImagerToolConfiguration &other);
  virtual ~ImagerToolConfiguration();

  void SetFrameDimensions ( unsigned int * );
  void GetFrameDimensions ( unsigned int * ) const;

  void  SetPixelDepth (unsigned int );
  unsigned int GetPixelDepth ( ) const;

  /**
   *Get the Imager tool type as a string. The only reason that this method 
   *exists is to make the ImagerToolConfiguration class an abstract base class. 
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
 * \class ImagerConfiguration
 * Abstract base class for all Imager configurations.
 */
class ImagerConfiguration : public Object
{
public:

  friend class ImagerController;

  //standard typedefs
  igstkStandardClassBasicTraitsMacro( ImagerConfiguration, igstk::Object )

  /** This event is generated when the frequency was set successfuly. */
  igstkEventMacro( FrequencySetSuccessEvent, IGSTKEvent );

  /** 
   * This event is generated when the frequency setting fails 
   * (user specified a value that was zero, negative, or greater than the 
   * Imager's maximal refresh rate). 
   */
  igstkLoadedEventMacro( FrequencySetFailureEvent, 
                                                  IGSTKErrorEvent, std::string);

  /** This event is generated when a tool was added successfuly. */
  igstkEventMacro( AddToolSuccessEvent, IGSTKEvent );

  /** 
   * This event is generated when the tool was not added. This can happen
   * when the internal tool data is invalid (e.g. wireless polaris tool with
   * an empty srom file specification) or if the specified tool does not match
   * the Imager type (e.g. TerasonToolConfiguration given to a 
   * ImagingSourceImagerConfiguration).
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
   * This method adds a tool to the Imager configuration. 
   *
   * NOTE: This tool will not be used as a dynamic reference. If you want to
   *       add the tool as a dynamic reference use the RequestAddReferenceTool()
   *       method.
   * The method generates two  events: AddToolSuccessEvent and 
   * AddToolFailureEvent.
   */
  void RequestAddTool( const ImagerToolConfiguration *tool );

  /**
   * Each Imager has its manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate()=0;

  igstkGetMacro( Frequency, double );


  igstkGetMacro( ImagerToolList, std::vector< ImagerToolConfiguration * > )

protected:

  ImagerConfiguration();
  virtual ~ImagerConfiguration();

  virtual void InternalAddTool( const ImagerToolConfiguration *tool )=0;

  //the frequency at which the Imager is queried for new transforms [Hz]
  double m_Frequency;

  //list of tools we want to connect to the Imager (excluding reference tool)
  std::vector< ImagerToolConfiguration * > m_ImagerToolList;

//  ImagerToolConfiguration * m_ReferenceTool; 
};

/**
 * \class SocketCommunicatingImagerConfiguration
 * A base class for all imager configurations that require socket 
 * communication.
 */
class SocketCommunicatingImagerConfiguration : public ImagerConfiguration
{
public:
  igstkSetMacro( SocketPort, unsigned int );
  igstkGetMacro( SocketPort, unsigned int );

protected:
  SocketCommunicatingImagerConfiguration();
  virtual ~SocketCommunicatingImagerConfiguration();

private:
  unsigned int m_SocketPort;
  //std::string m_HostName;
};

/**
 * \class SerialCommunicatingImagerConfiguration
 * A base class for all Imager configurations that require serial 
 * communication.
 */
class SerialCommunicatingImagerConfiguration : public ImagerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( SerialCommunicatingImagerConfiguration, 
                                      ImagerConfiguration )


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
  SerialCommunicatingImagerConfiguration();
  virtual ~SerialCommunicatingImagerConfiguration();

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
