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

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlClientSocket.h"


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
  
  void RequestSetDeviceName( const char * devicename );

  void RequestSetTrackerTool( const TrackerTool * tracker );

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

  typedef itk::MemberCommand< TrackerToolObserverToOpenIGTLinkRelay >   
                                                                   ObserverType;


private:

  ObserverType::Pointer       m_Observer;

  TrackerTool::ConstPointer   m_TrackerTool;

  vtkMatrix4x4              * m_Matrix;

  unsigned int                m_Tag;

  int                         m_Port;

  std::string                 m_HostName;

  igtl::ClientSocket::Pointer m_Socket;

  igtl::TransformMessage::Pointer m_TransformMessage;

};

} // end of namespace igstk

#endif //__igstk_TrackerToolObserverToOpenIGTLinkRelay_h_
