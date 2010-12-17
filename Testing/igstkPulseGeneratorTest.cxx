/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPulseGeneratorTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkPulseGenerator.h"
#include "itkCommand.h"
#include "igstkEvents.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Value_Output.H>

namespace PulseGeneratorTest
{
  
class PulseObserver : public ::itk::Command 
{
public:
  typedef  PulseObserver              Self;
  typedef  ::itk::Command             Superclass;
  typedef  ::itk::SmartPointer<Self>  Pointer;
  
  itkNewMacro( Self );
  
  typedef ::igstk::PulseGenerator  PulseGeneratorType;
  void SetForm( Fl_Window * form )
    {
    m_Form = form;
    }
  void SetCounter( Fl_Value_Output * valueOutput )
    {
    m_ValueOutput = valueOutput;
    }
  void SetEndFlag( bool * end )
    {
    m_End = end;
    }
protected:
 
  PulseObserver() 
    {
    m_PulseCounter = 0;
    m_Form = 0;
    m_ValueOutput = 0;
    }
public:

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr << "Execute( const * ) should not be called" << std::endl;
    }
      
  void Execute(itk::Object *caller, const itk::EventObject & event)
    { 
    PulseGeneratorType * generator = 
        dynamic_cast< PulseGeneratorType * >( caller );
      
    if( ::igstk::PulseEvent().CheckEvent( &event ) )
      {
      m_PulseCounter++;
      m_ValueOutput->value( m_PulseCounter );
  
      if( m_PulseCounter > 100 )
        {
        std::cout << m_PulseCounter << std::endl;
        generator->RequestStop();
        *m_End = true;
        return;
        }
      }
    }

private:
  unsigned long       m_PulseCounter;
  Fl_Window *         m_Form;
  Fl_Value_Output *   m_ValueOutput;
  bool *              m_End;
};
}

int igstkPulseGeneratorTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();
  typedef igstk::PulseGenerator  PulseGeneratorType;
    
  PulseGeneratorType::Pointer pulseGenerator = PulseGeneratorType::New();

  typedef PulseGeneratorTest::PulseObserver  ObserverType;

  bool bEnd = false;
  ObserverType::Pointer observer = ObserverType::New();

  typedef igstk::Object::LoggerType   LoggerType;
  LoggerType::Pointer logger = LoggerType::New();
  itk::StdStreamLogOutput::Pointer logOutput = itk::StdStreamLogOutput::New();
  logOutput->SetStream( std::cout );

  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  pulseGenerator->SetLogger( logger );

  pulseGenerator->AddObserver( igstk::PulseEvent(), observer );
    
  // On purpose set invalid frequency values,
  // just to check code coverage of error conditions
  pulseGenerator->RequestSetFrequency( 20000 );  // Frequency too high
  pulseGenerator->RequestSetFrequency(   -10 );  // Negative frequency 
                                                   // is invalid

  // Now set up a valid frequency rate value
  pulseGenerator->RequestSetFrequency( 100 );  // 100 Hz

  pulseGenerator->RequestStart();  

  std::cout << pulseGenerator << std::endl;

  // Create an FLTK minimal GUI
  Fl_Window * form = new Fl_Window(300,100,"Pulse Generator Test");
  Fl_Value_Output * counter = 
                         new Fl_Value_Output(150,20,80,20,"Number of Pulses");
  form->end();
  // End of the GUI creation

  form->show();
    
  observer->SetForm( form );
  observer->SetCounter( counter );
  observer->SetEndFlag( &bEnd );

  while(1)
    {
    Fl::wait(0.0001);
    igstk::PulseGenerator::CheckTimeouts();
    if( bEnd )
      {
      break;
      }
    }

  std::cout << "End of the pulses" << std::endl;
  std::cout << "Test PASSED !" << std::endl;
  
  delete form;
  
  return EXIT_SUCCESS;
}
