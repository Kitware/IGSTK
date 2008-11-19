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
* \brief Superclass for the different tracker configurations.
*
* This class functions as an adapter between GUI/file reader classes and
* the ImagerController class. Its sole purpose is to serve as a container for 
* the parameters defining the desired tracker and tool setup. 
*
* In this header file, we also define a base class for all trackers that 
* communicate using a serial port. 
*
* The tracker classes only perform "static" error checking. That is, they check
* the data for adherence to known values (e.g. tracker frequency is within the
* manufacturer specified bounds). "Dynamic" error checking is relegated to the 
* ImagerController (e.g. com port can be opened for serial communication).
*/


/**
 * Generic tracker tool settings container with the variables common to all 
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
   *Get the tracker tool type as a string. The only reason that this method 
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
 * Abstract base class for all tracker configurations.
 */
class ImagerConfiguration : public Object
{
public:

  ImagerConfiguration();
  virtual ~ImagerConfiguration();

  /** This event is generated when the frequency was set successfuly. */
  igstkEventMacro( FrequencySetSuccessEvent, IGSTKEvent );

  /** 
   * This event is generated when the frequency setting fails 
   * (user specified a value that was zero, negative, or greater than the 
   * tracker's maximal refresh rate). 
   */
  igstkLoadedEventMacro( FrequencySetFailureEvent, IGSTKErrorEvent, std::string );

  /** This event is generated when a tool was added successfuly. */
  igstkEventMacro( AddToolSuccessEvent, IGSTKEvent );

  /** 
   * This event is generated when the tool was not added. This can happen
   * when the internal tool data is invalid (e.g. wireless polaris tool with
   * an empty srom file specification) or if the specified tool does not match
   * the tracker type (e.g. MicronToolConfiguration given to a 
   * PolarisImagerConfiguration).
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
   * This method adds a tool to the tracker configuration. 
   *
   * NOTE: This tool will not be used as a dynamic reference. If you want to
   *       add the tool as a dynamic reference use the RequestAddReferenceTool()
   *       method.
   * The method generates two  events: AddToolSuccessEvent and 
   * AddToolFailureEvent.
   */
  void RequestAddTool( const ImagerToolConfiguration *tool );

  /**
   * Each tracker has its manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate()=0;

  igstkGetMacro( Frequency, double );


  igstkGetMacro( ImagerToolList, std::vector< ImagerToolConfiguration * > )

protected:

  virtual void InternalAddTool( const ImagerToolConfiguration *tool, 
                                bool isReference )=0;

  //the frequency at which the tracker is queried for new transforms [Hz]
  double m_Frequency;

  //the list of tools we want to connect to the tracker (excluding the reference tool)
  std::vector< ImagerToolConfiguration * > m_ImagerToolList;

  ImagerToolConfiguration * m_ReferenceTool; 
};

/**
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
//  std::string m_HostName;
};


} // end of name space
#endif
