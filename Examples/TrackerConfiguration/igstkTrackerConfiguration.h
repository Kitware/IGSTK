/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfiguration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerConfiguration_h
#define __igstkTrackerConfiguration_h

#include "igstkSerialCommunication.h"

namespace igstk
{

/** 
* \class TrackerConfiguration
* 
* \brief Superclass for the different tracker configurations.
*
* This class functions as an adapter between GUI/file reader classes and
* the TrackerController class. Its sole purpose is to serve as a container for 
* the parameters defining the desired tracker and tool setup. 
*
* In this header file, we also define a base class for all trackers that 
* communicate using a serial port. 
*
* The tracker classes only perform "static" error checking. That is, they check
* the data for adherence to known values (e.g. tracker frequency is within the
* manufacturer specified bounds). "Dynamic" error checking is relegated to the 
* TrackerController (e.g. com port can be opened for serial communication).
*/


/**
 * \class TrackerToolConfiguration Generic tracker tool settings container with 
 *        the variables common to all tools (name and rigid calibration 
 *        transformation).
 */
class TrackerToolConfiguration
{
public:
  TrackerToolConfiguration();
  TrackerToolConfiguration(const TrackerToolConfiguration &other);
  virtual ~TrackerToolConfiguration();
  
  /**
   * Set and Get the tool's calibration transform.
   */
  igstkSetMacro( CalibrationTransform, igstk::Transform );
  igstkGetMacro( CalibrationTransform, igstk::Transform );


  /**
   * Set and Get the tool's human readable name. Note that this is not the 
   * unique tool identifier we get when invoking 
   * igstk::TrackerTool::GetTrackerToolIdentifier(), that identifier is set 
   * internally by the tracker. This identifier is later used by the user to 
   * retrieve the specific tool from the tracker controller.
   */
  igstkSetMacro( ToolName, std::string );
  igstkGetMacro( ToolName, std::string );


  /**
   *Get the tracker tool type as a string. The only reason that this method 
   *exists is to make the TrackerToolConfiguration class an abstract base class.
   */
  virtual std::string GetToolTypeAsString() = 0;

protected:
  //the tool's calibration transformation
  igstk::Transform   m_CalibrationTransform;
  //the tool's name
  std::string m_ToolName;
};


/**
 * \class TrackerConfiguration Abstract base class for all tracker 
 *        configurations.
 */
class TrackerConfiguration : public Object
{
public:

  friend class TrackerController;

  //standard typedefs
  igstkStandardClassBasicTraitsMacro( TrackerConfiguration, igstk::Object )

  /** This event is generated when the frequency was set successfuly. */
  igstkEventMacro( FrequencySetEvent, IGSTKEvent );

  /** 
   * This event is generated when the frequency setting fails 
   * (user specified a value that was zero, negative, or greater than the 
   * tracker's maximal refresh rate). 
   */
  igstkEventMacro( FrequencySetErrorEvent, IGSTKErrorWithStringEvent );

  /** This event is generated when a tool was added successfuly. */
  igstkEventMacro( AddToolSuccessEvent, IGSTKEvent );

  /** 
   * This event is generated when the tool was not added. This can happen
   * when the internal tool data is invalid (e.g. wireless polaris tool with
   * an empty srom file specification) or if the specified tool does not match
   * the tracker type (e.g. MicronToolConfiguration given to a 
   * PolarisTrackerConfiguration).
   */
  igstkLoadedEventMacro( AddToolFailureEvent, IGSTKErrorEvent, std::string )

  /**
   * This method sets the data acquisition frequency.
   * 
   * The method generates two  events: FrequencySetSuccessEvent and 
   * FrequencySetErrorEvent.
   */
  void RequestSetFrequency( double frequency );

  /**
   * This method adds a tool to the tracker configuration. 
   *
   * @param tool The tool configuration we want to set.
   *
   * NOTE: This tool will not be used as a dynamic reference. If you want to
   *       add the tool as a dynamic reference use the RequestAddReferenceTool()
   *       method.
   * The method generates two  events: AddToolSuccessEvent and 
   * AddToolFailureEvent.
   */
  void RequestAddTool( const TrackerToolConfiguration *tool );

  /**
   * This method adds the tool as the dynamic reference for the tracker 
   * configuration. 
   *
   * NOTE: This tool will be the dynamic reference. The last invocation will be
   *       the one used.
   * The method generates two  events: AddToolSuccessEvent and 
   * AddToolFailureEvent.
   */
  void RequestAddReference( const TrackerToolConfiguration *tool );

  /**
   * Each tracker has its manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate()=0;

  igstkGetMacro( Frequency, double );
  
protected:

  TrackerConfiguration();
  virtual ~TrackerConfiguration();

  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference )=0;

  //the frequency at which the tracker is queried for new transforms [Hz]
  double m_Frequency;

  //the list of tools we want to connect to the tracker (excluding
  //the reference tool)
  std::map<std::string, TrackerToolConfiguration *>    m_TrackerToolList;   
  TrackerToolConfiguration *                           m_ReferenceTool; 
};


/**
 * \class SerialCommunicatingTrackerConfiguration A base class for all tracker 
 *        configurations that require serial communication.
 */
class SerialCommunicatingTrackerConfiguration : public TrackerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( SerialCommunicatingTrackerConfiguration, 
                                      TrackerConfiguration )


  /** This event is generated when the COM port was set successfuly. */
  igstkEventMacro( ComPortSetEvent, IGSTKEvent );


  /** 
   * This event is generated when the given COM port is invalid. 
   */
  igstkEventMacro( ComPortSetErrorEvent, IGSTKErrorWithStringEvent );


  /**
   * Set the com port to the given value. Generates ComPortSetEvent if 
   * successful otherwise ComPortSetErrorEvent.*/
  void RequestSetCOMPort( igstk::SerialCommunication::PortNumberType 
    portNumber);
  igstkGetMacro( COMPort, igstk::SerialCommunication::PortNumberType );


  /** This event is generated when the baud rate was set successfuly. */
  igstkEventMacro( BaudRateSetEvent, IGSTKEvent );


  /** 
   * This event is generated when the given baud rate is invalid. 
   */
  igstkEventMacro( BaudRateSetErrorEvent, IGSTKErrorWithStringEvent );


  /**
  * Set the baud rate to the given value. Generates BaudRateSetEvent if 
  * successful otherwise BaudRateSetErrorEvent.*/
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

  void RequestSetHandshake( igstk::SerialCommunication::HandshakeType 
    handShake );
  igstkGetMacro( Handshake, igstk::SerialCommunication::HandshakeType );

protected:
  SerialCommunicatingTrackerConfiguration();
  virtual ~SerialCommunicatingTrackerConfiguration();

private:
  igstk::SerialCommunication::PortNumberType   m_COMPort;
  igstk::SerialCommunication::BaudRateType     m_BaudRate;
  igstk::SerialCommunication::DataBitsType     m_DataBits;
  igstk::SerialCommunication::ParityType       m_Parity;
  igstk::SerialCommunication::StopBitsType     m_StopBits;
  igstk::SerialCommunication::HandshakeType    m_Handshake;
};

} // end of name space
#endif
