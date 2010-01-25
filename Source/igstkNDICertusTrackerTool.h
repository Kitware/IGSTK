/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICertusTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkNDICertusTrackerTool_h
#define __igstkNDICertusTrackerTool_h

#include "igstkTrackerTool.h"


namespace igstk
{

class NDICertusTracker;

/** \class NDICertusTrackerTool
  * \brief A NDICertusTracker-specific TrackerTool class.
  *
  * This class is a for providing NDICertusTracker-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an NDICertusTrackertool.
  *
  * \ingroup Tracker
  *
  */

class NDICertusTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( NDICertusTrackerTool, TrackerTool )

  /** Get Tracker tool RigidBody name */
  igstkGetStringMacro( RigidBodyName );

  /** Set tracker tool RigidBody name */
  void RequestSetRigidBodyName( const std::string & rigidBodyName); 

  /** The "RequestAttachToTracker" method attaches 
   * the tracker tool to a tracker. */
  virtual void RequestAttachToTracker( NDICertusTracker * );

protected:

  NDICertusTrackerTool();
  virtual ~NDICertusTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  NDICertusTrackerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( RigidBodyNameSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidRigidBodyName );
  igstkDeclareInputMacro( InValidRigidBodyName );

  /** Get boolean variable to check if the tracker tool is 
   * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured() const;

  /** Report Invalid marker name specified*/ 
  void ReportInvalidRigidBodyNameSpecifiedProcessing( );

  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Set RigidBodyName */
  void SetRigidBodyNameProcessing();

  std::string     m_RigidBodyName;
  std::string     m_RigidBodyNameToBeSet;

  bool            m_TrackerToolConfigured;

};  

} // namespace igstk


#endif  // __igstk_NDICertusTrackerTool_h_
