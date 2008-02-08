/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformObserverTestHelper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkTransform.h"

namespace igstk
{

class TransformObserverTestHelper : public ::itk::Command 
{
public:
  typedef  TransformObserverTestHelper     Self;
  typedef  ::itk::Command                  Superclass;
  typedef  ::itk::SmartPointer<Self>       Pointer;
  itkNewMacro( Self );

protected:
  TransformObserverTestHelper() 
    {
    m_GotTransform = false;
    m_GotTransformNotAvailableMessage = false;
    }
  ~TransformObserverTestHelper() {}
public:
    
  typedef ::igstk::CoordinateSystemTransformToEvent              PositiveEventType;
  typedef ::igstk::TransformNotAvailableEvent                             NegativeEventType;
        
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
    }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {

    m_GotTransform = false;
    m_GotTransformNotAvailableMessage = false;

    if( PositiveEventType().CheckEvent( &event ) )
      {
      const PositiveEventType * transformEvent = 
                 dynamic_cast< const PositiveEventType *>( &event );
      if( transformEvent )
        {
        m_TransformToResult = transformEvent->Get();
        m_Transform = m_TransformToResult.GetTransform();
        m_GotTransform = true;
        m_GotTransformNotAvailableMessage = false;
        }
      }

    if( NegativeEventType().CheckEvent( &event ) )
      {
      const NegativeEventType * negativeEvent = 
                 dynamic_cast< const NegativeEventType *>( &event );
      if( negativeEvent )
        {
        m_GotTransform = false;
        m_GotTransformNotAvailableMessage = true;
        }
      }
    }

  bool GotTransform() const
    {
    return m_GotTransform;
    }

  bool GotTransformNotAvailableMessage() const
    {
    return m_GotTransformNotAvailableMessage;
    }

  const ::igstk::Transform & GetTransform() const
    {
    return m_Transform;
    }
        
private:

  typedef ::igstk::CoordinateSystemTransformToResult TransformToResultType;
  typedef ::igstk::Transform                                  TransformType;
  
  TransformToResultType   m_TransformToResult;
  TransformType           m_Transform;
  
  bool                m_GotTransform;
  bool                m_GotTransformNotAvailableMessage;
};

} // end namespace igstk
