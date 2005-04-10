/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPulseGeneratorTest.cxx
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

#include "igstkPulseGenerator.h"
#include "itkCommand.h"
#include "igstkEvents.h"

namespace PulseGeneratorTest
{
  class PulseObserver : public ::itk::Command 
  {
  public:
    typedef  PulseObserver   Self;
    typedef  ::itk::Command    Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );
    typedef ::igstk::PulseGenerator  PulseGeneratorType;
  protected:
    PulseObserver() 
      {
      m_PulseCounter = 0;
      }
  public:

    void Execute(const itk::Object * caller, const itk::EventObject & event)
      {
      std::cerr << "Const Executed should not be called"  << std::endl;
      }

    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
        
        PulseGeneratorType * generator = 
          dynamic_cast< PulseGeneratorType * >( caller );
        
        if( ::igstk::PulseEvent().CheckEvent( &event ) )
          {
          std::cout << "pulse" << std::endl;
          }
        
        m_PulseCounter++;
        
        if( m_PulseCounter > 1000 )
          {
          generator->RequestStop();
          return;
          }

        if( m_PulseCounter % 100 )
          {
          std::cout << "Pulse # " << m_PulseCounter << std::endl;
          return;
          }
      }
  private:
    unsigned long m_PulseCounter;
  };



}

int igstkPulseGeneratorTest( int, char * [] )
{
    typedef igstk::PulseGenerator  PulseGeneratorType;
    
    PulseGeneratorType::Pointer pulseGenerator = PulseGeneratorType::New();

    typedef PulseGeneratorTest::PulseObserver  ObserverType;

    ObserverType::Pointer observer = ObserverType::New();

    pulseGenerator->AddObserver( igstk::PulseEvent(), observer );
    
    pulseGenerator->RequestSetFrequency( 10 );  // 10 Hz
    pulseGenerator->RequestStart();  

    std::cout << "End of the pulses" << std::endl;

    std::cout << "Test PASSED !" << std::endl;

    return EXIT_SUCCESS;
}


