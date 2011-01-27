/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LandmarkRegistration1.cxx
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
//Warning about: identifier was truncated to '255' characters in the 
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

// BeginLatex
// 
// This example illustrates how to use IGSTK's landmark registration 
// component to determine rigid body transformation parameters between 
// an image and the patient coordinate system.
//
// EndLatex

#include <iostream>

// BeginLatex
// 
// To use the registration component, the header file for 
// \doxygen{Landmark3DRegistration} is added:
//
// EndLatex

// BeginCodeSnippet
#include "igstkLandmark3DRegistration.h"
// EndCodeSnippet
 

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"

// BeginLatex
// 
// Transform parameters are returned to the application using loaded events. 
// To handle these events, the following \doxygen{Events} 
// and \doxygen{Transform} header files are needed:
//
// EndLatex

// BeginCodeSnippet
#include "igstkEvents.h"
#include "igstkTransform.h"
#include "igstkCoordinateSystemTransformToResult.h" 
// EndCodeSnippet


// BeginLatex
// 
// To fully  utilize the registration component, callbacks need to be set up 
// to observe events that could be thrown by the registration component.  
// For this purpose, the ITK command class is used to derive a callback class.
// The ITK command class implements a subject/observer (command design) pattern.
// A subject notifies an observer by running the \code{Execute } method of 
// the derived callback class. For example, a callback class meant 
// to observe an error in the transform computation is defined as follows:
// 
// EndLatex

// BeginCodeSnippet
class Landmark3DRegistrationErrorCallback : public itk::Command
{
public:
  typedef Landmark3DRegistrationErrorCallback Self;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::Command                        Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {

    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr<<"Error in transform computation"<<std::endl;
    }
protected:
  Landmark3DRegistrationErrorCallback() {};

private:
};
//EndCodeSnippet


class Landmark3DRegistrationInvalidRequestCallback : public itk::Command
{
public:
  typedef Landmark3DRegistrationInvalidRequestCallback Self;
  typedef itk::SmartPointer<Self>                      Pointer;
  typedef itk::Command                                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
    {

    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
    {
    std::cerr<<"Invalid input request!!"<<std::endl;
    }
protected:
  Landmark3DRegistrationInvalidRequestCallback() {};

private:
};

// BeginLatex
// 
// Similarly, a callback class needs to be defined to observe the
// \doxygen{CoordinateSystemTransformToEvent} event. This event is loaded with
// transform parameters that are computed by the registration component.  The
// commands are as follows:
// EndLatex

// BeginCodeSnippet
class Landmark3DRegistrationGetTransformCallback: public itk::Command
{
public:
  typedef Landmark3DRegistrationGetTransformCallback    Self;
  typedef itk::SmartPointer<Self>                       Pointer;
  typedef itk::Command                                  Superclass;
  itkNewMacro(Self);

  typedef igstk::CoordinateSystemTransformToEvent TransformEventType;
  
  void Execute( const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
    {
    }
 
  void Execute( itk::Object * itkNotUsed(caller), const itk::EventObject & event )
    {
    std::cout<< " TransformEvent is thrown" << std::endl;
    const TransformEventType * transformEvent =
      dynamic_cast < const TransformEventType* > ( &event );

    const igstk::CoordinateSystemTransformToResult transformCarrier =
      transformEvent->Get();

    m_Transform = transformCarrier.GetTransform();

    m_EventReceived = true;
    } 
  bool GetEventReceived()
    {
    return m_EventReceived;
    }
  igstk::Transform GetTransform()
    {
    return m_Transform;
    }  
protected:

  Landmark3DRegistrationGetTransformCallback()   
    {
    m_EventReceived = true;
    }

private:
  bool m_EventReceived;
  igstk::Transform m_Transform;
};
// EndCodeSnippet


// BeginLatex
// 
// For more information on IGSTK events, see Chapter~\ref{Chapter:Events}.
// After the helper classes are defined, the main function implementation 
// is started, as follows:
//
// EndLatex

// BeginCodeSnippet
int main( int , char * [] )
{
// EndCodeSnippet


  igstk::RealTimeClock::Initialize();


// BeginLatex
//
// Next, all the necessary data types are defined:
// 
// EndLatex

// BeginCodeSnippet
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;
    
  typedef igstk::Landmark3DRegistration
                            Landmark3DRegistrationType;
  typedef igstk::Landmark3DRegistration::LandmarkPointContainerType
                            LandmarkPointContainerType;
  typedef igstk::Landmark3DRegistration::LandmarkImagePointType 
                            LandmarkImagePointType;
  typedef igstk::Landmark3DRegistration::LandmarkTrackerPointType
                            LandmarkTrackerPointType;
  typedef Landmark3DRegistrationType::TransformType::OutputVectorType 
                            OutputVectorType;
  typedef igstk::Transform  TransformType;
// EndCodeSnippet


// BeginLatex
// 
// Then, the registration component is instantiated as follows:
// 
// EndLatex

// BeginCodeSnippet
  Landmark3DRegistrationType::Pointer landmarkRegister = 
                                        Landmark3DRegistrationType::New();
// EndCodeSnippet
 

// BeginLatex
// 
// Next, the landmark containers that hold the landmark image and tracker 
// coordinates are instantiated:
//
// EndLatex

// BeginCodeSnippet
  LandmarkPointContainerType  imagePointContainer;
  LandmarkPointContainerType  trackerPointContainer;
// EndCodeSnippet


  LandmarkImagePointType      imagePoint;
  LandmarkTrackerPointType    trackerPoint;

// BeginLatex
// 
// Then, error event callback objects are instantiated and added to the observer
// list of the registration component, as follows:
//
// EndLatex

// BeginCodeSnippet
  Landmark3DRegistrationInvalidRequestCallback::Pointer 
                  lrcb = Landmark3DRegistrationInvalidRequestCallback::New();
    
  typedef igstk::InvalidRequestErrorEvent  InvalidRequestEvent;

  landmarkRegister->AddObserver( InvalidRequestEvent(), lrcb );

  Landmark3DRegistrationErrorCallback::Pointer ecb = 
                  Landmark3DRegistrationErrorCallback::New();
  typedef igstk::Landmark3DRegistration::TransformComputationFailureEvent 
                                                     ComputationFailureEvent;
  landmarkRegister->AddObserver( ComputationFailureEvent(), ecb );

// EndCodeSnippet

// BeginLatex
// 
// A logger can then be connected to the registration component for 
// debugging purpose, as follows:
//
// EndLatex


// BeginCodeSnippet 
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );
  landmarkRegister->SetLogger( logger );
// EndCodeSnippet
   

// BeginLatex
//
// Next, landmark points are added to the image and tracker containers.  The
// state machine of this registration component is designed so that the image
// and tracker coordinates that correspond to each landmark are added
// consecutively. This scheme prevents the mismatch in landmark correspondence
// that could occur when all landmarks image coordinates are recorded first and
// followed by the tracker coordinates.  This design choice is consistent with
// the ``safety by design'' philosophy of IGSTK. The commands are as follows:
//
//EndLatex 

// BeginCodeSnippet
  // Add 1st landmark
  imagePoint[0] =  25.0;
  imagePoint[1] =  1.0;
  imagePoint[2] =  15.0;
  imagePointContainer.push_back(imagePoint);
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);

  trackerPoint[0] =  29.8;
  trackerPoint[1] =  -5.3;
  trackerPoint[2] =  25.0;
  trackerPointContainer.push_back(trackerPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  // Add 2nd landmark
  imagePoint[0] =  15.0;
  imagePoint[1] =  21.0;
  imagePoint[2] =  17.0;
  imagePointContainer.push_back(imagePoint);
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);
    
  trackerPoint[0] =  35.0;
  trackerPoint[1] =  16.5;
  trackerPoint[2] =  27.0;
  trackerPointContainer.push_back(trackerPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  // Add 3d landmark
  imagePoint[0] =  14.0;
  imagePoint[1] =  25.0;
  imagePoint[2] =  11.0;
  imagePointContainer.push_back(imagePoint);
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);

  trackerPoint[0] =  36.8;
  trackerPoint[1] =  20.0;
  trackerPoint[2] =  21.0;
  trackerPointContainer.push_back(trackerPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  // EndCodeSnippet

  // BeginLatex
  // 
  // More landmarks can be added for the transform computation.  
  // 
  // EndLatex

  // Add 4th landmark
  imagePoint[0] =  10.0;
  imagePoint[1] =  11.0;
  imagePoint[2] =  8.0;
  imagePointContainer.push_back(imagePoint);
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);

  trackerPoint[0] =  24.7;
  trackerPoint[1] =  12.0;
  trackerPoint[2] =  18.0;
  trackerPointContainer.push_back(trackerPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  // BeginLatex
  // 
  // After all landmark coordinates are added, the transform computation is 
  // requested as follows:
  // 
  // EndLatex

  // Calculate transform

  // BeginCodeSnippet
  landmarkRegister->RequestComputeTransform();
  // EndCodeSnippet
     
  typedef itk::VersorRigid3DTransform<double>        VersorRigid3DTransformType;
  typedef VersorRigid3DTransformType::ParametersType ParametersType;

  TransformType      transform;
  ParametersType     parameters(6);

  // BeginLatex
  // 
  // To access the transform parameters, a GetTransform callback is instantiated
  // to observe the transform event, as follows: 
  // 
  // EndLatex

  // BeginCodeSnippet
  Landmark3DRegistrationGetTransformCallback::Pointer lrtcb =
    Landmark3DRegistrationGetTransformCallback::New();

  landmarkRegister->AddObserver( 
    igstk::CoordinateSystemTransformToEvent(), lrtcb );
  //EndCodeSnippet


  // BeginLatex
  //
  // To request that the registration component throw an event loaded with 
  // transform parameters, a \code{RequestGetTransform} function is invoked as 
  // follows:
  // 
  // EndLatex

  // BeginCodeSnippet
  landmarkRegister->RequestGetTransformFromTrackerToImage();
  std::cout << "Transform " << transform << std::cout;
  // EndCodeSnippet
    
  return EXIT_SUCCESS;
}
