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

  /** Attach as a child of another Spatial Object. Once this call has been
   * honored, the object is considered to be static in space, and any call to
   * RequestSetTransformToSpatialObjectParent() will only be honored the first
   * time it is invoked; calls to RequestSetCalibrationTransformToTrackerTool()
   * will be ignored as well. */
  void RequestAttachToSpatialObjectParent( Self * object );

  /** Attach to a tracker tool. If this call is successful, the object
   * will not honor any subsequent calls to
   * RequestSetTransformToSpatialObjectParent(), and will only respond
   * to RequestSetCalibrationTransformToTrackerTool(). If the operation
   * succeeds then the coordinate system of the Tracker tool will
   * become the parent of the current spatial object.  */
  void RequestAttachToTrackerTool( Self * trackerToolCoordinateReferenceSystem );

  /** Set the Transform corresponding to the ObjectToParent transformation of
   * the Parent SpatialObject. This call is acknowledge only the first time
   * that it is invoked. Since only tracked object should move in the scene.
   * */
  void RequestSetTransformToSpatialObjectParent( const Transform & transform );

  /**
   * DEPRECATED :
   *  This method should be to be replaced with RequestSetTransformToSpatialObjectParent()
   *  In the meantime we just use delegation.
   */
  void RequestSetTransform( const Transform & transform )
    {
    this->RequestSetTransformToSpatialObjectParent( transform );
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
   * Return a child object given the id */
  const Self * GetObject(unsigned long id) const
    {
    return NULL; // It used to return the child numbered "id".
    }

  /**
   * DEPRECATED :
   *  This method is deprecated, the interaction is now to Attache the child
   *  to the parent, by calling RequestAttachToSpatialObjectParent();
   *  In the meantime we just use delegation.
   */
  void RequestAddObject( Self * object )
    {
    if( object )  // ALL THIS METHOD WILL GO AWAY... don't be too picky about the style at this point.
      {
      object->RequestAttachToSpatialObjectParent( this );
      }
    }



  /** Set the Transform describing the relationship between the coordinate
   * system of the current SpatialObject and the coordinate system of the
   * TrackerTool to which the object is attached. This call is only honored
   * if the SpatialObject is currently attached to a TrackerTool.
   * */
  void RequestSetCalibrationTransformToTrackerTool( const Transform & transform );

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

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( InternalSpatialObjectNull );
  igstkDeclareInputMacro( InternalSpatialObjectValid );
  igstkDeclareInputMacro( SpatialObjectParentNull );
  igstkDeclareInputMacro( SpatialObjectParentValid );
  igstkDeclareInputMacro( TrackerToolNull );
  igstkDeclareInputMacro( TrackerToolValid );
  igstkDeclareInputMacro( TransformToSpatialObjectParent );
  igstkDeclareInputMacro( CalibrationTransformToTrackerTool );
  igstkDeclareInputMacro( GetTransformToWorld );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( InternalSpatialObjectValidSet );
  igstkDeclareStateMacro( AttachedToTrackerTool );
  igstkDeclareStateMacro( AttachedToSpatialObjectParent );
  igstkDeclareStateMacro( AttachedToTrackerToolAndCalibrated );
  igstkDeclareStateMacro( AttachedToSpatialObjectParentAndLocated );

  /** Action methods to be invoked only by the state machine */
  void AttachToSpatialObjectParentProcessing();
  void AttachToTrackerToolProcessing();
  void SetTransformToSpatialObjectParentProcessing();
  void SetCalibrationTransformToTrackerToolProcessing();
  void BroadcastTransformToWorldProcessing();
  void BroadcastInvalidTransformMessageProcessing();
  void SetInternalSpatialObjectProcessing();

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Report when a request has been made at an incorrect time. */
  void ReportInvalidRequestProcessing();

  /** Computes the transform from this object to the World Coordinate System.
   *  This is a service method and should never be exposed to the API of this
   *  class.
   **/
  virtual const Transform & ComputeTransformToWorld() const;

};

} // end namespace igstk

#endif // __igstkSpatialObject_h
