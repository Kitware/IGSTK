/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystem.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateReferenceSystem_h
#define __igstkCoordinateReferenceSystem_h

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTransform.h"
#include "itkTimeProbe.h"

namespace igstk
{

/** \class CoordinateReferenceSystem
 * 
 * \brief This class represents the frame of a coordinate reference system.
 *
 * The class is intended to be used as an anchor relative to which you can 
 * position Spatial Objects in the graph scene. The class is not exposed to 
 * the API of IGSTK, instead it is used internally in the SpatialObject, Tracker,
 * TrackerTool and View classes in order to provide a scaffolding in which 
 * each one of these classes can be attached as nodes of a scene graph.
 *
 * http://en.wikipedia.org/wiki/Scene_graph
 *
 *  The following diagram illustrates the state machine of 
 *  the CoordinateReferenceSystem class
 *
 *  \image html  igstkCoordinateReferenceSystem.png  "CoordinateReferenceSystem
 *  State Machine Diagram" 
 *
 *  \image latex igstkCoordinateReferenceSystem.eps "CoordinateReferenceSystem
 *  State Machine Diagram" 
 *
 *  
 * \ingroup Object
 */
class CoordinateReferenceSystem;

namespace Friends 
{

/** \class CoordinateReferenceSystemHelper 
 *
 * \brief A proxy that ensures the encapsulation of the
 * CoordinateReferenceSystem.
 *
 * This class is used to extract the internal CoordinateReferenceSystem instance 
 * from holder classes such as the SpatialObject, Tracker, TrackerTool, and View.
 *
 */ 
class CoordinateReferenceSystemHelper
{
public:
  /** Templated method to extract the CoordinateReferenceSystem from a holder
   * class. This method makes possible to protect the privacy of the Coordinate
   * Reference System in that class.
   */
  template <class T>
  static const CoordinateReferenceSystem* 
    GetCoordinateReferenceSystem( const T & input )
    {
    return input->GetCoordinateReferenceSystem();  // private
    }
};

} // end of Friends namespace 


class CoordinateReferenceSystem : public Object
{
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CoordinateReferenceSystem, Object )

  /** Tries to set the parent coordinate system and 
   *  the transform from this coordinate system to
   *  the parent. 
   */
  void RequestSetTransformAndParent(const Transform & t,
                                    const CoordinateReferenceSystem* parent);

  /** Request the transform to parent. */
  void RequestGetTransformToParent();

  /** Request that a transform is computed to targetCoordSys
   *   This method generates three possible events:
   *     CoordinateReferenceSystemTransformToEvent
   *     CoordinateReferenceSystemTransformToNullTargetEvent
   *     CoordinateReferenceSystemTransformToDisconnectedEvent
   *
   *   CoordinateReferenceSystemTransformToEvent is returned if the transform 
   *   is successfully computed. Otherwise, one of the other events is 
   *   generated.
   */
  void RequestComputeTransformTo(const 
                                 CoordinateReferenceSystem* targetCoordSys);

  /** void RequestDetach(); */
  
  /**  Coordinate systems have a name to facilitate
   *   future export of the scene graph as a diagram.
   *   These macros get and set the name.
   */
  igstkSetStringMacro( Name );
  igstkGetStringMacro( Name );

protected:

  /** Constructor */
  CoordinateReferenceSystem( void );

  /** Destructor */
  ~CoordinateReferenceSystem( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private: 
  /** Copy constructor -- purposely not implemented. */
  CoordinateReferenceSystem(const Self&);

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
  igstkDeclareInputMacro( NullCoordinateReferenceSystem  );
  igstkDeclareInputMacro( ThisCoordinateReferenceSystem  );
  igstkDeclareInputMacro( ValidCoordinateReferenceSystem );

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
  Transform ComputeTransformTo(const CoordinateReferenceSystem* ancestor) const;

  /** This method is used to ensure that we do not set a parent that 
   *  causes a cycle in the scene graph. CanReach returns true if 
   *  the target is currently reachable in the scene graph.
   */
  bool CanReach(const CoordinateReferenceSystem* target) const;

  /** This method is called when the state machine receives an 
   *  invalid request. 
   */
  void InvalidRequestProcessing();

  /** This method is here so we can behave like higher-level objects with 
   *  coordinate systems.
   */
  const CoordinateReferenceSystem* GetCoordinateReferenceSystem() const;

  /** Make the CoordinateReferenceSystemHelper a friend. */
  igstkFriendClassMacro( igstk::Friends::CoordinateReferenceSystemHelper ); 

}; // class CoordinateReferenceSystem


//  
//   Macros defining events related to the CoordinateReferenceSystem.
//
igstkEventMacro( CoordinateReferenceSystemNullParentEvent, IGSTKEvent );
igstkEventMacro( CoordinateReferenceSystemThisParentEvent, IGSTKEvent );
igstkLoadedConstObjectEventMacro( CoordinateReferenceSystemParentCycleEvent,
                                  IGSTKEvent, CoordinateReferenceSystem );

} // end namespace igstk

#endif // __igstkCoordinateReferenceSystem_h
