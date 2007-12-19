#ifndef __igstkPivotCalibrationFLTKWidget_h
#define __igstkPivotCalibrationFLTKWidget_h

#include <FL/Fl.H>
#include <FL/Fl_Group.H>

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkTracker.h"
#include "igstkPivotCalibrationNew.h"

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
 *  computation of the pivot calibration. You set the tracker, specific tool and 
 *  number of required transformations using the RequestInitialize() method. 
 *  You can also set a delay before data acquistion starts using the 
 *  RequestSetDelay() method. The class expects the tracker to be in tracking 
 *  state. If this is not the case an error is reported, both on screen and an 
 *  event is generated for interested observers. Once initialized pressing the 
 *  "Calibrate" button will start data acquistion (after a user set delay) and 
 *  perform calibration. If data acqusition fails multiple times (e.g. line 
 *  of sight is blocked for optical tracker) the calibration will fail and the 
 *  approapriate message is displayed. Corresponding events are also generated
 *  for interested observers:
 *  PivotCalibrationNew::InitializationSuccessEvent 
 *  PivotCalibrationNew::InitializationFailureEvent 
 *  PivotCalibrationNew::DataAcquisitionStartEvent
 *  PivotCalibrationNew::DataAcquisitionEvent - containes the percentage of 
 *                                              data acquired
 *  PivotCalibrationNew::DataAcquisitionEndEvent
 *  PivotCalibrationNew::CalibrationSuccessEvent
 *  PivotCalibrationNew::CalibrationFailureEvent
 *  igstk::TransformModifiedEvent - contains the calibration transform
 *  igstk::PointEvent - contains the pivot point
 *  igstk::DoubleTypeEvent - contains the calibration root mean square error
 *
 *  NOTE: DataAcquisitionEvent is a descendent of DoubleTypeEvent so if you
 *        are observing both of them with the same observer you need to carefully
 *        identify them (when checking which event occured check for 
 *        DataAcquisitionEvent and only after that for DoubleTypeEvent, similar
 *        to the situation encountered when catching multiple exceptions from
 *        the same inheritance heirarchy).
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
  PivotCalibrationFLTKWidget( int x, int y, 
                              int w, int h, 
                              const char *label="" );
  /** Destructor for the widget. */
  virtual ~PivotCalibrationFLTKWidget( void );

  /** This method sets the number of transformations required for performing 
   *  the pivot calibration, the tracker information, and the tool information. 
   *  It is assumed that the tracker is already in tracking mode and that the 
   *  tool is connected to the given port and channel. If the initialization 
   *  fails a message box will be displayed.
   *  The method generates two  events: InitializationSuccessEvent and 
   *  InitializationFailureEvent. */
  void RequestInitialize( unsigned int n, igstk::Tracker::Pointer &tracker, 
                          unsigned int toolPort, unsigned int toolChannel = 0 );

  /** This method sets the delay in seconds between the moment the 
   *  "Calibrate" button is pressed and the begining of data acquistion. */
  void RequestSetDelay( unsigned int delayInSeconds );  

  /** Add an observer so that an outside class is notified of events. */
  unsigned long AddObserver( const itk::EventObject & event, 
                            itk::Command *command );
  unsigned long AddObserver( const itk::EventObject & event, 
                             itk::Command *command ) const;

  /** This method needs to be implemented (check with Ziv) 
   * In the meantime we display a warning message. */
  void RequestReset();
 
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
  PivotCalibrationNew::Pointer m_pivotCalibration;
               //delay before data acquistion starts [milliseconds]
  unsigned int m_delay;
              //description of the tool we want to calibrate
  std::string m_currentToolInformation;

  friend class InitializationObserver;

  /** Class that observes the intialization of the pivot calibration class.
   *  If successful it enables the "Calibrate" button in the UI, otherwise
   *  it opens a dialog stating that initialization has failed. */
  class InitializationObserver : public itk::Command
  {
  public:
    typedef InitializationObserver            Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(InitializationObserver, itk::Command)

    void SetParent(PivotCalibrationFLTKWidget *p); 

      /** Invoke this method when an initialization event occurs. */
    void Execute(itk::Object *caller, const itk::EventObject & event); 
    void Execute(const itk::Object *caller, const itk::EventObject & event);

  protected:
    InitializationObserver() : m_parent(NULL) {}
    virtual ~InitializationObserver(){}

  private:
    PivotCalibrationFLTKWidget *m_parent;

    //purposely not implemented
    InitializationObserver(const Self&);
    void operator=(const Self&); 
  };

  /** Class that observes the events generated by the pivot calibration class
   *  during the actual calibration process. */
  class CalibrationObserver : public itk::Command
  {
  public:
    typedef CalibrationObserver              Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(CalibrationObserver, itk::Command)

    void SetParent(PivotCalibrationFLTKWidget *p); 
    void Execute(const itk::Object *caller, const itk::EventObject & event);  
    void Execute(itk::Object *caller, const itk::EventObject & event);

  protected:
    CalibrationObserver();
    virtual ~CalibrationObserver(){}

  private:
    PivotCalibrationFLTKWidget *m_parent;
                //accumulate the calibration information in this stream and
                //then display to the user
    std::ostringstream m_calibrationInformationStream;

    //purposely not implemented
    CalibrationObserver(const Self&);
    void operator=(const Self&); 
  };

};

} // end namespace igstk

#endif //__igstkPivotCalibrationFLTKWidget_h
