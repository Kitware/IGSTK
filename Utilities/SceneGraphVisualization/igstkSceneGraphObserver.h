/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraphObserver.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkSceneGraphObserver_h
#define __igstkSceneGraphObserver_h

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkTransform.h"
#include "igstkCoordinateSystemSetTransformResult.h"
#include "igstkSceneGraph.h"
#include "igstkCoordinateSystemTransformToResult.h"

namespace igstk
{

/**
 *  \class SceneGraphObserver
 *
 * \brief This class is used to observer the details required by the 
 * scene graph.
 *
 * This class is added as an observer to the CoordinateSystemObserver.
 *  \ingroup CoordinateSystem
 *
 */

class SceneGraphObserver : public ::itk::Command 
{
public:
  igstkStandardClassBasicTraitsMacro( SceneGraphObserver, ::itk::Command );
  igstkNewMacro( Self );  

protected:
  SceneGraphObserver() 
    {
    m_GotTransform = false;
    m_GotTransformNotAvailableMessage = false;
    m_SceneGraph = SceneGraph::getInstance();
    }
  ~SceneGraphObserver() {}
public:
    
  typedef CoordinateSystemSetTransformEvent     PositiveEventType;
  typedef TransformNotAvailableEvent           NegativeEventType;
        
  /** Payload contained in an event that returns a valid transform. This
   * payload includes the following items: Transform, Source coordinate system,
   * Destination coordinate system. */
  typedef CoordinateSystemSetTransformResult PayloadType;

  SceneGraph* m_SceneGraph;

  void ObserveTransformEventsFrom( Object * objectToObserve )
    {
    if( objectToObserve )
      {
      objectToObserve->AddObserver( PositiveEventType(), this );
      objectToObserve->AddObserver( NegativeEventType(), this );
      objectToObserve->AddObserver( CoordinateSystemTransformToEvent(), this);
      }
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
    }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {
    if( CoordinateSystemSetTransformEvent().CheckEvent( &event ) )
    {
      const CoordinateSystemSetTransformEvent  *transformEvent = 
                 dynamic_cast< const CoordinateSystemSetTransformEvent *>( &event );
    if(transformEvent->Get().IsAttach())
    {
    m_SceneGraph->AddCoordinateSystem(transformEvent);
    }
    else 
    {
      m_SceneGraph->DetachCoordinateSystem(transformEvent);
    }
  }

  if( CoordinateSystemTransformToEvent().CheckEvent( &event) )
  {
    const CoordinateSystemTransformToEvent  *transformEvent = 
                 dynamic_cast< const CoordinateSystemTransformToEvent *>( &event );
  if(transformEvent->Get().GetCommonAncestor() != NULL)
  {
    m_SceneGraph->ShowTheTransformPath(transformEvent);
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
      (void) caller; //Get rid of unused parameter warning
    }

  bool GotTransform() const
    {
    return m_GotTransform;
    }

  bool GotTransformNotAvailableMessage() const
    {
    return m_GotTransformNotAvailableMessage;
    }

  const Transform & GetTransform() const
    {
    return this->m_Transform;
    }

  const PayloadType & GetTransformBetweenCoordinateSystems() const
    {
    return m_TransformBetweenCoordinateSystems;
    }
 
  void Clear()
    {
    this->m_GotTransform = false;
    this->m_GotTransformNotAvailableMessage = false;
    }

private:

  PayloadType         m_TransformBetweenCoordinateSystems;
  Transform           m_Transform;
  
  bool                m_GotTransform;
  bool                m_GotTransformNotAvailableMessage;
};

} // end namespace igstk

#endif
