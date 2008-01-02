
#ifndef __igstkCoordinateSystemInterfaceMacros_h
#define __igstkCoordinateSystemInterfaceMacros_h

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
  template <class TTarget> \
  void RequestComputeTransformTo(const TTarget * target) \
    { \
    m_CoordinateReferenceSystemDelegator->RequestComputeTransformTo(target);\
    } \
  void RequestGetTransformToParent() \
    { \
    m_CoordinateReferenceSystemDelegator->RequestGetTransformToParent(); \
    } \
  template < class TParent > \
  void RequestSetTransformAndParent( const Transform & transformToParent, \
                                     const TParent * parent ) \
    { \
    m_CoordinateReferenceSystemDelegator->RequestSetTransformAndParent( \
                                     transformToParent, parent); \
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
  m_CoordinateReferenceSystemObserver->SetCallbackFunction(this, &Self::ObserverCallback); \
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
