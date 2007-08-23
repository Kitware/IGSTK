/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerTool.h"
#include "igstkEvents.h"


namespace igstk
{

TrackerTool::TrackerTool(void):m_StateMachine(this)
{
  // Initialize the variables
  m_ValidityPeriod = 0.0; // current values are alreay invalid // FIXME DEPRECATED: This is redundat. Transform manage their own timestamps.
  m_Transform.SetToIdentity( m_ValidityPeriod ); 
  m_RawTransform.SetToIdentity( m_ValidityPeriod );  // FIXME: DEPRECATED: This is now the Transform to parent between the m_CoordinateReferenceSystem of this tool and the m_CoordinateReferenceSystem of the Tracker.
  m_ToolCalibrationTransform.SetToIdentity( 1e300 );  // FIXME : DEPRECATED: This is now the Transfrom To Parent of the spatial object attached to this tool.
  m_ToolType = UnknownTool; // FIXME : DEPRECATED : This is not being used.
  m_Updated = false; // not yet updated

  m_CoordinateReferenceSystem = CoordinateReferenceSystemType::New();


  // FIXME : Bogus State Machine
  //
  // The real state machine should require the TrackerTool to be attached to a Spatial Object before accepting RequestSetTransform calls.
  //
  igstkAddInputMacro( Initialize );

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( Invalid );
  igstkAddStateMacro( NotAvailable );
  igstkAddStateMacro( Available );
  igstkAddStateMacro( Initialized );
  igstkAddStateMacro( Tracking );
  igstkAddStateMacro( Visible );

  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();


}

TrackerTool::~TrackerTool(void)
{
}


/** This method should only be available to the Tracker */
void 
TrackerTool::RequestSetTransform( const TransformType & transform )
{
  m_Transform = transform;

  TransformModifiedEvent event;
  event.Set( transform );
  this->InvokeEvent( event );
}

/** Attach and spatial object to be tracked.
 *  FIXME: Add a state machine to this class and pass first this 
 *         request through the transition matrix. Then invoke a
 *         AttachSpatialObjectProcessing() method.
 */
void
TrackerTool::RequestAttachSpatialObject( SpatialObject * spatialObject )
{
  // Note: the ToolCalibrationTransform should be the transform relating
  //       the spatial object to the tracker tool. FIXME.
  spatialObject->RequestAttachToTrackerTool( this->m_CoordinateReferenceSystem );
}

/** Print object information */
void TrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Transform: " << this->m_Transform << std::endl;
  os << indent << "RawTransform: " << this->m_RawTransform << std::endl;
  os << indent << "ToolCalibrationTransform: "
               << this->m_ToolCalibrationTransform << std::endl;
  os << indent << "ValidityPeriod: " << this->m_ValidityPeriod << std::endl;
}


std::ostream& operator<<(std::ostream& os, const TrackerTool& o)
{
  o.Print(os, 0);
  return os;  
}


}
