/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPivotCalibrationFLTKWidget.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPivotCalibrationFLTKWidget_h
#define __igstkPivotCalibrationFLTKWidget_h

#include <FL/Fl.H>
#include <FL/Fl_Group.H>

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkTracker.h"
#include "igstkPivotCalibration.h"

class Fl_Output;
class Fl_Progress;
class Fl_Button;
class Fl_Multiline_Output;


namespace igstk
{
/** \class PivotCalibrationFLTKWidget
 *
 *  \brief This class is a user interface (FLTK-based) for the pivot calibration
 *         class.
 *
 *  This class provides a UI for performing pivot calibration (tool tip 
 *  calibration). The class is responsible for acquisition of tracking data and 
 *  computation of the pivot calibration. You set the specific tool and 
 *  number of required transformations using the RequestInitialize() method. 
 *  You can also set a delay before data acquisition starts using the 
 *  RequestSetDelay() method. The class expects the tracker to be in tracking 
 *  state. Once initialized pressing the 
 *  "Calibrate" button will start data acquisition (after a user set delay) and 
 *  perform calibration. Corresponding events are also generated
 *  for interested observers:
 *  PivotCalibration::InitializationSuccessEvent 
 *  PivotCalibration::InitializationFailureEvent 
 *  PivotCalibration::DataAcquisitionStartEvent
 *  PivotCalibration::DataAcquisitionEvent - contains the percentage of 
 *                                              data acquired
 *  PivotCalibration::DataAcquisitionEndEvent
 *  PivotCalibration::CalibrationSuccessEvent
 *  PivotCalibration::CalibrationFailureEvent
 *  igstk::CoordinateSystemTransformToEvent - contains the calibration transform
 *  igstk::PointEvent - contains the pivot point
 *  igstk::DoubleTypeEvent - contains the calibration root mean square error
 *
 *  NOTE: DataAcquisitionEvent is a descendant of DoubleTypeEvent so if you
 *        are observing both of them with the same observer you need to 
 *        carefully identify them (when checking which event occurred check for 
 *        DataAcquisitionEvent and only after that for DoubleTypeEvent, similar
 *        to the situation encountered when catching multiple exceptions from
 *        the same inheritance hierarchy).
 */
class PivotCalibrationFLTKWidget : public Fl_Group
{
public:
  
  typedef PivotCalibrationFLTKWidget    Self;
  typedef Fl_Group                      Superclass;

  igstkTypeMacro( PivotCalibrationFLTKWidget, Fl_Group );
  
  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

  /** Constructor creating the widget at the specified x,y location with the
   *  given width, w, and height, h. An optional label argument for the widget
   *  can also be given.
   */
  PivotCalibrationFLTKWidget( int xPos, int yPos,
                              int width, int height,
                              const char *l="" );
  /** Destructor for the widget. */
  virtual ~PivotCalibrationFLTKWidget( void );

  /** This method sets the number of transformations required for performing 
   *  the pivot calibration, and the tool information. 
   *  It is assumed that the tracker is already in tracking mode. 
   *  If the initialization fails a message box will be displayed.
   *  The method generates two events: InitializationSuccessEvent and 
   *  InitializationFailureEvent. */
  void RequestInitialize( unsigned int n, 
    igstk::TrackerTool::Pointer trackerTool ); 

  /** This method sets the delay in seconds between the moment the 
   *  "Calibrate" button is pressed and the beginning of data acquisition. */
  void RequestSetDelay( unsigned int delayInSeconds );  

  /** This method is used to request the calibration transformation.
   *  The method should only be invoked after a successful calibration. 
   *  It generates two events: CoordinateSystemTransformToEvent, and 
   *  TransformNotAvailableEvent, respectively denoting that a calibration 
   *  transform is and isn't available. */
  void RequestCalibrationTransform();

  /** This method is used to request the pivot point, given in the coordinate 
   *  system in which the user supplied transforms were given. It generates two 
   *  events: PointEvent, and InvalidRequestErrorEvent, respectively denoting 
   *  that the pivot point is and isn't available. */
  void RequestPivotPoint();

  /** This method is used to request the Root Mean Square Error (RMSE) of the 
   *  overdetermined equation system used to perform pivot calibration. It 
   *  generates two events: DoubleTypeEvent, and InvalidRequestErrorEvent, 
   *  respectively denoting that the RMSE is and isn't available.
   *  \sa PivotCalibrationAlgorithm */
  void RequestCalibrationRMSE();


  /** Add an observer so that an outside class is notified of events. */
  unsigned long AddObserver( const itk::EventObject & event, 
                            itk::Command *command );
  unsigned long AddObserver( const itk::EventObject & event, 
                             itk::Command *command ) const;
  void RemoveObserver( unsigned long observerID );

private:

  Fl_Output *m_toolDescription;
  Fl_Progress *m_progress;
  Fl_Button *m_calibrateButton;
  Fl_Multiline_Output *m_output;

  static const double TOOL_DESCRIPTION_X_SCALE;
  static const double TOOL_DESCRIPTION_Y_SCALE;
  static const double TOOL_DESCRIPTION_W_SCALE;
  static const double TOOL_DESCRIPTION_H_SCALE;

  static const double PROGRESS_X_SCALE;
  static const double PROGRESS_Y_SCALE;
  static const double PROGRESS_W_SCALE;
  static const double PROGRESS_H_SCALE;

  static const double BUTTON_X_SCALE;
  static const double BUTTON_Y_SCALE;
  static const double BUTTON_W_SCALE;
  static const double BUTTON_H_SCALE;

  static const double OUTPUT_X_SCALE;
  static const double OUTPUT_Y_SCALE;
  static const double OUTPUT_W_SCALE;
  static const double OUTPUT_H_SCALE;

  /** This is the callback invoked when the "Calibrate" button is pressed.*/
  static void RequestComputeCalibrationCB(Fl_Button *b, 
                                          void *v);
  inline void RequestComputeCalibration();
  //the igstk class that actually does everything
  PivotCalibration::Pointer m_pivotCalibration;
  //delay before data acquisition starts [milliseconds]
  unsigned int m_delay;
  //description of the tool we want to calibrate
  std::string m_currentToolInformation;

  //accumulate the calibration information in this stream and
  //then display to the user
  std::ostringstream m_calibrationInformationStream;

  //pivot calibration initialization observer
  typedef itk::MemberCommand<PivotCalibrationFLTKWidget>   
    InitializationObserverType;
  void OnInitializationEvent( itk::Object *caller, 
                              const itk::EventObject & event );
  InitializationObserverType::Pointer m_InitializationObserver;  

  //pivot calibration events (start, progress, end) 
  //observer
  typedef itk::MemberCommand<PivotCalibrationFLTKWidget>   
    CalibrationObserverType;
  void OnCalibrationEvent( itk::Object *caller, 
                           const itk::EventObject & event );
  CalibrationObserverType::Pointer m_CalibrationObserver;  

  //pivot calibration get transform observer
  igstkObserverMacro( TransformTo, 
                      igstk::CoordinateSystemTransformToEvent, 
                      igstk::CoordinateSystemTransformToResult )
  TransformToObserver::Pointer m_TransformToObserver;

  //pivot calibration get pivot point observer
  igstkObserverMacro( PivotPoint, 
                      igstk::PointEvent, 
                      igstk::EventHelperType::PointType )
  PivotPointObserver::Pointer m_PivotPointObserver;

  //pivot calibration get RMSE observer
  igstkObserverMacro( RMSE, 
                      igstk::DoubleTypeEvent, 
                      igstk::EventHelperType::DoubleType )
  RMSEObserver::Pointer m_RMSEObserver;

};

} // end namespace igstk

#endif //__igstkPivotCalibrationFLTKWidget_h
