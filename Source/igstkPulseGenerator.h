/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPulseGenerator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_PulseGenerator_h_
#define __igstk_PulseGenerator_h_


#include "itkObject.h"
#include "itkLogger.h"

#include "igstkMacros.h"
#include "igstkStateMachine.h"


namespace igstk
{
/** \class PulseGenerator
 *
    \brief This class produces ClockTickEvents at a user-defined frequency.

    The PulseGenerator is used for generating ClockTickEvents at regular
    intervals. The pulses are used by classes that need to perform tasks at
    user-defined frequencies. For example the View classes use the
    PulseGenerator in order to refresh a predefined frame rate. The precision
    of the time interval between the pulses is limited by the underlying time
    functions of the platform. In most cases you should not expect precision
    below the millisecond range. 

*/

class PulseGenerator  : public itk::Object
{
 
public: 

  typedef PulseGenerator Self; 
  typedef itk::Object Superclass; 
  typedef itk::SmartPointer< Self > Pointer; 
  typedef itk::SmartPointer< const Self > ConstPointer; 

  /** Method for defining the name of the class */ 
  igstkTypeMacro(PulseGenerator, Object); 

  /** Method for creation through the object factory */ 
  igstkNewMacro(Self); 

  /** Frequency at which the pulses will be generated. This frequency cannot be
   * guarranted. Users should verify experimentally whether the pulse generator
   * can actually behave at that frequency at run time. */
  void RequestSetFrequency( double frequency );

  /** Request to start the generation of pulses at the defined frequency. This
   * is only a request. It may or may not be honored depending on the current
   * state of the StateMachine.  */
  void RequestStart();

  /** Request to stop generating pulses. This is only a request. It may or may
   * not be honored depending on the current state of the StateMachine. */
  void RequestStop();

  /** Return the value set for the frequency of this pulse generator */
  igstkGetMacro( Frequency, double );
      
  /** Logger class */
  typedef itk::Logger                  LoggerType;

  /** The SetLogger method is used to attach a logger to this object for
   * debugging and retrospective analysis purposes. */
  void SetLogger( LoggerType * logger );

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  PulseGenerator(void);

  virtual ~PulseGenerator(void);


private:

  /** Frequency at which the pulses will be generated. This frequency cannot be
   * guarranted. Users should verify experimentally whether the pulse generator
   * can actually behave at that frequency at run time. */
  double          m_Frequency;
  double          m_FrequencyToBeSet;
  double          m_Period; // helper varable = 1 / frequency

  /** Inputs to the State Machine */
  InputType       m_ValidFrequencyInput;
  InputType       m_InvalidLowFrequencyInput;
  InputType       m_InvalidHighFrequencyInput;
  InputType       m_StopInput;
  InputType       m_StartInput;
  InputType       m_PulseInput;
  InputType       m_EventReturnInput;

  /** States for the State Machine */
  StateType       m_InitialState;
  StateType       m_StoppedState;
  StateType       m_PulsingState;
  StateType       m_WaitingEventReturnState;

  /** Methods to be called only by the State Machine */
  void SetFrequency();
  
  /** Report an error condition. For example unexpected inputs */
  void ReportErrorCondition();

  /** Report a missed pulse. This happens when the frequency of the pulse
   * generator is too high for the time required by the Observers to complete
   * their Execute method.  */
  void ReportMissedPulse();

  /** Value of the frequency above which it is known that the pulses will not
   * be generated correctly */
  static double   m_MaximumFrequency;

  /** Global callback function for the timer */
  static void CallbackTimerGlobal( void * );

  /** Callback function for the timer */
  void CallbackTimer();

  /** Program the timer for triggering a callback. At m_Period seconds from
   * now. */
  void SetTimer();

  /** Send a pulse. This method will notify observers. */
  void EmitPulse();

  /** The Logger instance */
  mutable LoggerType::Pointer      m_Logger;

  /** Get pointer to the Logger */
  LoggerType * GetLogger() const;
};

} // end of namespace igstk

#endif //__igstk_PulseGenerator_h_

