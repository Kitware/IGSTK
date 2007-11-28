/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerHardwareTestHelper.h
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
//  Warning about: identifier was truncated to '255' characters in the 
//  debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkEvents.h"
#include "igstkCylinderObject.h"
#include "igstkTracker.h"
#include "igstkEllipsoidObject.h"
#include "igstkPoint3D.h"
#include "igstkTransform.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"


namespace igstk {
  
class TrackerHardwareTestHelper : public Object
{
public:
  
  typedef  TrackerHardwareTestHelper      Self;
  typedef  Object                         Superclass;
  typedef  ::itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );

  typedef ::igstk::Tracker                TrackerType;
  typedef ::igstk::TrackerTool            TrackerToolType;
  typedef ::igstk::Transform              TransformType;
  typedef TransformType::VectorType       VectorType;
  typedef TransformType::VersorType       VersorType;

  typedef std::vector< VectorType >       VectorArrayType;
  typedef std::vector< VersorType >       VersorArrayType;

  typedef igstk::Object::LoggerType       LoggerType;
  typedef ::itk::StdStreamLogOutput       LogOutputType;

  typedef ::itk::ReceptorMemberCommand< Self >   ReceptorObserverType;

  typedef enum
    {
    GrabbingPosition,
    GrabbingOrientation
    }                                      GrabbingType;

protected:
  TrackerHardwareTestHelper() 
    {
    this->m_SpatialObject = EllipsoidObject::New();
    this->m_FinalStatus = EXIT_FAILURE;
    this->m_Logger = LoggerType::New();
    this->m_LogOutput = LogOutputType::New();

    this->m_Logger->SetPriorityLevel( LoggerType::DEBUG );

    this->m_TransformObserver = ReceptorObserverType::New();
    this->m_TransformObserver->SetCallbackFunction( this,
                                          & Self::CallbackModifiedEvent );
    this->m_SpatialObject->AddObserver( 
      TransformModifiedEvent(), this->m_TransformObserver );

    this->m_NumberOfPositionsToTest = 16;
    this->m_NumberOfOrientationsToTest = 12;
    this->m_GrabbingType = GrabbingPosition;
    }

public:

  void SetTracker( TrackerType * tracker )
    {
    this->m_Tracker = tracker;
    }

  void SetTrackerTool( TrackerToolType * trackerTool )
    {
    this->m_TrackerTool = trackerTool;
    }


  void SetBaselineFilename( const char * filename )
    {
    this->m_BaseLineFilename = filename;
    }

  void SetLogOutputFilename( const char * filename )
    {
    this->m_LogOutputFilename = filename;
    }

  unsigned int GetNumberOfPositionsToTest() const
    {
    return this->m_NumberOfPositionsToTest;
    }

  unsigned int GetNumberOfOrientationsToTest() const
    {
    return this->m_NumberOfOrientationsToTest;
    }

  unsigned int GetNumberOfPositionsCollected() const
    {
    return this->m_NumberOfPositionsCollected;
    }

  unsigned int GetNumberOfOrientationsCollected() const
    {
    return this->m_NumberOfOrientationsCollected;
    }

  void Initialize()
    {
    igstk::RealTimeClock::Initialize();

    const unsigned int toolNumber = 0;

    this->m_TrackerTool->RequestAttachSpatialObject( this->m_SpatialObject );
    this->m_Tracker->RequestAddTool( this->m_TrackerTool );

    //  PREVIOUS: DEPRECATED METHOD : DELETE FROM HERE : FIXME
    //  this->m_Tracker->AttachObjectToTrackerTool( 
    //  toolPort, 
    //  toolNumber, 
    //  this->m_SpatialObject );
    //  PREVIOUS: DEPRECATED METHOD : DELETE FROM HERE : FIXME

    this->m_LogFile.open( this->m_LogOutputFilename.c_str() );
    if( !this->m_LogFile.fail() )
      {
      this->m_LogOutput->SetStream( this->m_LogFile );
      }
    else
      {
      std::cerr << "Problem opening Log file, using cerr instead " 
        << std::endl;
      this->m_LogOutput->SetStream( std::cerr );
      }
    this->m_Logger->AddLogOutput( this->m_LogOutput );

    this->m_Tracker->SetLogger( this->m_Logger );

    this->m_NumberOfPositionsCollected = 0;
    this->m_NumberOfOrientationsCollected = 0;
   
    this->m_VectorArray.clear();
    this->m_VersorArray.clear();

    this->m_Tracker->RequestOpen();
    this->m_Tracker->RequestInitialize();
    this->m_Tracker->RequestStartTracking();
    }

  void GrabOnePosition()
    {
    this->m_GrabbingType = GrabbingPosition;
    std::cout << "Position # " << this->m_NumberOfPositionsCollected;
    std::cout << " of " << this->m_NumberOfPositionsToTest << "  press ENTER when ready" << std::endl;
    char pressKey;
    std::cin >> pressKey;
    this->m_SpatialObject->RequestGetTransform();  
    }

  void GrabOneOrientation()
    {
    this->m_GrabbingType = GrabbingOrientation;
    std::cout << "Orientation # " << this->m_NumberOfOrientationsCollected;
    std::cout << " of " << this->m_NumberOfOrientationsToTest << "  press ENTER when ready" << std::endl;
    char pressKey;
    std::cin >> pressKey;
    this->m_SpatialObject->RequestGetTransform();  
    }

  void Finalize()
    {
    this->m_Tracker->RequestReset();
    this->m_Tracker->RequestStopTracking();
    this->m_Tracker->RequestClose();
    }

  void Report()
    {
    }

  int GetFinalStatus() const 
    {
    return this->m_FinalStatus;
    }

private:
  
  void CallbackModifiedEvent( const ::itk::EventObject & eventvar )
    {
    const TransformModifiedEvent * realEvent =
      dynamic_cast < const TransformModifiedEvent * > ( &eventvar ); 

    if( realEvent )
      {

      this->m_SpatialObjectTransform = realEvent->Get();

      if( this->m_GrabbingType == GrabbingPosition )
        {
        VectorType vector = this->m_SpatialObjectTransform.GetTranslation();
        this->m_VectorArray.push_back( vector );
        this->m_NumberOfPositionsCollected++;
        }

      if( this->m_GrabbingType == GrabbingOrientation )
        {
        VersorType versor = this->m_SpatialObjectTransform.GetRotation();
        this->m_VersorArray.push_back( versor );
        this->m_NumberOfOrientationsCollected++;
        }

      }
    }

  int                             m_FinalStatus;

  TrackerType::Pointer            m_Tracker;
  TrackerTool::Pointer            m_TrackerTool;
  EllipsoidObject::Pointer        m_SpatialObject;

  std::string                     m_BaseLineFilename;
  std::string                     m_LogOutputFilename;
  LoggerType::Pointer             m_Logger;
  LogOutputType::Pointer          m_LogOutput;
  std::ofstream                   m_LogFile;

  unsigned int                    m_NumberOfPositionsToTest;
  unsigned int                    m_NumberOfOrientationsToTest;
  
  unsigned int                    m_NumberOfPositionsCollected;
  unsigned int                    m_NumberOfOrientationsCollected;
  
  VectorArrayType                 m_VectorArray;
  VersorArrayType                 m_VersorArray;

  TransformType                   m_SpatialObjectTransform;

  ReceptorObserverType::Pointer   m_TransformObserver;
  
  GrabbingType                    m_GrabbingType;
};

}

