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
#include "igstkCoordinateSystemDelegator.h"

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
    m_CoordinateSystemDelegator->RequestComputeTransformTo(target);\
    } \
  void RequestGetTransformToParent() \
    { \
    m_CoordinateSystemDelegator->RequestGetTransformToParent(); \
    } \
  void RequestDetach()\
    { \
    m_CoordinateSystemDelegator->RequestDetach();\
    } \
  template < class TParentPointer > \
  void RequestSetTransformAndParent( const Transform & transformToParent, \
                                     TParentPointer parent ) \
    { \
    if ( this->IsInternalTransformRequired() == false ) \
      { \
      m_CoordinateSystemDelegator->RequestSetTransformAndParent( \
                                     transformToParent, parent); \
      } \
    else \
      { \
      Transform internalTransform = this->GetInternalTransform(); \
      Transform transformToParentWithInternalTransform = \
                               Transform::TransformCompose( \
                                              transformToParent, \
                                              internalTransform ); \
      m_CoordinateSystemDelegator->RequestSetTransformAndParent( \
                          transformToParentWithInternalTransform, parent); \
      }\
    } \
  bool IsCoordinateSystem(const CoordinateSystem* inCS) const \
    { \
    return m_CoordinateSystemDelegator-> \
      IsCoordinateSystem( inCS ); \
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
  CoordinateSystemDelegator::Pointer \
                                  m_CoordinateSystemDelegator; \
  typedef ::itk::ReceptorMemberCommand< Self > CoordinateSystemObserverType; \
  CoordinateSystemObserverType::Pointer m_CoordinateSystemObserver; \
  void ObserverCallback(const ::itk::EventObject & eventvar) \
    { \
    this->InvokeEvent( eventvar ); \
    } \
  const CoordinateSystem* GetCoordinateSystem() const \
    { \
    return \
      igstk::Friends::CoordinateSystemHelper:: \
        GetCoordinateSystem(  \
                       m_CoordinateSystemDelegator.GetPointer() ); \
    } \
  igstkFriendClassMacro( igstk::Friends::CoordinateSystemHelper ); 

/**
 *  igstkCoordinateSystemClassInterfaceConstructorMacro initializes the 
 *  observer and delegator used in the coordinate system API. This macro
 *  should be called from the constructor of an object which has 
 *  igstkCoordinateSystemClassInterfaceMacro in its class header.
 */
#define igstkCoordinateSystemClassInterfaceConstructorMacro() \
  m_CoordinateSystemObserver = CoordinateSystemObserverType::New(); \
  m_CoordinateSystemObserver->SetCallbackFunction(this, \
                                                   &Self::ObserverCallback); \
  m_CoordinateSystemDelegator = \
                          CoordinateSystemDelegator::New(); \
  m_CoordinateSystemDelegator->AddObserver( \
    CoordinateSystemTransformToNullTargetEvent() \
    , m_CoordinateSystemObserver ); \
  m_CoordinateSystemDelegator->AddObserver( \
    CoordinateSystemTransformToDisconnectedEvent() \
    , m_CoordinateSystemObserver ); \
  m_CoordinateSystemDelegator->AddObserver( \
    CoordinateSystemNullParentEvent() \
    , m_CoordinateSystemObserver ); \
  m_CoordinateSystemDelegator->AddObserver( \
    CoordinateSystemThisParentEvent() \
    , m_CoordinateSystemObserver ); \
  m_CoordinateSystemDelegator->AddObserver( \
    CoordinateSystemParentCycleEvent() \
    , m_CoordinateSystemObserver ); \
  m_CoordinateSystemDelegator->AddObserver( \
    CoordinateSystemTransformToEvent() \
    , m_CoordinateSystemObserver ); \
  std::stringstream tempStream; \
  tempStream << this->GetNameOfClass() << " 0x"; \
  tempStream << static_cast<void*>(this); \
  std::string name = tempStream.str(); \
  m_CoordinateSystemDelegator->SetName( name.c_str() );

} // end namespace igstk

#endif // #ifndef __igstkCoordinateSystemInterfaceMacros_h
