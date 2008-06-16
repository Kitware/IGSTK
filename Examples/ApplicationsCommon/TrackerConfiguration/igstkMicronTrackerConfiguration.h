#ifndef __igstkMicronTrackerConfiguration_h
#define __igstkMicronTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * A micron tracker tool.
 */
class MicronToolConfiguration : public TrackerToolConfiguration
{
public:
  MicronToolConfiguration();
  MicronToolConfiguration(const MicronToolConfiguration &other);
  virtual ~MicronToolConfiguration();

  igstkSetMacro( MarkerName, std::string );
  igstkGetMacro( MarkerName, std::string );

  igstkSetMacro( CalibrationFileName, std::string );
  igstkGetMacro( CalibrationFileName, std::string );

  virtual std::string GetToolTypeAsString();

protected:

  std::string m_MarkerName;
  std::string m_CalibrationFileName;
};


/**
 * Configuration for the Micron tracker.
 */
class MicronTrackerConfiguration : public TrackerConfiguration
{
public:
  MicronTrackerConfiguration();
  virtual ~MicronTrackerConfiguration();

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

  igstkSetMacro( CameraCalibrationFileDirectory, std::string );
  igstkGetMacro( CameraCalibrationFileDirectory, std::string );

  igstkSetMacro( InitializationFile, std::string );
  igstkGetMacro( InitializationFile, std::string );

  igstkSetMacro( TemplatesDirectory, std::string );
  igstkGetMacro( TemplatesDirectory, std::string );

protected:
  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );

private:
  std::string     m_CameraCalibrationFileDirectory;
  std::string     m_InitializationFile;
  std::string     m_TemplatesDirectory;  

                  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


} // end of name space
#endif
