/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolObserverToSocketRelay.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

#include "igstkTrackerToolObserverToSocketRelay.h"

#include "igstkEvents.h"
#include "igstkTrackerTool.h"
#include "vtkMatrix4x4.h"

namespace igstk
{

/** Constructor */
TrackerToolObserverToSocketRelay::TrackerToolObserverToSocketRelay():m_StateMachine(this)
{
  this->m_Observer = ObserverType::New();
  this->m_Observer->SetCallbackFunction( this, & Self::ResendTransformThroughSocket );

  this->m_SocketController   = vtkSocketController::New();
  this->m_SocketCommunicator = vtkSocketCommunicator::New();

  this->m_Matrix = vtkMatrix4x4::New();
}

TrackerToolObserverToSocketRelay::~TrackerToolObserverToSocketRelay()
{
   this->m_Observer = NULL; // FIXME also disconnect as an observer

   this->m_SocketController->Delete();
   this->m_SocketCommunicator->Delete();
   this->m_Matrix->Delete();

   this->m_SocketController = NULL;
   this->m_SocketCommunicator = NULL;
   this->m_Matrix = NULL;
}


void
TrackerToolObserverToSocketRelay::RequestSetPort( int port )
{
  this->m_Port = port;
}


void
TrackerToolObserverToSocketRelay::RequestSetHostName( const char * hostname )
{
  this->m_HostName = hostname;
}



void
TrackerToolObserverToSocketRelay::RequestSetTrackerTool( const TrackerTool * trackerTool )
{
  this->m_TrackerTool = trackerTool;
  this->m_TrackerTool->AddObserver( TransformModifiedEvent(), this->m_Observer );
}


void
TrackerToolObserverToSocketRelay::RequestStart()
{
  char * hostname = const_cast< char * >( this->m_HostName.c_str() );

  std::cout << "Trying to connect to host = " << hostname << std::endl;
  std::cout << "In port = " << this->m_Port << std::endl;

  this->m_SocketController->Initialize();

  if( !this->m_SocketCommunicator->ConnectTo( hostname, this->m_Port ) )
    {
    std::cerr << "Client error: Could not connect to the server." << std::endl;
    }

  //this->m_Tag = 0;
  this->m_Tag = 17;
}
 

void
TrackerToolObserverToSocketRelay::ResendTransformThroughSocket( itk::Object * caller, const itk::EventObject & event )
{

  //
  // We send 12 parameters: 3x3 from the rotation matrix plus 3 from the
  // translation vector.
  //
  const int numberOfParametersToSend = 12;


  const igstk::TransformModifiedEvent * transformEvent = static_cast< const igstk::TransformModifiedEvent * >( &event );

  igstk::Transform transform = transformEvent->Get();

  std::cout << "Sending transform " << transform << std::endl;

  transform.ExportTransform( *(this->m_Matrix) );

  unsigned int counter = 0;
  double dataToBeSent[ numberOfParametersToSend ];

  for (unsigned int i = 0; i < 4; i++)
    {
    for (unsigned int j = 0; j < 3; j++)
      {
      dataToBeSent[counter++] = this->m_Matrix->GetElement( j, i );
      }
    }

  if( !this->m_SocketCommunicator->Send( dataToBeSent, numberOfParametersToSend, 1, this->m_Tag ) )
    {
    std::cerr << "Client error: Error sending data." << std::endl;
    }

  //this->m_Tag++;
}
 


/** Print Self function */
void TrackerToolObserverToSocketRelay::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Port: " << this->m_Port << std::endl;
  os << indent << "Hostname: " << this->m_HostName << std::endl;
  os << indent << "Tag: " << this->m_Tag << std::endl;
}

}
