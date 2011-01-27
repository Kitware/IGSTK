/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObjectTest.cxx
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

#include <iostream>

#include "igstkConfigure.h"
#include "igstkVascularNetworkObject.h"
#include "igstkTubeObject.h"
#include "igstkRealTimeClock.h"
#include "igstkVTKLoggerOutput.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkAxesObject.h"

#include "igstkTransformObserver.h"

namespace igstk
{
namespace VascularNetworkObjectTest
{

class VesselObserver : public ::itk::Command 
{
public:
  typedef  VesselObserver             Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  VesselObserver() 
    {
    m_Vessel = NULL;
    m_GotVessel = false;
    m_GotVesselNotAvailableMessage = true;
    }
  ~VesselObserver() {}
public:
    
  typedef ::igstk::VesselObjectModifiedEvent      PositiveEventType;
  typedef ::igstk::VesselObjectNotAvailableEvent  NegativeEventType;
        
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
    }

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {

    m_Vessel = NULL;
    m_GotVessel = false;
    m_GotVesselNotAvailableMessage = true;

    if( PositiveEventType().CheckEvent( &event ) )
      {
      const PositiveEventType * vesselEvent = 
                 dynamic_cast< const PositiveEventType *>( &event );
      if( vesselEvent )
        {
        m_Vessel = vesselEvent->Get();
        m_GotVessel = true;
        m_GotVesselNotAvailableMessage = false;
        }
      }

    if( NegativeEventType().CheckEvent( &event ) )
      {
      const NegativeEventType * negativeEvent = 
                 dynamic_cast< const NegativeEventType *>( &event );
      if( negativeEvent )
        {
        m_Vessel = NULL;
        m_GotVessel = false;
        m_GotVesselNotAvailableMessage = true;
        }
      }

    }

  bool GotVessel() const
    {
    return m_GotVessel;
    }

  bool GotVesselNotAvailableMessage() const
    {
    return m_GotVesselNotAvailableMessage;
    }

  const ::igstk::VesselObject * GetVessel() const
    {
    return m_Vessel;
    }
        
private:

  ::igstk::VesselObject *   m_Vessel;
  bool                      m_GotVessel;
  bool                      m_GotVesselNotAvailableMessage;
};
} // end namespace VascularNetworkObjectTest
} // end namespace igstk


int igstkVascularNetworkObjectTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  // Create an igstk::VTKLoggerOutput and then test it.
  igstk::VTKLoggerOutput::Pointer vtkLoggerOutput 
                                                = igstk::VTKLoggerOutput::New();
  vtkLoggerOutput->OverrideVTKWindow();
  vtkLoggerOutput->SetLogger(logger);  // redirect messages from VTK 
                                       // OutputWindow -> logger

  igstk::AxesObject::Pointer worldReference = igstk::AxesObject::New();


  typedef igstk::VascularNetworkObject  ObjectType;
  ObjectType::Pointer network = ObjectType::New();
  network->SetLogger( logger );

  // Testing PrintSelf()
  network->Print(std::cout);
  network->GetNameOfClass();


  // Test GetTransform()
  std::cout << "Testing Set/GetTransform(): ";

  const double tolerance = 1e-8;
  double validityTimeInMilliseconds = 20000.0;  // 20 seconds
  igstk::Transform transform;
  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );
  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  typedef igstk::TransformObserver TransformObserverType;

  TransformObserverType::Pointer transformObserver 
    = TransformObserverType::New();

  transformObserver->ObserveTransformEventsFrom( network );

  transformObserver->Clear();

  network->RequestSetTransformAndParent( transform, worldReference );

  network->RequestGetTransformToParent();
  
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "The VascularNetworkObject did not returned a Transform event" 
              << std::endl;
    return EXIT_FAILURE;
    }
  

  igstk::Transform  transform2 = transformObserver->GetTransform();

  igstk::Transform::VectorType translation2 = transform2.GetTranslation();
  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "Translation component is out of range [FAILED]" 
                << std::endl;
      std::cerr << "input  translation = " << translation << std::endl;
      std::cerr << "output translation = " << translation2 << std::endl;
      return EXIT_FAILURE;
      }
    }

  igstk::Transform::VersorType rotation2 = transform2.GetRotation();
  if( fabs( rotation2.GetX() - rotation.GetX() ) > tolerance ||
      fabs( rotation2.GetY() - rotation.GetY() ) > tolerance ||
      fabs( rotation2.GetZ() - rotation.GetZ() ) > tolerance ||
      fabs( rotation2.GetW() - rotation.GetW() ) > tolerance     )
    {
    std::cerr << "Rotation component is out of range [FAILED]" << std::endl;
    std::cerr << "input  rotation = " << rotation << std::endl;
    std::cerr << "output rotation = " << rotation2 << std::endl;
    return EXIT_FAILURE;
    }

  //
  //  Test methods related to vessel addition.
  //
  if( network->GetNumberOfVessels()  != 0 )
    {
    std::cerr << "Error: initial number of vessels is not zero" << std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::VesselObject  VesselObjectType;

  const unsigned int maximumNumberOfVesselrenToTry = 100;

  VesselObjectType::Pointer vessels[ maximumNumberOfVesselrenToTry ];

  igstk::Transform vesselTransform;
  vesselTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  for( unsigned int birthId = 0; birthId < maximumNumberOfVesselrenToTry; birthId++ )
    {
    VesselObjectType::Pointer newBorn = VesselObjectType::New();
    vessels[ birthId ] = newBorn;
  
    network->RequestAddVessel( vesselTransform, newBorn );
  
    if( network->GetNumberOfVessels()  != birthId+1 )
      {
      std::cerr << "Error: number of vessels is not " << birthId+1 << std::endl;
      return EXIT_FAILURE;
      }
    }

  std::cout << "GetNumberOfVessels() Test PASSED " << std::endl;

  typedef igstk::VascularNetworkObjectTest::VesselObserver  VesselObserverType;

  VesselObserverType::Pointer vesselObserver = VesselObserverType::New();

  network->AddObserver( igstk::VesselObjectModifiedEvent(), vesselObserver );
  network->AddObserver( igstk::VesselObjectNotAvailableEvent(), vesselObserver );

  for( unsigned int vesselId = 0; 
       vesselId < maximumNumberOfVesselrenToTry; vesselId++ )
    {
    network->RequestGetVessel( vesselId );

    if( !vesselObserver->GotVessel() )
      {
      std::cerr << "Error: problem getting Vessel " << vesselId << std::endl;
      return EXIT_FAILURE;
      }

    if( vesselObserver->GotVesselNotAvailableMessage() )
      {
      std::cerr << "Error: problem getting Vessel, false negative ";
      std::cerr << vesselId << std::endl;
      return EXIT_FAILURE;
      }


    igstk::VesselObject::ConstPointer receivedVessel = vesselObserver->GetVessel();

    if( receivedVessel.GetPointer() != vessels[ vesselId ].GetPointer() )
      {
      std::cerr << "Error: wrong vessel has been returned " << std::endl;
      return EXIT_FAILURE;
      }
    }

  //
  // Now ask for a non existing vessel, verify that we don't get it,
  // and that we get the event indicating that it is not available.
  //
  network->RequestGetVessel( maximumNumberOfVesselrenToTry+10 );

  if( vesselObserver->GotVessel() )
    {
    std::cerr << "Error: problem getting Vessel out of bounds" << std::endl;
    return EXIT_FAILURE;
    }

  if( !vesselObserver->GotVesselNotAvailableMessage() )
    {
    std::cerr << "Error: failed to get VesselNotAvailableMessage";
    return EXIT_FAILURE;
    }

  std::cout << "Test [DONE]" << std::endl;

  if( vtkLoggerOutput->GetNumberOfErrorMessages()  > 0 )
    {
    return EXIT_FAILURE;
    }
  
  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
