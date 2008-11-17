/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronTracker_h
#define __igstkMicronTracker_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkTracker.h"

//
// MicronTracker utilitiy classes declarations.
// These classes are part of the MTC library
// and can be found in the Utilities subdirectory.
//
class Markers;
class Marker;
class Persistence;
class Cameras;
class MCamera;
class Facet;
class Xform3D;

#include <map>

namespace igstk {

/** \class MicronTracker
 * \brief Provides support for the Claron MicronTracker.
 *
 * The MicronTracker is a small firewire-based optical tracker from Claron
 * Technologies in Toronto, Canada. This class uses the MTC library that comes
 * with the Tracker to communicate with the tracker camera and gather pose
 * information of surgical tools.
 *
 * \sa http://www.clarontech.com/
 * 
 * \ingroup Tracker
 */

class MicronTracker : public Tracker
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTracker, Tracker )

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

  /** Set the directory path that contains the camera calibration
   * files.
   */
  igstkSetStringMacro( CameraCalibrationFilesDirectory );

  /** Set the full path to the persistance file.
   *  The persistance file contains camera and marker 
   *  detection algorithm parameters
   */
  igstkSetStringMacro( InitializationFile );

  /** Set marker templates directory */
  igstkSetStringMacro( MarkerTemplatesDirectory );

protected:

  MicronTracker(void);

  virtual ~MicronTracker(void);

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

  /** The "ValidateSpecifiedFrequency" method checks if the specified  
   *  frequency is valid for the tracking device that is being used. */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Create an associative container that maps error code to error
    * descritpion */
  static void CreateErrorCodeList();

  /** Get Error description given the error code */
  static std::string GetErrorDescription( unsigned int );

  /** Remove tracker tool entry from internal containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers( const
                                     TrackerToolType * trackerTool );

  /** Add tracker tool entry from internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers( const
                                     TrackerToolType * trackerTool );

private:

  MicronTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Initialize camera and algorithm attributes such as Frame interleave
      template matching tolerance, extrapolate frame etc */
  bool Initialize();

  /** load marker templates */
  bool LoadMarkerTemplates();

  /** Setup cameras */
  bool SetUpCameras();

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** Calibration files directory */
  std::string m_CameraCalibrationFilesDirectory;

  /** Initialization file directory */
  std::string m_InitializationFile;

  /** marker template directory */
  std::string m_MarkerTemplatesDirectory;

  Persistence * m_Persistence;
  Markers     * m_Markers;
  Cameras     * m_Cameras;
  MCamera     * m_SelectedCamera;

  /** Camera light coolness value */
  double        m_CameraLightCoolness;

  /** A buffer to hold tool transforms */
  typedef std::map< std::string, std::vector < double > >
                                TrackerToolTransformContainerType;

  TrackerToolTransformContainerType     m_ToolTransformBuffer;

  /** Error map container */
  typedef std::map< unsigned int, std::string>  ErrorCodeContainerType;
  static ErrorCodeContainerType   m_ErrorCodeContainer;

  /** boolean to indicate if error code list is created */
  static bool m_ErrorCodeListCreated;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

};

}

#endif //__igstk_MicronTracker_h_
