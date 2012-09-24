/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkArucoTrackerTool.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkArucoTrackerTool_h
#define __igstkArucoTrackerTool_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkTrackerTool.h"

namespace igstk
{

class igstkArucoTracker;

/** \class ArucoTrackerTool
* \brief An ArucoTracker specific Tool class.
*
* This class is a for providing ArucoTracker functionality
* for TrackerTools, and also to allow compile-time type enforcement
* for other classes and functions that specifically require
* an ArucoTrackerTool.
*
* \ingroup Tracker
*
*/

class ArucoTrackerTool : public TrackerTool
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro ( ArucoTrackerTool, TrackerTool )

  /** Get Tracker tool marker name */
  unsigned int GetMarkerID() { return m_MarkerID; }

  /** Set tracker tool marker ID (0-1023) */
  void RequestSetMarkerName ( const unsigned int markerID );

protected:

  /** Constructor (configures Aruco-specific tool values)  */
  ArucoTrackerTool();

  /** Destructor */
  virtual ~ArucoTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  ArucoTrackerTool( const Self& );   //purposely not implemented
  void operator=( const Self& );       //purposely not implemented

  /** Get boolean variable to check if the tracker tool is
   *  configured or not.
   */
  virtual bool CheckIfTrackerToolIsConfigured () const;

  /** Report invalid marker specified */
  void ReportInvalidMarkerNameSpecifiedProcessing ();

  /** Report any invalid request to the tool */
  void ReportInvalidRequestProcessing ();

  /** Set the marker processing */
  void SetMarkerNameProcessing ();

  /** States for the State Machine */
  igstkDeclareStateMacro ( Idle );
  igstkDeclareStateMacro ( MarkerNameSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro ( ValidMarkerName );
  igstkDeclareInputMacro ( InValidMarkerName );

  /** The marker */
  unsigned int m_MarkerID;
  unsigned int m_MarkerIDToBeSet;

  /** boolean to indicate if the tracker tool is configured */
  bool m_TrackerToolConfigured;

};

} // namespace igstk


#endif  // __igstk_ArucoTrackerTool_h_
