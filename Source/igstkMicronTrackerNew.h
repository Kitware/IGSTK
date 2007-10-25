/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronTrackerNew_h
#define __igstkMicronTrackerNew_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkMicronTrackerToolNew.h" 
#include "igstkTrackerNew.h"

#include "Markers.h"
#include "Marker.h"
#include "Persistence.h"
#include "Cameras.h"
#include "Facet.h"
#include "Xform3D.h"
#include "MTC.h"
#include <math.h>

#include <map>

namespace igstk {

/** \class MicronTrackerNew
  * \brief Provides support for the Claron MicronTracker.
  *
  * The MicronTracker is a small firewire-based optical tracker
  * from Claron Technologies in Toronto.
*/

class MicronTrackerNew : public TrackerNew
{
public:

  /** typedefs for the tool */
  /** The should be converted to the new Tool class */
  typedef igstk::MicronTrackerToolNew           MicronTrackerToolType;
  typedef MicronTrackerToolType::Pointer        MicronTrackerToolPointer;
  typedef MicronTrackerToolType::ConstPointer   MicronTrackerToolConstPointer;

  /** Typedef for internal boolean return type. */
  typedef TrackerNew::ResultType   ResultType;

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTrackerNew, TrackerNew )

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

  /** Set the directory path that contains the camera calibration
    * files. FIXME: this should be pushed through the state machine
  */
  void SetCameraCalibrationFilesDirectory( std::string fileName );

  /** Set the full path to the persistance file
    * The persistance file contains camera parameters and algorithm parameters 
     FIXME: this should be pushed through the state machine
  */
  void SetInitializationFile( std::string fileName );

  /** Load markers template */
  void LoadMarkerTemplate( std::string filename );

  void RequestAddTool( std::string identifier, MicronTrackerToolType * trackerTool );
protected:

  MicronTrackerNew(void);

  virtual ~MicronTrackerNew(void);

  /** Open communication with the tracking device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the tracking device. */
  virtual ResultType InternalClose( void );

  /** Activate the tools attached to the tracking device. */
  virtual ResultType InternalActivateTools( void );

  /** Deactivate the tools attached to the tracking device. */
  virtual ResultType InternalDeactivateTools( void );

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

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Initialize camera and algorithm attributes such as Frame interleave
      template matching tolerance, extrapolate frame etc */ 
  bool Initialize();

  /** Setup cameras */
  bool SetUpCameras();

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** Calibration files directory */
  std::string m_CalibrationFilesDirectory;

  /** Initialization file directory */
  std::string m_InitializationFile;
  
  /** marker template directory */
  std::string m_MarkerTemplateDirectory;

  Persistence * m_Persistence;
  Markers     * m_Markers;
  Cameras     * m_Cameras;
  MCamera     * m_SelectedCamera;

  /** A buffer to hold tool transforms */
  typedef std::map< std::string, std::vector < double > > 
                                TrackerToolTransformContainerType; 

 TrackerToolTransformContainerType     m_ToolTransformBuffer;
};

}

#endif //__igstk_MicronTrackerNew_h_
