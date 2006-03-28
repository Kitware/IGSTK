/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationReaderTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>
#include "igstkPivotCalibrationReader.h"


#define igstkObserverMacro( name, eventType, payloadType ) \
class name##Observer : public ::itk::Command \
  { \
  public: \
    typedef  name##Observer   Self; \
    typedef  ::itk::Command    Superclass;\
    typedef  ::itk::SmartPointer<Self>  Pointer;\
    itkNewMacro( Self );\
  protected:\
    name##Observer() \
      {\
      m_GotObject = false;\
      }\
    ~name##Observer() {}\
  public:\
      typedef eventType  EventType;\
      void Execute(itk::Object *caller, const itk::EventObject & event)\
        {\
        const itk::Object * constCaller = caller;\
        this->Execute( constCaller, event );\
        }\
      void Execute(const itk::Object *caller, const itk::EventObject & event)\
        {\
        m_GotObject = false;\
        if( EventType().CheckEvent( &event ) )\
          {\
          const EventType * objectEvent = \
                    dynamic_cast< const EventType *>( &event );\
          if( objectEvent )\
            {\
            m_Object = objectEvent->Get();\
            m_GotObject = true;\
            }\
          }\
        }\
        bool Got##name() const\
        {\
        return m_GotObject;\
        }\
      payloadType Get##name() const\
        {\
        return m_Object;\
        }\
  private:\
    payloadType  m_Object;\
    bool m_GotObject;\
};


namespace ToolCalibrationTest
{
igstkObserverMacro(Calibration,::igstk::CalibrationModifiedEvent,::igstk::PivotCalibration::Pointer)
igstkObserverMacro(String,::igstk::StringEvent,std::string)
}


int igstkPivotCalibrationReaderTest( int argc, char * argv[] )
{
  if(argc<2)
    {
    std::cout << "Usage = " << argv[0] << " calibrationFile" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::PivotCalibrationReader::Pointer reader = 
                              igstk::PivotCalibrationReader::New();

  reader->RequestSetFileName(argv[1]);
  reader->RequestReadObject();

  typedef ToolCalibrationTest::CalibrationObserver CalibrationObserverType;
  CalibrationObserverType::Pointer calibrationObserver = CalibrationObserverType::New();

  reader->AddObserver( ::igstk::CalibrationModifiedEvent(), calibrationObserver );
  reader->RequestGetCalibration();

  igstk::PivotCalibration::Pointer calibration = NULL;

  if(calibrationObserver->GotCalibration())
    {
    std::cout << "Got Calibration! " << std::endl;
    calibration = calibrationObserver->GetCalibration();
    }
  else
    {
    std::cout << "No calibration!" << std::endl;
    return EXIT_FAILURE;
    }

  typedef ToolCalibrationTest::StringObserver StringObserverType;
  StringObserverType::Pointer stringObserver = StringObserverType::New();
  calibration->AddObserver( ::igstk::StringEvent(), stringObserver );
  calibration->RequestGetDate();

  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString() << std::endl;
    }
  else
    {
    std::cout << "No date!" << std::endl;
    return EXIT_FAILURE;
    }

  calibration->RequestGetTime();

  if(stringObserver->GotString())
    {
    std::cout << stringObserver->GetString() << std::endl;
    }
  else
    {
    std::cout << "No time!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
