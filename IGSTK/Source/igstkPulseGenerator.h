/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPulseGenerator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPulseGenerator_h
#define __igstkPulseGenerator_h


#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"


namespace igstk
{
/** \class PulseGenerator
 *
 *  \brief This class produces ClockTickEvents at a user-defined frequency.
 *
 *  The PulseGenerator is used for generating ClockTickEvents at regular
 *  intervals. The pulses are used by classes that need to perform tasks at
 *  user-defined frequencies. For example the View classes use the
 *  PulseGenerator in order to refresh a predefined frame rate. The precision
 *  of the time interval between the pulses is limited by the underlying time
 *  functions of the platform. In most cases you should not expect precision
 *  below the millisecond range. 
 *
 *
 *  \image html  igstkPulseGenerator.png  
 *                                      "PulseGenerator State Machine Diagram"
 *  \image latex igstkPulseGenerator.eps  
 *                                      "PulseGenerator State Machine Diagram" 
 *
 */

class PulseGenerator  : public Object
{
 
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PulseGenerator, Object )

public:

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
      
  /** Method to be called from the main event loop in order to keep the timers
   * counting */
  static void CheckTimeouts();

  /** Sleep for a number of milliseconds */
  static void Sleep( unsigned int milliseconds );

protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  PulseGenerator(void);

  virtual ~PulseGenerator(void);

  /** Print the object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  /** Frequency at which the pulses will be generated. This frequency cannot be
   * guarranted. Users should verify experimentally whether the pulse generator
   * can actually behave at that frequency at run time. */
  double          m_Frequency;
  double          m_FrequencyToBeSet;
  double          m_Period; // helper varable = 1 / frequency

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidFrequency );
  igstkDeclareInputMacro( InvalidLowFrequency );
  igstkDeclareInputMacro( InvalidHighFrequency );
  igstkDeclareInputMacro( Stop );
  igstkDeclareInputMacro( Start );
  igstkDeclareInputMacro( Pulse );
  igstkDeclareInputMacro( EventReturn );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( Stopped );
  igstkDeclareStateMacro( Pulsing );
  igstkDeclareStateMacro( WaitingEventReturn );

  /** Methods to be called only by the State Machine */
  void SetFrequencyProcessing();
  
  /** Report an error condition. For example unexpected inputs */
  void ReportErrorConditionProcessing();

  /** Report a missed pulse. This happens when the frequency of the pulse
   * generator is too high for the time required by the Observers to complete
   * their Execute method.  */
  void ReportMissedPulseProcessing();

  /** Value of the frequency above which it is known that the pulses will not
   * be generated correctly */
  static double   m_MaximumFrequency;

  /** Global callback function for the timer */
  static void CallbackTimerGlobal( void * );

  /** Callback function for the timer */
  void CallbackTimer();

  /** Program the timer for triggering a callback. At m_Period seconds from
   * now. */
  void SetTimerProcessing();

  /** Send a pulse. This method will notify observers. */
  void EmitPulseProcessing();

  /** Stop the generation of pulses by purging the timer. */
  void StopPulsesProcessing();

  /** Null operation for a State Machine transition */
  void NoProcessing();


private:

  /** Types and methods needed for supporting timers */

  struct Timeout 
    {
    double time;
    void (*cb)(void*);
    void* arg;
    Timeout* next;
    };

  typedef void (*TimeoutHandler)(void*);

  static void AddTimeout(double time, TimeoutHandler cb, void* data);

  static void RepeatTimeout(double time, TimeoutHandler cb, void *argp);

  static void RemoveTimeout( TimeoutHandler cb, void *argp);

  static void ElapseTimeouts();

  static void InvokeTimeoutActions();


  static double       m_PreviousClock;

  static Timeout *    m_FirstTimeout;

  static Timeout *    m_FreeTimeout;

  static unsigned int m_FreeTimeoutCount;

  static unsigned int m_NumberOfPulseGenerators;

  mutable itk::SimpleFastMutexLock m_NumberOfPulseGeneratorsLock;

  static char         m_ResetClock;
  
  static double       m_MissedTimeoutBy;

};

} // end of namespace igstk

#endif //__igstk_PulseGenerator_h_
