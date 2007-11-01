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
 * position Spatial Objects in the graph scene. 
 *
 *  
 * \ingroup Object
 */

class CoordinateReferenceSystem : public Object
{
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CoordinateReferenceSystem, Object )

  void RequestSetTransformAndParent(Transform t, const CoordinateReferenceSystem* parent);

  /** Helper method. */
  void RequestGetTransformToParent()
    {
    this->RequestComputeTransformTo(this->m_Parent);
    }

  void RequestComputeTransformTo(const CoordinateReferenceSystem* targetCoordSys);

  void RequestDetach();

  //
  // Development only
  //
  void SetName(const std::string& name)
    {
    this->m_Name = name;
    }

  void SetName(const char* name)
    {
    this->SetName(std::string(name));
    }

  std::string GetName() const
    {
    return m_Name;
    }

protected:

  CoordinateReferenceSystem( void );
  ~CoordinateReferenceSystem( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private: 

  Self::ConstPointer  m_Parent;               // Parent node in the scene graph
  Transform           m_TransformToParent;    // Transform relating this node to the parent 
  std::string         m_Name;                 // Name this coordinate system for debugging.

  //-------------------------------------------------------------
  // State machine
  //-------------------------------------------------------------

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

  // igstkDeclareInputMacro( NoCommonAncestor               );
  // igstkDeclareInputMacro( CommonAncestorFound            );
  
  igstkDeclareInputMacro( AncestorFound                  );
  igstkDeclareInputMacro( Disconnected                   );



  Transform                        m_RequestSetTransformAndParentTransform;
  Self::ConstPointer                  m_RequestSetTransformAndParentParent;

  /** This method modifies the m_Parent and m_TransformToParent ivars */
  void SetTransformAndParentProcessing();
  void SetTransformAndParentNullParentProcessing();
  void SetTransformAndParentThisParentProcessing();
  void SetTransformAndParentCycleProcessing(); 

  Self::ConstPointer                  m_ComputeTransformToTarget;

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

public:
  void SetReportTiming(bool val)
    {
    m_ReportTiming = val;
    }

  bool GetReportTiming()
    {
    return m_ReportTiming;
    }

}; // class CoordinateReferenceSystem

//---------------------------------------------------------------------------
//
// Macros & classes declaring events to be used when making requests of 
// the CoordinateReferenceSystem.
//
//---------------------------------------------------------------------------


//
// This class encapsulates the results of asking the coordinate reference
// system for a transform to another coordinate reference system. It is
// meant to be used as payload in an event that is created after a 
// successful call to RequestTransformTo(). 
//
class CoordinateReferenceSystemTransformToResult
{
public:

  CoordinateReferenceSystemTransformToResult()
    {

    }

  inline void Initialize(const Transform& trans, 
                  const CoordinateReferenceSystem* src,
                  const CoordinateReferenceSystem* dst)
    {
    m_Transform = trans;
    m_Source = src;
    m_Destination = dst;
    }

  Transform                                 m_Transform;
  CoordinateReferenceSystem::ConstPointer   m_Source;
  CoordinateReferenceSystem::ConstPointer   m_Destination;
};



igstkLoadedEventMacro( CoordinateReferenceSystemTransformToEvent, 
                  IGSTKEvent, CoordinateReferenceSystemTransformToResult );


/*
class CoordinateReferenceSystemLowestCommonAncestorResult
{
public:

  CoordinateReferenceSystemLowestCommonAncestorResult()
    {

    }

  void Initialize(const CoordinateReferenceSystem* node1,
                  const CoordinateReferenceSystem* node2,
                  const CoordinateReferenceSystem* ancestor)
    {
    m_Node1 = node1;
    m_Node2 = node2;
    m_Ancestor = ancestor;
    }

  CoordinateReferenceSystem::ConstPointer   m_Node1;
  CoordinateReferenceSystem::ConstPointer   m_Node2;
  CoordinateReferenceSystem::ConstPointer   m_Ancestor;
};

igstkLoadedEventMacro( CoordinateReferenceSystemLowestCommonAncestorEvent, 
  IGSTKEvent, CoordinateReferenceSystemLowestCommonAncestorResult );
*/

class CoordinateReferenceSystemTransformToErrorResult
{
public:

  CoordinateReferenceSystemTransformToErrorResult()
    {

    }

  inline void Initialize(const CoordinateReferenceSystem* node1,
                  const CoordinateReferenceSystem* node2)
    {
    m_Node1 = node1;
    m_Node2 = node2;
    }

  CoordinateReferenceSystem::ConstPointer   m_Node1;
  CoordinateReferenceSystem::ConstPointer   m_Node2;
};

/*
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToErrorEvent, 
  IGSTKEvent, CoordinateReferenceSystemTransformToErrorResult );
*/
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToNullTargetEvent, IGSTKEvent, CoordinateReferenceSystemTransformToErrorResult );
igstkLoadedEventMacro( CoordinateReferenceSystemTransformToDisconnectedEvent, IGSTKEvent, CoordinateReferenceSystemTransformToErrorResult );


igstkEventMacro( CoordinateReferenceSystemNullParentEvent, IGSTKEvent );
igstkEventMacro( CoordinateReferenceSystemThisParentEvent, IGSTKEvent );
igstkLoadedConstObjectEventMacro( CoordinateReferenceSystemParentCycleEvent, IGSTKEvent, CoordinateReferenceSystem );

namespace Friends 
{

class CoordinateReferenceSystemHelper
{
public:
  template <class T>
  static const CoordinateReferenceSystem* GetCoordinateReferenceSystem( const T * input )
    {
    return input->GetCoordinateReferenceSystem();  // private
    }
};

} // end of Friends namespace 

} // end namespace igstk

#endif // __igstkCoordinateReferenceSystem_h
