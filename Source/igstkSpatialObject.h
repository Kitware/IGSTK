/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSpatialObject_h
#define __igstkSpatialObject_h

#include "itkLogger.h"
#include "itkSpatialObject.h"

#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkTransform.h"
#include "igstkStateMachine.h"
#include "igstkTrackerTool.h"
#include "igstkEvents.h"

namespace igstk
{

/** \class SpatialObject
 * 
 * \brief This class encapsulates an ITK spatial object with the goal of
 * restricting access to functionalities that are not essential for IGS
 * applications, or to functionalities thay may present risks and unnecessary
 * flexibility.  This is an abstract class, you should use the derived classes
 * that represent specific shapes.
 *
 *
 *  \image html  igstkSpatialObject.png  "SpatialObject State Machine Diagram"
 *  \image latex igstkSpatialObject.eps  "SpatialObject State Machine Diagram" 
 *
 * \ingroup Object
 */

class SpatialObject : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( SpatialObject, Object )

public:

  /** Typedefs */
  typedef itk::SpatialObject<3>          SpatialObjectType;

  /** Set the Transform corresponding to the ObjectToWorld transformation of
   * the SpatialObject. */
  void RequestSetTransform( const Transform & transform );

  /** Set a new object. */
  void RequestAddObject(Self * object);

  /** Request the Transform associated to the ObjectToWorld transformation
   * of the SpatialObject */
  void RequestGetTransform();

  /** Return a child object given the id */
  const Self * GetObject(unsigned long id) const;

  /** Request the protocol for attaching to a tracker tool. This is a one-time
   * operation. Once a Spatial Object is attached to a tracker tool it is not
   * expected to get back to manual nor to be re-attached to a second tracker
   * tool. */
  void RequestAttachToTrackerTool( const TrackerTool * trackerTool );

protected:

  SpatialObject( void );
  ~SpatialObject( void );

  /** Connect this representation class to the spatial object */
  void RequestSetSpatialObject( SpatialObjectType * );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  SpatialObjectType * GetSpatialObject();

private:
  
  /** Internal itkSpatialObject */
  SpatialObjectType::Pointer   m_SpatialObject;
  SpatialObjectType::Pointer   m_SpatialObjectToBeSet;

  /** Parent Spatial Object */
  Self::Pointer                m_Parent;

  /** Internal list of object */
  std::vector<Pointer> m_InternalObjectList;

  /** Internal Object */
  Pointer m_ObjectToBeAdded;

  /** Internal Transform and temporary transform */
  Transform                    m_Transform;
  Transform                    m_TransformToBeSet;

  /** TrackerTool to be attached to, and temporary pointer */
  TrackerTool::ConstPointer    m_TrackerTool;
  TrackerTool::ConstPointer    m_TrackerToolToAttachTo;

  /** Set the Transform corresponding to the ObjectToWorld transformation of
   * the SpatialObject. This method is only intended to be called from a 
   * callback that is observing events from a TrackerTool object. */
  void RequestSetTrackedTransform(const Transform & transform );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( SpatialObjectNull );
  igstkDeclareInputMacro( SpatialObjectValid );
  igstkDeclareInputMacro( ObjectNull );
  igstkDeclareInputMacro( ObjectValid );
  igstkDeclareInputMacro( TrackingEnabled );
  igstkDeclareInputMacro( TrackingLost );
  igstkDeclareInputMacro( TrackingRestored );
  igstkDeclareInputMacro( TrackingDisabled );
  igstkDeclareInputMacro( ManualTransform );
  igstkDeclareInputMacro( TrackerTransform );
  igstkDeclareInputMacro( RequestGetTransform );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( NonTracked );
  igstkDeclareStateMacro( Tracked );
  igstkDeclareStateMacro( TrackedLost );

  /** Action methods to be invoked only by the state machine */
  void SetSpatialObjectProcessing();
  void AddObjectProcessing();
  void ReportTrackingRestoredProcessing();
  void ReportTrackingDisabledProcessing();
  void ReportTrackingLostProcessing();
  void ReportInvalidRequestProcessing();
  void AttachToTrackerToolProcessing();
  void SetTransformProcessing();
  void BroadcastStaticTransformProcessing();
  void BroadcastTrackedTransformProcessing();
  void BroadcastExpiredTrackedTransformProcessing();

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Set/Get the parent Spatial Object, this method provides half of the
   * functionality of attaching this object to a scene graph. This method is
   * private, and can only be called from the AddObjectProcessing() method. */
  igstkSetMacro( Parent, Pointer );
  igstkGetMacro( Parent, Pointer );

};

} // end namespace igstk

#endif // __igstkSpatialObject_h
