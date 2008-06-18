#ifndef __igstkAscensionTrackerConfiguration_h
#define __igstkAscensionTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * An Ascension tool.
 */
class AscensionToolConfiguration : public TrackerToolConfiguration
{
public:
  AscensionToolConfiguration();
  AscensionToolConfiguration(const AscensionToolConfiguration &other);
  virtual ~AscensionToolConfiguration();

  igstkSetMacro( BirdName, std::string );
  igstkGetMacro( BirdName, std::string );

  virtual std::string GetToolTypeAsString();

protected:
  std::string m_BirdName;
  int m_PortNumber;
};



/**
 * Configuration for the Ascension tracker.
 */
class AscensionTrackerConfiguration : 
  public SerialCommunicatingTrackerConfiguration
{
public:
  AscensionTrackerConfiguration();
  virtual ~AscensionTrackerConfiguration();

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );

private:
                  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

                   //maximal number of physical ports 
  static const unsigned MAXIMAL_PORT_NUMBER;
};

} // end of name space
#endif
