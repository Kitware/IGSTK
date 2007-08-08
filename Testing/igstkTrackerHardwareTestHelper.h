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

  typedef igstk::Point3D                  PointType;
  typedef std::vector< PointType >        PointsArrayType;

  typedef itk::Logger                     LoggerType; 
  typedef itk::StdStreamLogOutput         LogOutputType;

protected:
  TrackerHardwareTestHelper() 
    {
    this->m_SpatialObject = EllipsoidObject::New();
    this->m_FinalStatus = EXIT_FAILURE;
    this->m_Logger = LoggerType::New();
    this->m_LogOutput = LogOutputType::New();

    this->m_Logger->SetPriorityLevel( LoggerType::DEBUG );
    this->m_Tracker->SetLogger( this->m_Logger );

    this->m_NumberOfPositions = 16;
    this->m_NumberOfOrientations = 12;
    }

public:

  void SetTracker( Tracker * tracker )
    {
    this->m_Tracker = tracker;
    }

  void SetBaselineFilename( const char * filename )
    {
    this->m_BaseLineFilename = filename;
    }

  void SetLogOutputFilename( const char * filename )
    {
    this->m_LogOutputFilename = filename;
    }

  void Initialize()
    {
    igstk::RealTimeClock::Initialize();

    const unsigned int toolPort = 0;
    const unsigned int toolNumber = 0;

    this->m_Tracker->AttachObjectToTrackerTool( 
      toolPort, 
      toolNumber, 
      this->m_SpatialObject );

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

    this->m_Tracker->RequestOpen();
    this->m_Tracker->RequestInitialize();

    this->m_PositionsArray.resize( this->m_NumberOfPositions );
    }

   void Execute()
    {
    this->m_Tracker->RequestStartTracking();

    for( unsigned int positionItr = 0; 
         positionItr < this->m_NumberOfPositions; positionItr++ )
      {
      this->GrabOneMeasurement( positionItr );
      }

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
  
  int GrabOneMeasurement( unsigned int positionItr )
    {
    PointType point;
    this->m_PositionsArray[ positionItr ] = point;
    return EXIT_SUCCESS;
    }

  int                         m_FinalStatus;

  Tracker::Pointer            m_Tracker;
  EllipsoidObject::Pointer    m_SpatialObject;

  std::string                 m_BaseLineFilename;
  std::string                 m_LogOutputFilename;
  LoggerType::Pointer         m_Logger;
  LogOutputType::Pointer      m_LogOutput;
  std::ofstream               m_LogFile;

  unsigned int                m_NumberOfPositions;
  unsigned int                m_NumberOfOrientations;
  
  PointsArrayType             m_PositionsArray;
};

}

