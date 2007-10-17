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
#include "igstkEvents.h"
#include "igstkCoordinateReferenceSystem.h"

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

  /** DEPRECATED 
   * Attach as a child of another Spatial Object. Once this call has been
   * honored, the object is considered to be static in space, and any call to
   * RequestSetTransformToSpatialObjectParent() will only be honored the first
   * time it is invoked; calls to RequestSetCalibrationTransformToTrackerTool()
   * will be ignored as well. */
  void RequestSetTransform( const Transform & transform, const Self * object )
  {
  // REMOVE ME
  }

  /** Attach to a tracker tool. If this call is successful, the object
   * will not honor any subsequent calls to
   * RequestSetTransformToSpatialObjectParent(), and will only respond
   * to RequestSetCalibrationTransformToTrackerTool(). If the operation
   * succeeds then the coordinate system of the Tracker tool will
   * become the parent of the current spatial object.  */
  void RequestAttachToTrackerTool( Self * trackerToolCoordinateReferenceSystem );

  /** This method implements the construction of a scene graph by defining the
   * parent of this object and the Transforms defining their relative position
   * and orientation */
  template < class TParent >
  void RequestSetTransformAndParent( const Transform & transformToParent, const TParent * parent )
    {
    if( parent != NULL )
       {
       CoordinateReferenceSystem & referenceSystem = 
       igstk::Friends::CoordinateReferenceSystemHelper::GetCoordinateReferenceSystem( parent );
       // m_CoordinateSystemParentToBeSet = parent;
       // Add here the state machine input invocation...
       // igstkPushInputMacro( TransformAndParent );
       // m_StateMachine.ProcessInputs();
       //
       //
       //  WE need to add a private method called SetTransformAndParentProcessing() with a body like
       //  {
       //  m_CoordinateReferenceSystem->RequesetSetParent( m_CoordinateSystemParentToBeSet );
       //  }
       }
    }

  /**
   * DEPRECATED :
   *  This method should be to be replaced with RequestSetTransformToSpatialObjectParent()
   *  In the meantime we just use delegation.
   */
  void RequestSetTransform( const Transform & transform )
    {
    // DEPRECATED this->RequestSetTransform( transform, Self::New() );
    }

  /**
   * DEPRECATED :
   *  This method should be to be replaced with RequestGetTransformToWorld()
   *  In the meantime we just use delegation.
   */
  void RequestGetTransform()
    {
    this->RequestGetTransformToWorld();
    }

  /**
   * DEPRECATED:
   * Return a child object given the identifier */
  const Self * GetObject(unsigned long childId) const
    {
    return NULL; // It used to return the child numbered "childId".
    }

  /**
   * DEPRECATED :
   *  This method is deprecated, the interaction is now to Attache the child
   *  to the parent, by calling RequestAttachToSpatialObjectParent();
   *  In the meantime we just use delegation.
   */
  void RequestAddObject( Self * object );
    
  /** Request the Transform associated to the ObjectToWorld transformation of
   * the SpatialObject. This call, if acknowledged, will compute the transform
   * between this spatial object coordinate frame and the world coordinate
   * system. */
  void RequestGetTransformToWorld();

protected:

  SpatialObject( void );
  ~SpatialObject( void );

  /** DEPRECATED: Connect this representation class to the spatial object */
  void RequestSetSpatialObject( SpatialObjectType * object )
    {
    this->RequestSetInternalSpatialObject( object );
    }
  /** Replacement for RequestSetSpatialObject(). Internal is added to the
   *  name to clarify that this is used with the ITK spatial object as argument */
  void RequestSetInternalSpatialObject( SpatialObjectType * object );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** DEPRECATED: use now GetInternalSpatialObject(). The word "Internal" was
   *  added to differentiate between the ITK spatial object and the IGSTK 
   *  spatial objects that wraps around it. */
  SpatialObjectType * GetSpatialObject()
    {
    return this->GetInternalSpatialObject();
    }

  /** Returns the ITK spatial object that is contained inside this IGSTK 
   *  spatial object */
  SpatialObjectType * GetInternalSpatialObject();

private:
  
  /** Internal itkSpatialObject */
  SpatialObjectType::Pointer   m_SpatialObject;
  SpatialObjectType::Pointer   m_SpatialObjectToBeSet;

  /** Parent Spatial Object */
  Self::Pointer                m_Parent;
  Self::Pointer                m_ParentToBeSet;

  /** Internal Transform and temporary transform. 
   *  This transform can represent the relationship to the parent
   *  spatial object or to the tracker tool to which this spatial
   *  object may be attached. */
  Transform                    m_TransformToSpatialObjectParent;
  Transform                    m_TransformToSpatialObjectParentToBeSet;

  /** This transform will be recomputed every time that a
   * RequestGetTransformToWorld() is called. The purpose of this
   * variable is to cache the memory allocation so that the Transform
   * doesn't have to be constructed every time. */
  mutable Transform            m_TransformToWorld;

  /** Declaring frienship with helper that will facilitate enforcing the
   * privacy of the CoordinateReferenceSystem. */
  igstkFriendClassMacro( igstk::Friends::CoordinateReferenceSystemHelper );

  /** Private method for getting the CoordinateReferenceSystem. This method is
   * mainly intended to be called from the CoordinateReferenceSystemHelper as a
   * secure way of passing the CoordinateReferenceSystem without breaking its
   * encapsulation. */
  const CoordinateReferenceSystem * GetCoordinateReferenceSystem() const
    {
    return m_CoordinateReferenceSystem; // FIXME Move to the .cxx
    }

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( InternalSpatialObjectNull );
  igstkDeclareInputMacro( InternalSpatialObjectValid );
  igstkDeclareInputMacro( TransformAndParent );
  igstkDeclareInputMacro( AttachmentToParentSuccess ); // To be done through TransductionMacro
  igstkDeclareInputMacro( GetTransformToWorld );

  igstkDeclareInputMacro( SpatialObjectParentNull );  // deprecated
  igstkDeclareInputMacro( SpatialObjectParentValid );  // deprecated;
  igstkDeclareInputMacro( TrackerToolNull );  // deprecated
  igstkDeclareInputMacro( TrackerToolValid );  // deprecated

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( InternalSpatialObjectValidSet );
  igstkDeclareStateMacro( AttachedToParent );
  igstkDeclareStateMacro( AttemptingToAttachToParent ); // new

  igstkDeclareStateMacro( AttachedToTrackerTool ); // deprecated


  /** Action methods to be invoked only by the state machine */
  void AttachToTrackerToolProcessing();
  void SetTransformToParentProcessing();
  void BroadcastTransformToWorldProcessing();
  void BroadcastInvalidTransformMessageProcessing();
  void SetInternalSpatialObjectProcessing();
  void SetTransformToSpatialObjectParentProcessing();

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Report when a request has been made at an incorrect time. */
  void ReportInvalidRequestProcessing();

  /** Computes the transform from this object to the World Coordinate System.
   *  This is a service method and should never be exposed to the API of this
   *  class.
   **/
  virtual const Transform & ComputeTransformToWorld() const;

  /** Node of the Scene graph to which this object is attached */
  CoordinateReferenceSystem::Pointer    m_CoordinateReferenceSystem;

};

//
// Macros declaring events to be used when requesting SpatialObjects.
//
igstkLoadedObjectEventMacro( SpatialObjectModifiedEvent, 
  IGSTKEvent, SpatialObject );

igstkEventMacro( SpatialObjectNotAvailableEvent, InvalidRequestErrorEvent );

} // end namespace igstk

#endif // __igstkSpatialObject_h
