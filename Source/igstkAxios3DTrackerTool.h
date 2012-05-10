/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkAxios3DTracker.h,v $
  Language:  C++
  Date:      $Date: 2011-03-17 20:12:26 $
  Version:   $Revision: 1.00 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkAxios3DTrackerTool_h
#define __igstkAxios3DTrackerTool_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkTrackerTool.h"

namespace igstk
{

igstkEventMacro( Axios3DTrackerToolErrorEvent, TrackerToolErrorEvent);
igstkEventMacro( InvalidMarkerNameErrorEvent, Axios3DTrackerToolErrorEvent);

class igstkAxios3DTracker;

/** \class Axios3DTrackerTool
* \brief A Axios3DTrack-specific TrackerTool class.
*
* This class is a for providing Axios3DTrack-specific functionality
* for TrackerTools, and also to allow compile-time type enforcement
* for other classes and functions that specifically require
* an Axios3DTrackerTool.
*
* \sa http://www.atracsys.com/
*
* \ingroup Tracker
*
*/

class Axios3DTrackerTool : public TrackerTool
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro (Axios3DTrackerTool, TrackerTool )

  /** Get Tracker tool marker name */
  igstkGetStringMacro (MarkerName);

  /** Set tracker tool marker name */
  void RequestSetMarkerName (const std::string & markerName);

protected:

  /** Constructor (configures Axios3D-specific tool values)  */
  Axios3DTrackerTool();

  /** Destructor */
  virtual ~Axios3DTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  Axios3DTrackerTool(const Self&);   //purposely not implemented
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


#endif  // __igstk_Axios3DTrackerTool_h_
