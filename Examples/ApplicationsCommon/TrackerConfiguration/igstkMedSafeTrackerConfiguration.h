#ifndef __igstkMedSafeTrackerConfiguration_h
#define __igstkMedSafeTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * A 3D Guidance MedSafe tool.
 */
class MedSafeToolConfiguration : public TrackerToolConfiguration
{
public:
  MedSafeToolConfiguration();
  MedSafeToolConfiguration(const MedSafeToolConfiguration &other);
  virtual ~MedSafeToolConfiguration();

  /**
   * Set the tool's physical port number.
   */
  igstkSetMacro( PortNumber, unsigned );
  igstkGetMacro( PortNumber, unsigned );

  virtual std::string GetToolTypeAsString();

protected:
  unsigned m_PortNumber;
};



/**
 * Configuration for the Ascension tracker.
 */
class MedSafeTrackerConfiguration : 
  public SerialCommunicatingTrackerConfiguration
{
public:
  MedSafeTrackerConfiguration();
  virtual ~MedSafeTrackerConfiguration();

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
