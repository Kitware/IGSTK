/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FourViewsTrackingWithCTImplementation.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _FourViewsTrackingWithCTImplementation_h
#define _FourViewsTrackingWithCTImplementation_h

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "igstkSandboxConfigure.h"
#include "FourViewsTrackingWithCT.h"
#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkAuroraTracker.h"

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"


namespace igstk
{


class FourViewsTrackingWithCTImplementation : public FourViewsTrackingWithCT
{
public:
  
  /** typedefs for the logger */
  typedef itk::Logger              LoggerType; 
  
  /** typedefs for the log output */
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  /** typedefs for the tracker */
  typedef igstk::AuroraTracker     TrackerType;

  /** typedefs for the communication */
  #ifdef WIN32
    typedef igstk::SerialCommunicationForWindows  CommunicationType;
  #else
    typedef igstk::SerialCommunicationForPosix    CommunicationType;
  #endif

public:

  /** Enable the tracking */
  void EnableTracking( void );

  void DisableTracking( void );

  void AddCylinder( igstk::CylinderObjectRepresentation * cylinderRepresentation );

  void AddEllipsoid( igstk::EllipsoidObjectRepresentation * ellipsoidRepresentation );

  void AttachObjectToTrack( igstk::SpatialObject * objectToTrack );

  void LoadImage( void );

  FourViewsTrackingWithCTImplementation( void );
  virtual ~FourViewsTrackingWithCTImplementation( void );

private:

  LoggerType::Pointer     m_Logger;
  LogOutputType::Pointer  m_LogOutput;
  TrackerType::Pointer    m_Tracker;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;

  std::ofstream           m_LogFile;
};


} // end of namespace

#endif

