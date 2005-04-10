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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Value_Output.H>

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
    void SetForm( Fl_Window * form )
      {
      m_Form = form ;
      }
    void SetCounter( Fl_Value_Output * valueOutput )
      {
      m_ValueOutput = valueOutput;
      }
  protected:
    PulseObserver() 
      {
      m_PulseCounter = 0;
      m_Form = 0;
      m_ValueOutput = 0;
      }
  public:

    void Execute(const itk::Object *caller, const itk::EventObject & event)
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
          generator->RequestStop();
          if( m_Form )
            {
            m_Form->hide(); // close the FLTK window
            }
          return;
          }
        }
      }
  private:
    unsigned long       m_PulseCounter;
    Fl_Window *         m_Form;
    Fl_Value_Output *   m_ValueOutput;
  };



}

int igstkPulseGeneratorTest( int, char * [] )
{
    typedef igstk::PulseGenerator  PulseGeneratorType;
    
    PulseGeneratorType::Pointer pulseGenerator = PulseGeneratorType::New();

    typedef PulseGeneratorTest::PulseObserver  ObserverType;

    ObserverType::Pointer observer = ObserverType::New();

    pulseGenerator->AddObserver( igstk::PulseEvent(), observer );
    
    pulseGenerator->RequestSetFrequency( 100 );  // 10 Hz
    pulseGenerator->RequestStart();  

    // Create an FLTK minimal GUI
    Fl_Window * form = new Fl_Window(300,100,"Pulse Generator Test");
    Fl_Value_Output * counter = new Fl_Value_Output(150,20,80,20,"Number of Pulses");
    form->end();
    // End of the GUI creation

    form->show();
    
    observer->SetForm( form );
    observer->SetCounter( counter );

    Fl::run();

    std::cout << "End of the pulses" << std::endl;

    std::cout << "Test PASSED !" << std::endl;

    return EXIT_SUCCESS;
}


