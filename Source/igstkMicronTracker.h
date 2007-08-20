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

#include "igstkMicronTrackerCommandInterpreter.h"
#include "igstkMicronTrackerTool.h"
#include "igstkMicronTrackerAttributes.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class MicronTracker
  * \brief Provides support for the Claron MicronTracker.
  *
  * The MicronTracker is a small firewire-based optical tracker
  * from Claron Technologies in Toronto.
*/


class MicronTracker : public Tracker
{
public:

  /** typedefs for the tool */
  typedef igstk::MicronTrackerTool              MicronTrackerToolType;
  typedef MicronTrackerToolType::Pointer        MicronTrackerToolPointer;
  typedef MicronTrackerToolType::ConstPointer   MicronTrackerToolConstPointer;

  /** Number of ports to allow for this tracker. */
  itkStaticConstMacro( NumberOfPorts, unsigned int, 10 );

  /** Number of channels to allow. */
  itkStaticConstMacro( NumberOfChannels, unsigned int, 1 );

  /** Typedef for the attributes of the tracker. */
  typedef igstk::MicronTrackerAttributes           AttributesType;

  /** Typedef for command interpreter. */
  typedef igstk::MicronTrackerCommandInterpreter   CommandInterpreterType;

  /** Typedef for internal boolean return type. */
  typedef Tracker::ResultType   ResultType;

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTracker, Tracker )

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  MicronTracker(void);

  virtual ~MicronTracker(void);

  /** The MicronTrackerAttributes class contains settings and status
   *  information for the MicronTracker.  Collecting all the
   *  settings in a single class, rather than having a separate
   *  request method for each setting, makes the state machine for the
   *  MicronTracker much simpler.  */
  virtual void RequestSetAttributes( const AttributesType *attribs );

  /** Get settings and status information for the MicronTracker via
   *  a MicronTrackerAttributes object. */
  virtual void RequestGetAttributes( AttributesType *attribs);

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

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** MicronTracker version information */
  std::string m_Version;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;
};

}

#endif //__igstk_MicronTracker_h_
