/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolObserverToOpenIGTLinkRelay.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerToolObserverToOpenIGTLinkRelay_h
#define __igstkTrackerToolObserverToOpenIGTLinkRelay_h

#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkTrackerTool.h"

// #include "vtkSocketCommunicator.h"
// #include "vtkSocketController.h"
#include "AcquisitionTrackingSimulator.h"
#include "TransferOpenIGTLink.h"

namespace igstk
{
/** \class TrackerToolObserverToOpenIGTLinkRelay
 *
 *  \brief This class observe a TrackerTool for Transforms events and relay
 *  them to a socket connection.
 *
 */

class TrackerToolObserverToOpenIGTLinkRelay  : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TrackerToolObserverToOpenIGTLinkRelay, Object )

public:

  void RequestSetPort( int port );

  void RequestSetHostName( const char * hostname );

  void RequestSetTrackerTool( const TrackerTool * tracker );

  void RequestSetFramesPerSecond( double fps  );

  void RequestStart();

protected:

  /** Constructor is protected in order to enforce
   *  the use of the New() operator */
  TrackerToolObserverToOpenIGTLinkRelay(void);

  virtual ~TrackerToolObserverToOpenIGTLinkRelay(void);

  /** Print the object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  void ResendTransformThroughOpenIGTLink(
    itk::Object * caller, const itk::EventObject & event );

  typedef itk::MemberCommand< TrackerToolObserverToOpenIGTLinkRelay >   ObserverType;


private:

  ObserverType::Pointer       m_Observer;

  TrackerTool::ConstPointer   m_TrackerTool;

//  vtkSocketController       * m_SocketController;

//  vtkSocketCommunicator     * m_SocketCommunicator;

  bool                        m_WaitingForNextRequestFromOpenIGTLink;

  vtkMatrix4x4              * m_Matrix;

  unsigned int                m_Tag;

  int                         m_Port;

  std::string                 m_HostName;

  AcquisitionTrackingSimulator *m_Acquisition;
  TransferOpenIGTLink *m_Transfer;
  double m_FramesPerSecond;

};

} // end of namespace igstk

#endif //__igstk_TrackerToolObserverToOpenIGTLinkRelay_h_
