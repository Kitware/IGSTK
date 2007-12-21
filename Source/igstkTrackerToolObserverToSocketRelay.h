/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolObserverToSocketRelay.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerToolObserverToSocketRelay_h
#define __igstkTrackerToolObserverToSocketRelay_h

#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkTrackerTool.h"

#include "vtkSocketCommunicator.h"
#include "vtkSocketController.h"

namespace igstk
{
/** \class TrackerToolObserverToSocketRelay
 *
 *  \brief This class observe a TrackerTool for Transforms events and relay them to a socket connection.
 *
 */

class TrackerToolObserverToSocketRelay  : public Object
{
 
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TrackerToolObserverToSocketRelay, Object )

public:

  void RequestSetPort( int port );

  void RequestSetHostName( const char * hostname );

  void RequestSetTrackerTool( const TrackerTool * tracker );

  void RequestStart();

protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  TrackerToolObserverToSocketRelay(void);

  virtual ~TrackerToolObserverToSocketRelay(void);

  /** Print the object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  void ResendTransformThroughSocket( itk::Object * caller, const itk::EventObject & event );

  typedef itk::MemberCommand< TrackerToolObserverToSocketRelay >   ObserverType;


private:

  int                         m_Port;

  ObserverType::Pointer       m_Observer;

  TrackerTool::ConstPointer   m_TrackerTool;

  vtkSocketController       * m_SocketController;

  vtkSocketCommunicator     * m_SocketCommunicator;

  vtkMatrix4x4              * m_Matrix;

  std::string                 m_HostName;
};

} // end of namespace igstk

#endif //__igstk_TrackerToolObserverToSocketRelay_h_
