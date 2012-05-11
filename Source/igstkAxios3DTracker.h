/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAxios3DTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAxios3DTracker_h
#define __igstkAxios3DTracker_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkTracker.h"
#include "igstkAxios3DTrackerTool.h"
#include "MetrologySystems.h"
#include "MetroUtils.h"
#include "Metrology.h"
#include "MetrologyLibraryLoader.h"

struct itkMarkerPos;

#include "vector"

/** \class MetroWarningReceiver
 *  \brief to know which error occured */
 class MetroWarningReceiver : public metro_lib::MetroWarningReceiverItf
{
public:

  void Receive(const metro_lib::MetroError & e)
    {
    std::stringstream strm;
    unsigned int size = e.Size();
    strm << "Warnings (" << size<< "):\n";
    for (unsigned u = 0; u < size; ++u)
    {
    strm << e.GetCode(u) << " ";
    strm << e.GetSeparatedInfoString(u) << "\n";
    }
    text += strm.str();
  }

  std::string GetText()
  {
    std::string s_copy;
    std::swap(text, s_copy);
    return s_copy;
  }
private:
  std::string text;
};

namespace igstk {

/** \class Axios3DTracker
 *  \brief Provides support for the Axios3DTrack optical tracker.
 *
 *  The Axios3DTrack tracker is a firewire-based optical tracker form Atracsys
 *  sàrl in Bottens, Switzerland. This class uses the infiniTrack library that
 *  comes with the Tracker to communicate with the tracker camera and gather
 *  pose information of surgical tools.
 *
 *  Notice that the Axios3DTrack library will search in the application
 *  directory for the calibration file that match the serial number of attached
 *  camera.
 *  The calibration file is identified by 0xXXXXXXXX.xml.
 *
 *  \sa http://www.atracsys.com/
 *
 *  \ingroup Tracker
 */

class Axios3DTracker : public Tracker
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro(Axios3DTracker, Tracker)

  /** Set marker templates directory */
  igstkSetStringMacro (MarkerTemplatesDirectory);
  //std::string RequestLoadLocatorsXML(const std::string & file);
  virtual ResultType RequestLoadLocatorsXML(const std::string & file);

protected:

  /** Constructor */
  Axios3DTracker (void);

  /** Destructor */
  virtual ~Axios3DTracker(void);

  /** The "InternalOpen" method opens communication with a tracking device.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalOpen( void );

  /** The "InternalClose" method closes communication with a tracking device.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalClose( void );

  /** The "InternalReset" method resets tracker to a known configuration.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalReset( void );

  /** The "InternalStartTracking" method starts tracking.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalStartTracking( void );

  /** The "InternalStopTracking" method stops tracking.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalStopTracking( void );

  /** The "InternalUpdateStatus" method updates tracker status.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void );

  /** The "InternalThreadedUpdateStatus" method updates tracker status.
      This method is called in a separate thread.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Verify if a tracker tool information is correct before attaching
      it to the tracker. This method is used to verify the information supplied
      by the user about the tracker tool. The information depends on the
      tracker type. For example, during the configuration step of the
      Axios3DTracker, location of the directory containing marker template
      files is specified. If the user tries to attach a tracker tool with a
      marker type whose template file is not stored in this directory, this
      method will return failure. Similarly, for PolarisTracker, the method
      returns failure,  if the tool part number specified by the user during
      the tracker tool configuration step does not match with the part number
      read from the SROM file.
   */
  virtual ResultType VerifyTrackerToolInformation ( const TrackerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified
      frequency is valid for the tracking device that is being used. This
      method is to be overridden in the derived tracking-device specific
      classes to take into account the maximum frequency possible in the
      tracking device
   */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );


  /** This method will remove entries of the traceker tool from internal
      data containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers(
            const TrackerToolType * trackerTool );

  /** Add tracker tool entry to internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers(
            const TrackerToolType * trackerTool );

private:

  /** CreateObject */
  virtual ResultType Axios3DTracker::CreateObject();

  /** Interface to Measuring System */
  metro_lib::MetrologySystems * m_MetrologySystems;
  metro_lib::Metrology * m_Metrology;

  /** for Warning_handling */
  MetroWarningReceiver m_Mwr;

  /** Start/Stop Tracking */
  bool m_Tracking;

  /** locks the measurement data access */
  virtual ResultType Lock();

  /** locator administration */
  std::list<std::string> GetLocatorNames();
  std::list<std::string> m_LoadedLocators;

  /** measurement functions */
  virtual ResultType MeasurePoints(bool extended_protocol = true);
  virtual ResultType MeasureLocator(
                  const std::string & loc_id, bool extended_protocol = true);

  /** locator administration */
  std::string WriteLocators(const std::string & file);
  virtual ResultType DeleteLocators();
  std::string GetLoadedLocators();

  /** Initialises a virtual measurement */
  const char * m_Path;
  unsigned m_Config;

  /** The Axios3DTrack handle */
  void* m_Handle;

  /** The serial number */
  unsigned long long m_U64DeviceSerialNumber;

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Buffers to hold the marker positions */
  std::vector<itkMarkerPos*>* m_pvecMarkerPos;

  /** The marker templates directory */
  std::string m_MarkerTemplatesDirectory;
  std::string m_MarkerTemplatesDirectoryToBeSet;

  /** To set the measuring system to Virtual */
  bool m_VirtualMode;
public:
  void setVirtualMode(bool mode)
    {
    m_VirtualMode = mode;
    }

  bool getVirtualMode()
    {
    return m_VirtualMode;
    }

  /** To set m_Path for pictures in virtual_mode */
public:
  void setPath(char *p)
    {
    m_Path = p;
    }
  const char getPath()
    {
    return *m_Path;
    }

  /** \class LocatorResult
   *  \brief to check if Locator isVisible */
  class LocatorResult
    {
  public:
    LocatorResult(){}

    LocatorResult(TransformType t,bool v)
      {
      m_Transform = t;
      m_IsVisible = v;
      }
    TransformType m_Transform;
    bool m_IsVisible;
    };

  /** Container holding LocatorResult of the tools */
  std::map< std::string, LocatorResult> m_LocatorResultsContainer;

}; // end of class Axios3DTracker

} // end of namespace igstk

#endif //__igstk_Axios3DTracker_h_
