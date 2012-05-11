/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAtracsysEasyTrackTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-05-09 13:29:45 $
  Version:   $Revision: 1.0 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAtracsysEasyTrackTracker_h
#define __igstkAtracsysEasyTrackTracker_h

#include "igstkTracker.h"
#include "igstkAtracsysEasyTrackTrackerTool.h"

#include <apiTracking.hpp>

namespace igstk {

/** \class AtracsysEasyTrackTracker
 *
 * \brief Provides support for the easyTrack 500 tracker (Atracsys, Switzerland).
 *
 * This class provides an interface to the easyTrack 500 optical
 * tracking system.
 *
 * \ingroup Tracker
 *
 */

class AtracsysEasyTrackTracker : public Tracker
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AtracsysEasyTrackTracker, Tracker )

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  AtracsysEasyTrackTracker(void);

  virtual ~AtracsysEasyTrackTracker(void);

  /** Typedef for internal boolean return type. */
  typedef Tracker::ResultType   ResultType;

  /** Open communication with the tracking device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the tracking device. */
  virtual ResultType InternalClose( void );

  /** Put the tracking device into tracking mode. */
  virtual ResultType InternalStartTracking( void );

  /** Take the tracking device out of tracking mode. */
  virtual ResultType InternalStopTracking( void );

  /** Update the status and the transforms for all TrackerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the transforms.
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( const TrackerToolType * );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Get Error description  */
  static std::string GetErrorDescription(  );

  /** Remove tracker tool entry from internal containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers( const
                                     TrackerToolType * trackerTool );

  /** Add tracker tool entry from internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers( const
                                     TrackerToolType * trackerTool );

private:
  void ReportError();

  // struct holding PnP marker properties
  struct PnpMarker
    {
    unsigned  muModel;
    char      mstrName [256];
    unsigned  muStatus;
    };

  // frame counter
  static unsigned m_SuCounter;

  // current etk device
  static glbDevice* m_SpDevice;

  // current etk handle
  static glbHandle m_ShHandle;

  // PnP markers
  static PnpMarker* m_SpMarkers[4];

  // error code
  static bool           m_HasError;
  static unsigned int   m_ErrorCode;
  static char          *m_ErrorString;

  // ETK API callbacks

  // this just calls in-class ObjectMarkerCallback
  static void MarkerCallback( glbDevice *pDevice,
            unsigned uMarkerID,
                  double *adMatrix33,
                  double *adVector3,
                  double *pdError,
                  unsigned uExtended );

  void ObjectMarkerCallback( unsigned uMarkerID,
                 double *adMatrix33,
                 double *adVector3,
                 double *pdError );

  static void ErrorCallback( unsigned uGravity,
                             char *strDescription );

  static void PnpMarkerCallback( glbDevice *pDevice,
                                 unsigned uMarkerID,
                                 unsigned uModelID,
                                 char *strMarkerName,
                                 unsigned uStatus );

  AtracsysEasyTrackTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;    //CONTROLLA

  /** A buffer to hold tool transforms */
  typedef std::map< std::string, std::vector < double > >
                                TrackerToolTransformContainerType;

  TrackerToolTransformContainerType     m_ToolTransformBuffer;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

  static AtracsysEasyTrackTracker *m_TrackerObject;
  static bool               m_BTrackingEnabled;
  static bool               m_BDeviceOpened;
};

}

#endif //__igstk_AtracsysEasyTrackTracker_h_
