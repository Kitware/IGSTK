/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronTrackerTool_h
#define __igstkMicronTrackerTool_h

#include "igstkTrackerTool.h"


namespace igstk
{

class MicronTracker;

/** \class MicronTrackerTool
  * \brief A MicronTracker-specific TrackerTool class.
  *
  * This class is a for providing MicronTracker-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an MicronTrackertool.
  *
  * \ingroup Tracker
  *
  */

class MicronTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MicronTrackerTool, TrackerTool )

  /** Get Tracker tool marker name */
  igstkGetStringMacro( MarkerName );

  /** Set tracker tool marker name */
  void RequestSetMarkerName( const std::string & markerName); 

protected:

  MicronTrackerTool();
  virtual ~MicronTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  MicronTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( MarkerNameSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidMarkerName );
  igstkDeclareInputMacro( InValidMarkerName );

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured() const;

  /** Report Invalid marker name specified*/ 
  void ReportInvalidMarkerNameSpecifiedProcessing( );

  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Set marker name */
  void SetMarkerNameProcessing();

  std::string     m_MarkerName;
  std::string     m_MarkerNameToBeSet;

  bool            m_TrackerToolConfigured;

};  

} // namespace igstk


#endif  // __igstk_MicronTrackerTool_h_
