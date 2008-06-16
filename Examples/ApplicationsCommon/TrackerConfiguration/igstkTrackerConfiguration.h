#ifndef __igstkTrackerConfiguration_h
#define __igstkTrackerConfiguration_h

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class TrackerConfiguration
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
 * Generic tracker tool settings container with the variables common to all 
 * tools.
 */
class TrackerToolConfiguration
{
public:
  TrackerToolConfiguration();
  TrackerToolConfiguration(const TrackerToolConfiguration &other);
  virtual ~TrackerToolConfiguration();
      
  igstkSetMacro( CalibrationTransform, igstk::Transform );
  igstkGetMacro( CalibrationTransform, igstk::Transform );

  /**
   *Get the tracker tool type as a string. The only reason that this method 
   *exists is to make the TrackerToolConfiguration class an abstract base class. 
   */
  virtual std::string GetToolTypeAsString() = 0;

protected:
       //the tool's calibration transformation
  igstk::Transform   m_CalibrationTransform;
};



/**
 * Abstract base class for all tracker configurations.
 */
class TrackerConfiguration : public Object
{
public:

  TrackerConfiguration();
  virtual ~TrackerConfiguration();

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
   * PolarisTrackerConfiguration).
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
  TrackerToolConfiguration* GetReferenceTool() { 
    return this->m_ReferenceTool; }
  igstkGetMacro( TrackerToolList, std::vector< TrackerToolConfiguration * > )

protected:

  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference )=0;

       //the frequency at which the tracker is queried for new transforms [Hz]
  double m_Frequency;

             //the list of tools we want to connect to the tracker (excluding
             //the reference tool)
  std::vector< TrackerToolConfiguration * > m_TrackerToolList;

  TrackerToolConfiguration * m_ReferenceTool; 
};


/**
 * A base class for all tracker configurations that require serial 
 * communication.
 */
class SerialCommunicatingTrackerConfiguration : public TrackerConfiguration
{
public:
    igstkSetMacro( COMPort, igstk::SerialCommunication::PortNumberType );
    igstkGetMacro( COMPort, igstk::SerialCommunication::PortNumberType );

    igstkSetMacro( BaudRate, igstk::SerialCommunication::BaudRateType );
    igstkGetMacro( BaudRate, igstk::SerialCommunication::BaudRateType );

    igstkSetMacro( DataBits, igstk::SerialCommunication::DataBitsType );
    igstkGetMacro( DataBits, igstk::SerialCommunication::DataBitsType );

    igstkSetMacro( Parity, igstk::SerialCommunication::ParityType );
    igstkGetMacro( Parity, igstk::SerialCommunication::ParityType );

    igstkSetMacro( StopBits, igstk::SerialCommunication::StopBitsType );
    igstkGetMacro( StopBits, igstk::SerialCommunication::StopBitsType );

    igstkSetMacro( Handshake, igstk::SerialCommunication::HandshakeType );
    igstkGetMacro( Handshake, igstk::SerialCommunication::HandshakeType );

protected:
    SerialCommunicatingTrackerConfiguration();
    virtual ~SerialCommunicatingTrackerConfiguration();

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
