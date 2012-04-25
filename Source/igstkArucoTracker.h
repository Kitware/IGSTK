/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkArucoTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkArucoTracker_h
#define __igstkArucoTracker_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#pragma warning( disable : 4996 )
#endif

#include "aruco.h"

#include "igstkTracker.h"
#include "igstkArucoTrackerTool.h"

#include "vector"

namespace igstk {

/** \class ArucoTracker
 *  \brief Provides support for the ArUco monocular video camera tracker.
 *
 *
 *  \ingroup Tracker
 */

class ArucoTracker : public Tracker
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro(ArucoTracker, Tracker)

  bool SetCameraParametersFromXMLFile(std::string file);

  igstkSetMacro(ShowVideoStream, bool);
  igstkGetMacro(ShowVideoStream, bool);

  igstkSetMacro(SimulationVideo, std::string);
  igstkGetMacro(SimulationVideo, std::string);

  void CaptureAndShowVideoFrame(unsigned int delay);

protected:

  /** Constructor */
  ArucoTracker (void);

  /** Destructor */
  virtual ~ArucoTracker(void);

  /** The "InternalOpen" method opens communication with a tracking device.*/
  virtual ResultType InternalOpen( void );

  /** The "InternalClose" method closes communication with a tracking device.*/
  virtual ResultType InternalClose( void );

  /** The "InternalReset" method resets tracker to a known configuration.*/
  virtual ResultType InternalReset( void );

  /** The "InternalStartTracking" method starts tracking.*/
  virtual ResultType InternalStartTracking( void );

  /** The "InternalStopTracking" method stops tracking.*/
  virtual ResultType InternalStopTracking( void );

  /** The "InternalUpdateStatus" method updates tracker status.*/
  virtual ResultType InternalUpdateStatus( void );

  /** The "InternalThreadedUpdateStatus" method updates tracker status.
      This method is called in a separate thread.*/
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Verify if a tracker tool information is correct before attaching
      it to the tracker. This method is used to verify the information supplied
      by the user about the tracker tool. The information depends on the
      tracker type. For example, during the configuration step of the
      ArucoTracker, location of the directory containing marker template
      files is specified. If the user tries to attach a tracker tool with a
      marker type whose template file is not stored in this directory, this
      method will return failure.
   */
  virtual ResultType VerifyTrackerToolInformation ( const TrackerToolType * );

  /** The "ValidateSpecifiedFrequency" method checks if the specified
      frequency is valid for the tracking device that is being used.*/
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );


  /** This method will remove entries of the traceker tool from internal
      data containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers(
            const TrackerToolType * trackerTool );

  /** Add tracker tool entry to internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers(
            const TrackerToolType * trackerTool );

private:

  cv::VideoCapture        m_VideoCapturer;
  cv::Mat                 m_InputImage;
  cv::Mat                 m_InputImageCopy;
  aruco::CameraParameters m_CameraParameters;
  vector<aruco::Marker>   m_Markers;
  aruco::MarkerDetector   m_MDetector;
  double                  m_ThreshParam1;
  double                  m_ThreshParam2;
  bool                    m_ShowVideoStream;
  std::string             m_SimulationVideo;

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

}; // end of class ArucoTracker

} // end of namespace igstk

#endif //__igstk_ArucoTracker_h_
