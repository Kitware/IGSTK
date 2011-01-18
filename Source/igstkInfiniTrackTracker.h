/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkInfiniTrackTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkInfiniTrackTracker_h
#define __igstkInfiniTrackTracker_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "igstkTracker.h"
#include "igstkInfiniTrackTrackerTool.h"

struct itkMarkerPos;

#include "vector"

namespace igstk {
        
/** \class InfiniTrackTracker
 *  \brief Provides support for the InfiniTrack optical tracker.
 *
 *  The InfiniTrack tracker is a firewire-based optical tracker form Atracsys 
 *  sàrl in Bottens, Switzerland. This class uses the infiniTrack library that 
 *  comes with the Tracker to communicate with the tracker camera and gather 
 *  pose information of surgical tools.
 * 
 *  Notice that the infiniTrack library will search in the application directory
 *  for the calibration file that match the serial number of attached camera. 
 *  The calibration file is identified by 0xXXXXXXXX.xml.
 *
 *  \sa http://www.atracsys.com/
 *
 *  \ingroup Tracker
 */
  

class InfiniTrackTracker : public Tracker
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro(InfiniTrackTracker, Tracker)

public:

  /** Set marker templates directory */
  igstkSetStringMacro (MarkerTemplatesDirectory);

protected:

  /** Constructor */
  InfiniTrackTracker (void);

  /** Destructor */
  virtual ~InfiniTrackTracker(void);

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
      InfiniTrackTracker, location of the directory containing marker template 
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

  /** The infiniTrack handle */
  void* m_Handle;

  /** The serial number */
  unsigned long long m_u64DeviceSerialNumber;

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Buffers to hold the marker positions */
  std::vector<itkMarkerPos*>* m_pvecMarkerPos;

  /** Object array currently beeing processed by the user */
  int m_iProcessed;

  /** Currently available object array */
  int m_iAvailable;

  /** Object array currently in acquisition */
  int m_iInAcquisition;

  /** Set the next available object array for acquisition */
  void setNextArrayForAcquisition ();

  /** Set the next array for the user. True if a new array is available */
  bool setNextArrayForUser ();

  /** Called when the acquisition is finished */
  void acquisitionFinished ();

  /** The marker templates directory */
  std::string m_MarkerTemplatesDirectory;
  std::string m_MarkerTemplatesDirectoryToBeSet;


  /** The tracker tool <-> marker identification */
  struct TrackerToolIdentification 
    {std::string m_TrackerToolName; unsigned long m_u32GeometryID;}; 

  std::vector <TrackerToolIdentification> m_vecTrackerToolID;


}; // end of class InfiniTrackTracker

} // end of namespace igstk

#endif //__igstk_InfiniTrackTracker_h_
