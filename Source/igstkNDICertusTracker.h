/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICertusTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkNDICertusTracker_h
#define __igstkNDICertusTracker_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkTracker.h"
#include "igstkNDICertusTrackerTool.h"

#include <math.h>

#include <map>

/////// Headers containing constants and structs for the NDI API ///////////
//
  #include "ndtypes.h"
  #include "ndpack.h"
  #include "ndopto.h"
  #include "certus_aux.h"
//
////////////////////////////////////////////////////////////////////////////

/*
 * Type definition to retreive and access rigid body transformation
 * data.
 */
typedef struct RigidBodyDataStruct
{
    struct OptotrakRigidStruct  pRigidData[ MAX_RIGID_BODIES];
    Position3d                  p3dData[ DEVICE_MAX_MARKERS];
}RigidBodyDataType;



namespace igstk {

/** \class NDICertusTracker
 * \brief Provides support for the Northern Digital Certus Tracker.
 *
 * The Certus Tracker is the acitve-based otpoeletronic system
 *  by Northern Digital. It adopts wired markers and rigid bodies
 *
 * \sa http://www.ndigital.com/
 *
 * \ingroup Tracker
 */

class NDICertusTracker : public Tracker
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( NDICertusTracker, Tracker )

public:

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

  /** Change reference frame for rigid body pose calculations **/
  /*
  OPTOTRAK_STATIC_RIGID_FLAG indicates that the Optotrak System is to use
    the current position and orientation of the specified rigid body to define
    the static coordinate system.
  OPTOTRAK_CONSTANT_RIGID_FLAG indicates that the Optotrak System is to define
    the coordinate system based on the specified rigid body's transformation
    each time the rigid body transformations are determined. This is equivalent 
    to measuring one moving rigid body with respect to another moving rigid body.
  */
  int SetCoordinateFrame(int rigidBodyID, int flag);


  /** Set ini file name*/
  void SetIniFileName(std::string strIniFileName);
  
  /** Set cgf rigid body file name */
  void SetCfgFileName(std::string strCfgFileName);
 
   
  /** Status structure */
  OptotrakStatus          Status;

  /** RigidBody structures */
  struct  OptotrakRigidStatusStruct    rigidBodyStatus;
  struct  OptotrakRigidDescrStruct    rigidBodyDescrArray[MAX_RIGID_BODIES];
  
  RigidBodyDataType               RigidBodyData;

  unsigned int                    m_UFlags, m_UElements, m_UFrameNumber;
  int                             m_NFlags;
protected:

  NDICertusTracker(void);

  virtual ~NDICertusTracker(void);

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

  /** Create an associative container that maps error code to error
    * descritpion */
  static void CreateErrorCodeList();

  /** Get Error description  */
  static std::string GetErrorDescription(  );

  /** Remove tracker tool entry from internal containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers( const
                                     TrackerToolType * trackerTool );

  /** Add tracker tool entry from internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers( const
                                     TrackerToolType * trackerTool );

private:
  
  NDICertusTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Name of ini files */
  std::string m_strIniFileName;

  /** Reset the system variables */
  bool ResetSystemVariables();

  /** Load .rig files */
  bool LoadRigidBodies(); //CONTROLLA

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;    //CONTROLLA

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

#endif //__igstk_NDICertusTracker_h_
