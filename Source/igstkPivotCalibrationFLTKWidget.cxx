#include "igstkPivotCalibrationFLTKWidget.h"
#include <strstream>

#include <FL/Fl_Output.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/fl_ask.H>

namespace igstk
{ 

const double PivotCalibrationFLTKWidget::TOOL_DESCRIPTION_X_SCALE = 0.1;
const double PivotCalibrationFLTKWidget::TOOL_DESCRIPTION_Y_SCALE = 0.0746;
const double PivotCalibrationFLTKWidget::TOOL_DESCRIPTION_W_SCALE = 0.8125;
const double PivotCalibrationFLTKWidget::TOOL_DESCRIPTION_H_SCALE = 0.0746;

const double PivotCalibrationFLTKWidget::PROGRESS_X_SCALE = 0.1;
const double PivotCalibrationFLTKWidget::PROGRESS_Y_SCALE = 0.179;
const double PivotCalibrationFLTKWidget::PROGRESS_W_SCALE = 0.8125;
const double PivotCalibrationFLTKWidget::PROGRESS_H_SCALE = 0.0447;

const double PivotCalibrationFLTKWidget::BUTTON_X_SCALE = 0.4;
const double PivotCalibrationFLTKWidget::BUTTON_Y_SCALE = 0.253;
const double PivotCalibrationFLTKWidget::BUTTON_W_SCALE = 0.2;
const double PivotCalibrationFLTKWidget::BUTTON_H_SCALE = 0.089;

const double PivotCalibrationFLTKWidget::OUTPUT_X_SCALE = 0.1;
const double PivotCalibrationFLTKWidget::OUTPUT_Y_SCALE = 0.373;
const double PivotCalibrationFLTKWidget::OUTPUT_W_SCALE = 0.8125;
const double PivotCalibrationFLTKWidget::OUTPUT_H_SCALE = 0.582;


PivotCalibrationFLTKWidget::PivotCalibrationFLTKWidget( int x, int y, 
                                                        int w, int h,
                                                        const char *label ) :
Fl_Group( x, y, w, h, label )
{
          //setup the UI
  this->m_toolDescription = new Fl_Output( 
    static_cast<int>(x+TOOL_DESCRIPTION_X_SCALE*w+0.5),
    static_cast<int>(y+TOOL_DESCRIPTION_Y_SCALE*h+0.5),
    static_cast<int>(TOOL_DESCRIPTION_W_SCALE*w+0.5),
    static_cast<int>(TOOL_DESCRIPTION_H_SCALE*h+0.5) );
  
  this->m_progress = new Fl_Progress( static_cast<int>(x+PROGRESS_X_SCALE*w+0.5),
                                      static_cast<int>(y+PROGRESS_Y_SCALE*h+0.5),
                                      static_cast<int>(PROGRESS_W_SCALE*w+0.5),
                                      static_cast<int>(PROGRESS_H_SCALE*h+0.5) );
  this->m_progress->minimum(0.0);
  this->m_progress->maximum(1.0);
  this->m_progress->selection_color(FL_DARK_BLUE);

  this->m_calibrateButton = new Fl_Button( static_cast<int>(x+BUTTON_X_SCALE*w+0.5),
                                           static_cast<int>(y+BUTTON_Y_SCALE*h+0.5),
                                           static_cast<int>(BUTTON_W_SCALE*w+0.5),
                                           static_cast<int>(BUTTON_H_SCALE*h+0.5),
                                           "Calibrate" );
  this->m_calibrateButton->deactivate();
  this->m_calibrateButton->callback( ( Fl_Callback* ) ( Self::RequestComputeCalibrationCB ), 
                                      this);

  this->m_output = new Fl_Multiline_Output( static_cast<int>(x+OUTPUT_X_SCALE*w+0.5),
                                            static_cast<int>(y+OUTPUT_Y_SCALE*h+0.5),
                                            static_cast<int>(OUTPUT_W_SCALE*w+0.5),
                                            static_cast<int>(OUTPUT_H_SCALE*h+0.5) );
  end();

  m_delay = 0;
              //create the class that actually does all the work
  this->m_pivotCalibration = PivotCalibration::New();

              //add observer for initialization events
  this->m_InitializationObserver = InitializationObserverType::New();
  this->m_InitializationObserver->SetCallbackFunction( 
    this, 
    &PivotCalibrationFLTKWidget::OnInitializationEvent );

  this->m_pivotCalibration->AddObserver( PivotCalibration::InitializationFailureEvent(),
                                         this->m_InitializationObserver );
  this->m_pivotCalibration->AddObserver( PivotCalibration::InitializationSuccessEvent(),
                                         this->m_InitializationObserver );

              //add observer for the events generated during pivot calibration
  this->m_CalibrationObserver = CalibrationObserverType::New();
  this->m_CalibrationObserver->SetCallbackFunction( 
    this, 
    &PivotCalibrationFLTKWidget::OnCalibrationEvent );

  this->m_pivotCalibration->AddObserver( PivotCalibration::DataAcquisitionEvent(),
                                         this->m_CalibrationObserver );
  this->m_pivotCalibration->AddObserver( PivotCalibration::DataAcquisitionEndEvent(),
                                         this->m_CalibrationObserver );
  this->m_pivotCalibration->AddObserver( PivotCalibration::CalibrationSuccessEvent(),
                                         this->m_CalibrationObserver );
  this->m_pivotCalibration->AddObserver( PivotCalibration::CalibrationFailureEvent(),
                                         this->m_CalibrationObserver );
  this->m_pivotCalibration->AddObserver( igstk::CoordinateSystemTransformToEvent(),
                                         this->m_CalibrationObserver );
  this->m_pivotCalibration->AddObserver( igstk::PointEvent(),
                                         this->m_CalibrationObserver );
  this->m_pivotCalibration->AddObserver( igstk::DoubleTypeEvent(),
                                         this->m_CalibrationObserver );

    //settings for the stream that accumulates the calibration information
  this->m_calibrationInformationStream.precision(3);
  this->m_calibrationInformationStream.setf(ios::fixed); 
}


PivotCalibrationFLTKWidget::~PivotCalibrationFLTKWidget()
{
  delete this->m_toolDescription;
  delete this->m_progress;
  delete this->m_calibrateButton;
  delete this->m_output;
}


void 
PivotCalibrationFLTKWidget::RequestInitialize( unsigned int n, 
                                              igstk::TrackerTool::Pointer trackerTool )
{
              //show the description of the current tool on the UI
    const std::string trackerToolIdentifier =
          trackerTool->GetTrackerToolIdentifier();

    this->m_toolDescription->value(trackerToolIdentifier.c_str());

            //try to initialize  
  this->m_pivotCalibration->RequestInitialize( n, trackerTool );   
}


void 
PivotCalibrationFLTKWidget::RequestReset()
{
  Fl::warning("The method RequestReset() is not implemented");
}


void 
PivotCalibrationFLTKWidget::RequestComputeCalibration()
{
  std::ostringstream msg;

  this->m_calibrateButton->deactivate();

  for(unsigned int i=this->m_delay; i>0; i-=1000 )
    {
    msg.str("");
    msg<<"Data acquisition starts in "<<(int)(i/1000)<<" seconds."; 
    this->m_output->value(msg.str().c_str());
    Fl::check();
    PulseGenerator::Sleep(1000);
    }

  this->m_output->value("");
  Fl::check();
  this->m_pivotCalibration->RequestComputeCalibration();
}


void 
PivotCalibrationFLTKWidget::RequestComputeCalibrationCB( Fl_Button *b, 
                                                             void *v )
{ 
  ( ( Self* ) ( b->user_data() ) )->RequestComputeCalibration();
}


void 
PivotCalibrationFLTKWidget::RequestSetDelay( unsigned int delayInSeconds )
{
  this->m_delay = delayInSeconds*1000;
}


unsigned long 
PivotCalibrationFLTKWidget::AddObserver( const itk::EventObject & event, 
                                         itk::Command *command )
{
  return this->m_pivotCalibration->AddObserver( event, command );
}


unsigned long 
PivotCalibrationFLTKWidget::AddObserver( const itk::EventObject & event, 
                                         itk::Command *command ) const
{
  return this->m_pivotCalibration->AddObserver( event, command );
}


void 
PivotCalibrationFLTKWidget::OnInitializationEvent( itk::Object *caller, 
                                                   const itk::EventObject & event )
{
  if( dynamic_cast< const igstk::PivotCalibration::InitializationSuccessEvent * > (&event) ) 
  {            //activate "Calibrate" button 
    this->m_calibrateButton->activate();               
  }
  else if( dynamic_cast< const igstk::PivotCalibration::InitializationFailureEvent * > (&event) ) 
  {
    fl_alert("Failed to initialize pivot calibration.\n Check that tool is valid.");
    fl_beep(FL_BEEP_ERROR);;
  }
}


void 
PivotCalibrationFLTKWidget::OnCalibrationEvent( itk::Object *caller, 
                                                const itk::EventObject & event )
{
  std::ostringstream msg;

  if( const PivotCalibration::DataAcquisitionEvent *evt = 
      dynamic_cast< const PivotCalibration::DataAcquisitionEvent * > (&event) ) 
  {
    this->m_progress->value( evt->Get() );
    Fl::check();
  }
  else if( dynamic_cast< const PivotCalibration::DataAcquisitionEndEvent * > (&event) ) 
  {            
    this->m_progress->value(0.0);
  }
  else if( const PivotCalibration::CalibrationFailureEvent *evt = 
           dynamic_cast< const PivotCalibration::CalibrationFailureEvent * > (&event) ) 
  {
    this->m_calibrateButton->activate();
    this->m_progress->value(0.0);
    msg.str("");
    msg<<"Calibration failed:\n\t"<<evt->Get();
    fl_alert(msg.str().c_str());
    fl_beep(FL_BEEP_ERROR);;
  }
  else if( dynamic_cast< const PivotCalibration::CalibrationSuccessEvent * > (&event) ) 
  {
    this->m_progress->value(0.0);
    this->m_calibrationInformationStream.str("");
    PivotCalibration* calib = dynamic_cast< PivotCalibration *> (caller);
    calib->RequestCalibrationTransform();
  }
  else if( const igstk::CoordinateSystemTransformToEvent *transformEvent =
           dynamic_cast< const igstk::CoordinateSystemTransformToEvent *> (&event) ) 
  {
    igstk::CoordinateSystemTransformToResult result = transformEvent->Get();
    igstk::Transform transform = result.GetTransform();
    igstk::Transform::VersorType v = transform.GetRotation();
    igstk::Transform::VectorType t = transform.GetTranslation();

    this->m_calibrationInformationStream<<"Transform:\n";
    this->m_calibrationInformationStream<<"\t quaternion: ";
    this->m_calibrationInformationStream<<v.GetX()<<"\t"<<v.GetY();
    this->m_calibrationInformationStream<<"\t"<<v.GetZ()<<"\t"<<v.GetW()<<"\n";
    this->m_calibrationInformationStream<<"\t translation: ";
    this->m_calibrationInformationStream<<t[0]<<"\t"<<t[1]<<"\t"<<t[2]<<"\n";
    PivotCalibration* calib = dynamic_cast< PivotCalibration *> (caller);
    calib->RequestPivotPoint();
  }
  else if( const igstk::PointEvent *pntEvt =
           dynamic_cast< const igstk::PointEvent *> (&event) ) 
  {
    igstk::EventHelperType::PointType pnt = pntEvt->Get();
    
    this->m_calibrationInformationStream<<"Pivot point: ";
    this->m_calibrationInformationStream<<pnt[0]<<"\t"<<pnt[1]<<"\t"<<pnt[2]<<"\n";
    PivotCalibration* calib = dynamic_cast< PivotCalibration *> (caller);
    calib->RequestCalibrationRMSE();
  }
  else if( const igstk::DoubleTypeEvent *rmsEvt =
           dynamic_cast< const igstk::DoubleTypeEvent *> (&event) ) 
  {
    igstk::EventHelperType::DoubleType rmse = rmsEvt->Get();
    
    this->m_calibrationInformationStream<<"RMSE: "<<rmse<<"\n";
    this->m_output->value(this->m_calibrationInformationStream.str().c_str());
    this->m_calibrateButton->activate();
  }
}

} //end namespace igstk
