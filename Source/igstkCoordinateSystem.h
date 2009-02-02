/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystem.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateSystem_h
#define __igstkCoordinateSystem_h

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTransform.h"

namespace igstk
{

/** \class CoordinateSystem
 * 
 * \brief This class represents the frame of a coordinate reference system.
 *
 * The class is intended to be used as an anchor relative to which you can 
 * position Spatial Objects in the graph scene. The class is not exposed to 
 * the API of IGSTK, instead it is used internally in the SpatialObject, 
 * Tracker, TrackerTool and View classes in order to provide a scaffolding in 
 * which each one of these classes can be attached as nodes of a scene graph.
 *
 * http://en.wikipedia.org/wiki/Scene_graph
 *
 *  The following diagram illustrates the state machine of 
 *  the CoordinateSystem class
 *
 *  \image html  igstkCoordinateSystem.png  "CoordinateSystem
 *  State Machine Diagram" 
 *
 *  \image latex igstkCoordinateSystem.eps "CoordinateSystem
 *  State Machine Diagram" 
 *
 *  
 * \ingroup Object
 */
class CoordinateSystem;

namespace Friends 
{

/** \class CoordinateSystemHelper 
 *
 * \brief A proxy that ensures the encapsulation of the
 * CoordinateSystem.
 *
 * This class is used to extract the internal CoordinateSystem 
 * instance from holder classes such as the SpatialObject, Tracker, 
 * TrackerTool, and View.
 *
 */ 
class CoordinateSystemHelper
{
public:
  /** Templated method to extract the CoordinateSystem from a holder
   * class. This method makes possible to protect the privacy of the Coordinate
   * Reference System in that class.
   */
  template <class T>
  static const CoordinateSystem* 
    GetCoordinateSystem( const T & input )
    {
    return input->GetCoordinateSystem();  // private
    }
};

} // end of Friends namespace 


class CoordinateSystem : public Object
{
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CoordinateSystem, Object )

  /** Tries to set the parent coordinate system and 
   *  the transform from this coordinate system to
   *  the parent. 
   */
  void RequestSetTransformAndParent(const Transform & t,
                                    const CoordinateSystem* parent);

  // Tries to update the transform to its parents   
  void RequestUpdateTransformToParent(const Transform & t);

  /** Request the transform to parent. */
  void RequestGetTransformToParent();

  /** Request that a transform is computed to targetCoordSys
   *   This method generates three possible events:
   *     CoordinateSystemTransformToEvent
   *     CoordinateSystemTransformToNullTargetEvent
   *     CoordinateSystemTransformToDisconnectedEvent
   *
   *   CoordinateSystemTransformToEvent is returned if the transform 
   *   is successfully computed. Otherwise, one of the other events is 
   *   generated.
   */
  void RequestComputeTransformTo(const 
                                 CoordinateSystem* targetCoordSys);

  /** Request that the coordinate system be detached from its parent. 
   */
  void RequestDetachFromParent();
  
  /**  Coordinate systems have a name to facilitate
   *   future export of the scene graph as a diagram.
   *   These macros get and set the name.
   */
  igstkSetStringMacro( Name );
  igstkGetStringMacro( Name );

  /**  Set/Get the type as a string */
  igstkSetStringMacro( Type );
  igstkGetStringMacro( Type ); 

protected:

  /** Constructor */
  CoordinateSystem( void );

  /** Destructor */
  ~CoordinateSystem( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private: 
  /** Copy constructor -- purposely not implemented. */
  CoordinateSystem(const Self&);

  /** Assignment operator -- purposely not implemented. */
  void operator=(const Self&);

  /** Parent node in the scene graph */
  Self::ConstPointer  m_Parent;

  /** Transform relating this node to the parent. This transform
   *  takes a point in the current coordinate system and transforms
   *  that point into the parent coordinate system.
   */
  Transform           m_TransformToParent;

  /** Coordinate system name for diagram export. */
  std::string         m_Name;

  /** Coordinate system type for diagram export. */
  std::string      m_Type;

  /** 
   * State machine declarations
   */

  /** Initial state. */
  igstkDeclareStateMacro( Initialized );

  /** State that signifies parent & transform have been set. */
  igstkDeclareStateMacro( ParentSet );

  /** Temporary states used for RequestComputeTransformTo. */
  igstkDeclareStateMacro( AttemptingComputeTransformTo );
  igstkDeclareStateMacro( AttemptingComputeTransformToInInitialized );

  /**
   *  State machine inputs
   */

  /** Inputs for RequestComputeTransformTo */
  igstkDeclareInputMacro( NullCoordinateSystem  );
  igstkDeclareInputMacro( ThisCoordinateSystem  );
  igstkDeclareInputMacro( ValidCoordinateSystem );

  /** Inputs for RequestSetTransformAndParent */
  igstkDeclareInputMacro( NullParent                     ); 
  igstkDeclareInputMacro( ThisParent                     );
  igstkDeclareInputMacro( ValidParent                    );
  igstkDeclareInputMacro( ParentCausesCycle              );
  
  /** Inputs used when attempting to compute a transform to another
   *  coordinate system.
   */
  igstkDeclareInputMacro( AncestorFound                  );
  igstkDeclareInputMacro( Disconnected                   );

  /** Input for detaching from parent.
   */
  igstkDeclareInputMacro( DetachFromParent               );
  
  /** Input for updating just transform  */
  igstkDeclareInputMacro( UpdateTransformToParent        );

  /** These variables hold the inputs from RequestSetTransformAndParent
   *  for later use by methods invoked from the state machine.
   */
  Transform                        m_TransformFromRequestSetTransformAndParent;
  Self::ConstPointer               m_ParentFromRequestSetTransformAndParent;

  /** Processing for RequestSetTransformAndParent called with a NULL parent. */
  void SetTransformAndParentNullParentProcessing();

  /** Processing for RequestSetTransformAndParent called with this as parent. */
  void SetTransformAndParentThisParentProcessing();

  /** Processing for RequestSetTransformAndParent when the parent causes a 
   *  cycle in the scene graph.
   */
  void SetTransformAndParentCycleProcessing(); 

  /** Default processing for RequestSetTransformAndParent. */
  void SetTransformAndParentProcessing();

  /** Default processing for RequestUpdateTransformToParent. */
  void UpdateTransformToParentProcessing();

  /** This variable holds the target coordinate system passed to 
   *  RequestComputeTransformTo so that later methods invoked by 
   *  the state machine can reference the target coordinate system.
   */
  Self::ConstPointer               m_TargetFromRequestComputeTransformTo;

  /** Processing for RequestComputeTransformTo called with this as 
   *  the target coordinate system.
   */
  void ComputeTransformToThisTargetProcessing();

  /** Processing for RequestComputeTransformTo called with NULL as 
   *  the target coordinate system.
   */
  void ComputeTransformToNullTargetProcessing();

  /** Processing for RequestComputeTransformTo called with a valid 
   *  target coordinate system.
   */
  void ComputeTransformToValidTargetProcessing();

  /** Processing for RequestComputeTransformTo when this and the
   *  target coordinate system are disconnected.
   */
  void ComputeTransformToDisconnectedProcessing();

  /** Processing for RequestComputeTransformTo when this and the
   *  target coordinate system are connected, i.e. a lowest 
   *  common ancestor has been found.
   */
  void ComputeTransformToAncestorFoundProcessing();

  /** Finds the lowest common ancestor between two nodes in the
   *  coordinate system graph. The lowest common ancestor is used
   *  to find the transform between two nodes in the coordinate
   *  system graph.
   */
  void FindLowestCommonAncestor(const Self* targetCoordinateSystem);

  /** Holds a pointer to the lowest common ancestor in the coordinate
   *  system graph. The lowest common ancestor is found by 
   *  FindLowestCommonAncestor and used to compute the transform 
   *  between two coordinate systems by composing each coordinate
   *  systems transform to the lowest common ancestor.
   */
  Self::ConstPointer                m_LowestCommonAncestor;

  /** This method is used internally to compute the transform to 
   *  the lowest common ancestor. The argument should be the 
   *  ancestor found by FindLowestCommonAncestor.
   */
  Transform ComputeTransformTo(const CoordinateSystem* ancestor) const;

  /** This method is used to ensure that we do not set a parent that 
   *  causes a cycle in the scene graph. CanReach returns true if 
   *  the target is currently reachable in the scene graph.
   */
  bool CanReach(const CoordinateSystem* target) const;

  /** This method is called when the state machine receives an 
   *  invalid request. 
   */
  void InvalidRequestProcessing();

  /** This method does nothing. Useful for some state-machine transitions. */
  void DoNothingProcessing();

  /** This method is called when we get a valid DetachFromParent request. */
  void DetachFromParentProcessing();

  /** This method is here so we can behave like higher-level objects with 
   *  coordinate systems.
   */
  const CoordinateSystem* GetCoordinateSystem() const;

  /** Make the CoordinateSystemHelper a friend. */
  igstkFriendClassMacro( igstk::Friends::CoordinateSystemHelper ); 

}; // class CoordinateSystem


//  
//   Macros defining events related to the CoordinateSystem.
//
igstkEventMacro( CoordinateSystemErrorEvent, IGSTKErrorEvent );
igstkEventMacro( CoordinateSystemSetParentError, CoordinateSystemErrorEvent );

/** This event should be invoked when RequestSetTransformAndParent is called 
 *  with a NULL parent.
 */
igstkEventMacro( CoordinateSystemNullParentEvent, 
                 CoordinateSystemSetParentError );

/** This event should be invoked when RequestSetTransformAndParent is called
 *  with the parent == this.
 */
igstkEventMacro( CoordinateSystemThisParentEvent, 
                 CoordinateSystemSetParentError );

/** This event should be invoked when RequestSetTransformAndParent is called
 *  with a parent that causes a cycle in the coordinate system graph.
 */
igstkLoadedConstObjectEventMacro( CoordinateSystemParentCycleEvent,
                                  CoordinateSystemSetParentError, 
                                  CoordinateSystem );

} // end namespace igstk

#endif // __igstkCoordinateSystem_h
