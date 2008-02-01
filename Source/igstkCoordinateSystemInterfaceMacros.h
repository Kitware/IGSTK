/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkCoordinateSystemInterfaceMacros.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCoordinateSystemInterfaceMacros_h
#define __igstkCoordinateSystemInterfaceMacros_h

#include "igstkCoordinateSystemTransformToResult.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"
#include "igstkCoordinateReferenceSystemDelegator.h"

namespace igstk 
{

/**
 *  igstkCoordinateSystemClassInterfaceMacro defines the coordinate system 
 *  API used for managing spatial relationships between objects.
 *  To keep the interface definition common & centralized, it has been 
 *  placed in a macro.
 *
 *  If an object uses igstkCoordinateSystemClassInterfaceMacro() in 
 *  its header file, igstkCoordinateSystemClassInterfaceConstructorMacro 
 *  must be called in that object's constructor in order to initialize
 *  objects declared by this macro.
 */
#define igstkCoordinateSystemClassInterfaceMacro() \
public: \
  template <class TTargetPointer> \
  void RequestComputeTransformTo(const TTargetPointer & target) \
    { \
    m_CoordinateReferenceSystemDelegator->RequestComputeTransformTo(target);\
    } \
  void RequestGetTransformToParent() \
    { \
    m_CoordinateReferenceSystemDelegator->RequestGetTransformToParent(); \
    } \
  template < class TParentPointer > \
  void RequestSetTransformAndParent( const Transform & transformToParent, \
                                     TParentPointer parent ) \
    { \
    if ( this->IsInternalTransformRequired() == false ) \
      { \
      m_CoordinateReferenceSystemDelegator->RequestSetTransformAndParent( \
                                     transformToParent, parent); \
      } \
    else \
      { \
      Transform internalTransform = this->GetInternalTransform(); \
      Transform transformToParentWithInternalTransform = \
                               Transform::TransformCompose( \
                                              transformToParent, \
                                              internalTransform ); \
      m_CoordinateReferenceSystemDelegator->RequestSetTransformAndParent( \
                          transformToParentWithInternalTransform, parent); \
      }\
    } \
protected: \
  virtual bool IsInternalTransformRequired() \
    { \
    return false; \
    } \
  virtual Transform GetInternalTransform() const \
    { \
    Transform identity; \
    identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() ); \
    return identity; \
    } \
private: \
  CoordinateReferenceSystemDelegator::Pointer \
                                  m_CoordinateReferenceSystemDelegator; \
  typedef ::itk::ReceptorMemberCommand< Self > CoordinateSystemObserverType; \
  CoordinateSystemObserverType::Pointer m_CoordinateReferenceSystemObserver; \
  void ObserverCallback(const ::itk::EventObject & eventvar) \
    { \
    this->InvokeEvent( eventvar ); \
    } \
  const CoordinateReferenceSystem* GetCoordinateReferenceSystem() const \
    { \
    return \
      igstk::Friends::CoordinateReferenceSystemHelper:: \
        GetCoordinateReferenceSystem(  \
                       m_CoordinateReferenceSystemDelegator.GetPointer() ); \
    } \
  igstkFriendClassMacro( igstk::Friends::CoordinateReferenceSystemHelper ); 

/**
 *  igstkCoordinateSystemClassInterfaceConstructorMacro initializes the 
 *  observer and delegator used in the coordinate system API. This macro
 *  should be called from the constructor of an object which has 
 *  igstkCoordinateSystemClassInterfaceMacro in its class header.
 */
#define igstkCoordinateSystemClassInterfaceConstructorMacro() \
  m_CoordinateReferenceSystemObserver = CoordinateSystemObserverType::New(); \
  m_CoordinateReferenceSystemObserver->SetCallbackFunction(this, \
                                                   &Self::ObserverCallback); \
  m_CoordinateReferenceSystemDelegator = \
                          CoordinateReferenceSystemDelegator::New(); \
  m_CoordinateReferenceSystemDelegator->AddObserver( \
    CoordinateReferenceSystemTransformToNullTargetEvent() \
    , m_CoordinateReferenceSystemObserver ); \
  m_CoordinateReferenceSystemDelegator->AddObserver( \
    CoordinateReferenceSystemTransformToDisconnectedEvent() \
    , m_CoordinateReferenceSystemObserver ); \
  m_CoordinateReferenceSystemDelegator->AddObserver( \
    CoordinateReferenceSystemNullParentEvent() \
    , m_CoordinateReferenceSystemObserver ); \
  m_CoordinateReferenceSystemDelegator->AddObserver( \
    CoordinateReferenceSystemThisParentEvent() \
    , m_CoordinateReferenceSystemObserver ); \
  m_CoordinateReferenceSystemDelegator->AddObserver( \
    CoordinateReferenceSystemParentCycleEvent() \
    , m_CoordinateReferenceSystemObserver ); \
  m_CoordinateReferenceSystemDelegator->AddObserver( \
    CoordinateReferenceSystemTransformToEvent() \
    , m_CoordinateReferenceSystemObserver ); 

} // end namespace igstk

#endif // #ifndef __igstkCoordinateSystemInterfaceMacros_h
