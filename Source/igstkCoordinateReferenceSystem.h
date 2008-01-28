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
 * TrackerTool and View classes in order to provide an scaffolding in which 
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
  
  /**  Development only */
  igstkSetStringMacro( Name );
  igstkGetStringMacro( Name );

protected:

  CoordinateReferenceSystem( void );
  ~CoordinateReferenceSystem( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private: 
  CoordinateReferenceSystem(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Parent node in the scene graph
  Self::ConstPointer  m_Parent;

  // Transform relating this node to the parent 
  Transform           m_TransformToParent;

  // Name this coordinate system for debugging.
  std::string         m_Name;

  /** 
   * State machine
   */

  // Initial state
  igstkDeclareStateMacro( Initialized );

  // State for when parent & transform have been set
  igstkDeclareStateMacro( ParentSet );

  // State for RequestComputeTransformTo
  igstkDeclareStateMacro( AttemptingComputeTransformTo );
  igstkDeclareStateMacro( AttemptingComputeTransformToInInitialized );

  //
  // Inputs
  //

  // RequestComputeTransformTo
  igstkDeclareInputMacro( NullCoordinateReferenceSystem  );
  igstkDeclareInputMacro( ThisCoordinateReferenceSystem  );
  igstkDeclareInputMacro( ValidCoordinateReferenceSystem );

  // RequestSetTransformAndParent
  igstkDeclareInputMacro( NullParent                     ); 
  igstkDeclareInputMacro( ThisParent                     );
  igstkDeclareInputMacro( ValidParent                    );
  igstkDeclareInputMacro( ParentCausesCycle              );
  
  igstkDeclareInputMacro( AncestorFound                  );
  igstkDeclareInputMacro( Disconnected                   );

  Transform                        m_RequestSetTransformAndParentTransform;
  Self::ConstPointer               m_RequestSetTransformAndParentParent;

  /** This method modifies the m_Parent and m_TransformToParent ivars */
  void SetTransformAndParentProcessing();
  void SetTransformAndParentNullParentProcessing();
  void SetTransformAndParentThisParentProcessing();
  void SetTransformAndParentCycleProcessing(); 

  Self::ConstPointer               m_ComputeTransformToTarget;

  void ComputeTransformToThisTargetProcessing();
  void ComputeTransformToNullTargetProcessing();
  void ComputeTransformToValidTargetProcessing();

  void FindLowestCommonAncestor(const Self* targetCoordinateSystem);

  void ComputeTransformToDisconnectedProcessing();
  void ComputeTransformToAncestorFoundProcessing();

  Transform                         m_ComputedTransform;


  Self::ConstPointer                m_LowestCommonAncestor;

  Transform ComputeTransformTo(const CoordinateReferenceSystem* ancestor) const;


  mutable itk::TimeProbe                    m_LowestCommonAncestorTimer;
  mutable itk::TimeProbe                    m_ComputeTransformToTimer;
  mutable itk::TimeProbe                    m_RequestComputeTransformToTimer;
  bool                                      m_ReportTiming;

  bool CanReach(const CoordinateReferenceSystem* target) const;

  void InvalidRequestProcessing();

  /** So we can behave like higher-level objects with coordinate systems. */
  const CoordinateReferenceSystem* GetCoordinateReferenceSystem() const
    {
    return this;
    }

  igstkFriendClassMacro( igstk::Friends::CoordinateReferenceSystemHelper ); 

public:
  /** Development only */
  void SetReportTiming(bool val)
    {
    m_ReportTiming = val;
    }

  /** Development only */
  bool GetReportTiming()
    {
    return m_ReportTiming;
    }

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
