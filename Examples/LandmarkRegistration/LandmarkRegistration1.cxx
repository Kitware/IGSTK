/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LandmarkRegistration1.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters in the \
//debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

// BeginLatex
// 
// This example illustrates how to use igstk's landmark registration 
// component to determine a rigid body transformation parameters between 
// image and patient coordinate system.
//
// EndLatex

#include <iostream>

// BeginLatex
// 
// To use the registration component, the header file for 
// \doxygen{Landmark3DRegistration} will be added.
//
// EndLatex

// BeginCodeSnippet
#include "igstkLandmark3DRegistration.h"
// EndCodeSnippet
 

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"

// BeginLatex
// 
// Transform parameters are returned to the application using loaded events. 
// To handle these events, \doxygen{Events} and \doxygen{Transform} header 
// files are needed.
//
// EndLatex

// BeginCodeSnippet
#include "igstkEvents.h"
#include "igstkTransform.h"
// EndCodeSnippet



// BeginLatex
// 
// To fully  utilize the registration component, callbacks need to be set up 
// to  observers events that could be thrown by the registration component.  
//  For this purpose, ITK command class is used to derive a callback class .
// The ITK command class implements subject/observer (command design) pattern.
//  A subject notifies an observer by running the \code{Execute } method of 
// the derived callback class . For example a callback class to observe error 
// in transform computation is defined as follows.
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
  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {

    }
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    std::cerr<<"Error in transform computation"<<std::endl;
    }
protected:
  Landmark3DRegistrationErrorCallback()   { };

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
  void Execute(const itk::Object *caller, const itk::EventObject & event)
    {

    }
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    std::cerr<<"Invalid input request!!"<<std::endl;
    }
protected:
  Landmark3DRegistrationInvalidRequestCallback()   { };

private:
};

// BeginLatex
// 
// Similarly, a callback class needs to be defined to observe the 
// \doxygen{TransformModified} event. This event is loaded with the 
// transform parameters computed by the registration component.
//
// EndLatex

// BeginCodeSnippet
class Landmark3DRegistrationGetTransformCallback: public itk::Command
{
public:
  typedef Landmark3DRegistrationGetTransformCallback    Self;
  typedef itk::SmartPointer<Self>                       Pointer;
  typedef itk::Command                                  Superclass;
  itkNewMacro(Self);

  typedef igstk::TransformModifiedEvent TransformModifiedEventType;
  
  void Execute( const itk::Object *caller, const itk::EventObject & event )
    {
    }
 
  void Execute( itk::Object *caller, const itk::EventObject & event )
    {
    std::cout<< " TransformEvent is thrown" << std::endl;
                dynamic_cast < const TransformModifiedEventType* > ( &event );
    const TransformModifiedEventType * transformEvent =
                dynamic_cast < const TransformModifiedEventType* > ( &event );
    m_Transform = transformEvent->Get();
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
    };
private:
  bool m_EventReceived;
  igstk::Transform m_Transform;
};
// EndCodeSnippet



// BeginLatex
// 
// After defining the helper classes, the main function implementation 
// is started.
//
// EndLatex

// BeginCodeSnippet
int main( int argv, char * argc[] )
{
// EndCodeSnippet


  igstk::RealTimeClock::Initialize();



// BeginLatex
//
// All the necessary data types are defined.
// 
// EndLatex

// BeginCodeSnippet
  typedef itk::Logger                   LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
    
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
// The registration component is instantiated as follows
// 
// EndLatex

// BeginCodeSnippet
  Landmark3DRegistrationType::Pointer landmarkRegister = 
                                        Landmark3DRegistrationType::New();
// EndCodeSnippet
 


// BeginLatex
// 
// The landmark containers that hold the landmark image and tracker coordinates
// are instantiated.
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
// Error event callback objects are instantiated and added to the observer list
// of the registration component.
//
// EndLatex

// BeginCodeSnippet
  Landmark3DRegistrationInvalidRequestCallback::Pointer 
                  lrcb = Landmark3DRegistrationInvalidRequestCallback::New();
    
  typedef igstk::Landmark3DRegistration::InvalidRequestErrorEvent  
                                                         InvalidRequestEvent;
  landmarkRegister->AddObserver( InvalidRequestEvent(), lrcb );

  Landmark3DRegistrationErrorCallback::Pointer ecb = 
                  Landmark3DRegistrationErrorCallback::New();
  typedef igstk::Landmark3DRegistration::TransformComputationFailureEvent 
                                                     ComputationFailureEvent;
  landmarkRegister->AddObserver( ComputationFailureEvent(), ecb );

// EndCodeSnippet

// BeginLatex
// 
// A logger can be connected to the registration component for 
// debugging purpose as follows
//
// EndLatex



// BeginCodeSnippet 
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );
  landmarkRegister->SetLogger( logger );
// EndCodeSnippet
   

  
// BeginLatex
//
// Next, landmark points are added to the image and tracker containers. 
// The state machine of this registration component is designed in such a 
// way that image and tracker coordinates corresponding to
// each landmark are added consecutively. This scheme was selected to prevent 
// a mismatch in landmark correspondence that could occur when all landmarks 
// image coordinates are recorded followed by the tracker coordinates. 
// This design choice is consistent with the philosophy of igstk i.e safety
// by design.
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

//EndCodeSnippet

  // BeginLatex
  // 
  // More landmarks could be added for transform computation.  
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
  // After adding all the landmark coordinates, the transform computation is 
  // requested as follows
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
  // To access the tranform parameters, a GetTransform callback is instantiated
  // to observe the transform event as follows. 
  // 
  // EndLatex

  // BeginCodeSnippet
  Landmark3DRegistrationGetTransformCallback::Pointer lrtcb =
                            Landmark3DRegistrationGetTransformCallback::New();
  landmarkRegister->AddObserver( igstk::TransformModifiedEvent(), lrtcb );
  //EndCodeSnippet


  // BeginLatex
  //
  // To request the registration component throw an event loaded with transform
  // parameters, a \code{RequestGetTransform} function is invoked as follows.
  // 
  // EndLatex

  // BeginCodeSnippet
  landmarkRegister->RequestGetTransform();
  std::cout << "Transform " << transform << std::cout;
  // EndCodeSnippet
    
  return EXIT_SUCCESS;
}
