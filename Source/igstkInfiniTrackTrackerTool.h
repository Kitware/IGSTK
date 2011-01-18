/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkInfiniTrackTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkInfiniTrackTrackerTool_h
#define __igstkInfiniTrackTrackerTool_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkTrackerTool.h"


namespace igstk
{

class igstkInfiniTrackTracker;

/** \class InfiniTrackTrackerTool
* \brief A InfiniTrack-specific TrackerTool class.
*
* This class is a for providing InfiniTrack-specific functionality
* for TrackerTools, and also to allow compile-time type enforcement
* for other classes and functions that specifically require
* an InfiniTrackTrackertool.
*
* \sa http://www.atracsys.com/
*
* \ingroup Tracker
*
*/

class InfiniTrackTrackerTool : public TrackerTool
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro (InfiniTrackTrackerTool, TrackerTool )

  /** Get Tracker tool marker name */
  igstkGetStringMacro (MarkerName);

  /** Set tracker tool marker name */
  void RequestSetMarkerName (const std::string & markerName); 

protected:

  /** Constructor (configures InfiniTrack-specific tool values)  */
  InfiniTrackTrackerTool();

  /** Destructor */
  virtual ~InfiniTrackTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  InfiniTrackTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

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
  igstkDeclareStateMacro (Idle);
  igstkDeclareStateMacro (MarkerNameSpecified);

  /** Inputs to the State Machine */
  igstkDeclareInputMacro (ValidMarkerName);
  igstkDeclareInputMacro (InValidMarkerName);

  /** The marker */
  std::string m_MarkerName;
  std::string m_MarkerNameToBeSet;

  /** boolean to indicate if the tracker tool is configured */
  bool m_TrackerToolConfigured;

};  

} // namespace igstk


#endif  // __igstk_InfiniTrackTrackerTool_h_
