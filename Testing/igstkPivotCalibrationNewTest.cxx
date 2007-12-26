#include <iostream>
#include "igstkPivotCalibrationNew.h"
#include "igstkTracker.h"
#include <igstkPolarisTracker.h>
#include "igstkSystemInformation.h"

#define PIVOT_CALIBRATION_SIMULATOR_TEST

#ifdef PIVOT_CALIBRATION_SIMULATOR_TEST
  #include "igstkSerialCommunicationSimulator.h"
#else //a tracker is actually in  use
#include "igstkSerialCommunication.h"

//when a tracker is connected and you want to record the data 
//#define RECORD_DATA 

#endif



                       //settings for the serial communication
static const igstk::SerialCommunication::PortNumberType COM_PORT = 
  igstk::SerialCommunication::PortNumber5;
static const igstk::SerialCommunication::ParityType PARITY = 
  igstk::SerialCommunication::NoParity;
static const igstk::SerialCommunication::BaudRateType BAUD_RATE = 
  igstk::SerialCommunication::BaudRate115200;
static const igstk::SerialCommunication::DataBitsType DATA_BITS = 
  igstk::SerialCommunication::DataBits8;
static const igstk::SerialCommunication::StopBitsType STOP_BITS = 
  igstk::SerialCommunication::StopBits1;
static const igstk::SerialCommunication::HandshakeType HAND_SHAKE = 
  igstk::SerialCommunication::HandshakeOff;  
                         //settings for the tool
static const unsigned int TOOL_PORT = 4;
static const unsigned int TOOL_NUMBER_ON_PORT = 0;
static const char *TOOL_ROM_FILE_NAME = "pointer.rom";
                      //settings for the pivot calibration
static const unsigned int NUMBER_OF_TRANSFORMATIONS = 500;
                       //settings for the offline testing
static const char *TRACKER_SIMULATION_FILE_NAME = "pivotCalibrationTrackerData.txt"; 
    
class PivotCalibrationEventObserver : public itk::Command
{
public:
  typedef PivotCalibrationEventObserver            Self;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::Command                             Superclass;
  itkNewMacro(Self);

  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
    const itk::Object * constCaller = caller;
    this->Execute(constCaller, event);
  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    this->m_lastEventName.clear();
    this->m_lastEventName = event.GetEventName();
    if( dynamic_cast< const igstk::InvalidRequestErrorEvent * > (&event) )  
    {
      std::cout<<"Invalid method request, cannot invoke this method in current ";
      std::cout<<"object state."<<std::endl;
    }
    else if( dynamic_cast< const igstk::PivotCalibrationNew::InitializationSuccessEvent * > (&event) ) 
    {
      std::cout<<"Pivot calibration initialization succeeded."<<std::endl;      
    }
    else if( dynamic_cast< const igstk::PivotCalibrationNew::InitializationFailureEvent * > (&event) ) 
    {
      std::cout<<"Pivot calibration initialization failed."<<std::endl;
    }
    else if( dynamic_cast< const igstk::PivotCalibrationNew::DataAcquisitionStartEvent * > (&event) ) 
    {
      std::cout<<"Pivot calibration data acquisition started."<<std::endl;      
    }        //the conditional for DataAcquisitionEvent must appear before the DoubleTypeEvent because
            //it inherits from it, check for the subclass before the superclass
    else if( dynamic_cast< const igstk::PivotCalibrationNew::DataAcquisitionEvent * > (&event) ) 
    {          //write output three places after decimal point    
      int oldPrecision = std::cout.precision(3);
      std::cout.setf(ios::fixed);

      const igstk::PivotCalibrationNew::DataAcquisitionEvent *dataAcquiredEvent = 
        dynamic_cast< const igstk::PivotCalibrationNew::DataAcquisitionEvent * > (&event);
      std::cout<< "Acquired " << dataAcquiredEvent->Get() << " of the required transformations.\r";
                   //return formatting to the previous settings
      std::cout.precision(oldPrecision);
      std::cout.unsetf(ios::fixed);
    }    
    else if( dynamic_cast< const igstk::PivotCalibrationNew::CalibrationSuccessEvent * > (&event) ) 
    {
      std::cout<<"Calibration succeeded."<<std::endl;      
    }
    else if( dynamic_cast< const igstk::PivotCalibrationNew::CalibrationFailureEvent * > (&event) ) 
    {
      const igstk::PivotCalibrationNew::CalibrationFailureEvent *  
        calibrationFaliureEvent = dynamic_cast< const igstk::PivotCalibrationNew::CalibrationFailureEvent * > (&event);
      std::cout<<"Calibration failed: "<<calibrationFaliureEvent->Get()<<std::endl;
    }
    else if( dynamic_cast< const igstk::PointEvent * > (&event) )  
    {
      const igstk::PointEvent *pntEvent =  
        dynamic_cast< const igstk::PointEvent * > (&event);
      std::cout<<"Pivot point is: "<<pntEvent->Get()<<std::endl;
    }
    else if( dynamic_cast< const igstk::TransformModifiedEvent * > (&event) )  
    {
      const igstk::TransformModifiedEvent *transformEvent =  
        dynamic_cast< const igstk::TransformModifiedEvent * > (&event);
      std::cout<<"Calibration transformation is: "<<transformEvent->Get()<<std::endl;
    }
    else if( dynamic_cast< const igstk::DoubleTypeEvent * > (&event) )  
    {
      const igstk::DoubleTypeEvent *doubleEvent =  
        dynamic_cast< const igstk::DoubleTypeEvent * > (&event);
      std::cout<<"Calibration RMSE is: "<<doubleEvent->Get()<<std::endl;
    }
  }
  /** Get the class name of the last event that occured. */
  std::string GetLastEventName() 
  {
    return this->m_lastEventName;
  }
private:
  std::string m_lastEventName;
};


/** Observer all tracking related errors. Not part of the tests done in this
 *  class */
class TrackingErrorObserver : public itk::Command
{
public:
  typedef TrackingErrorObserver            Self;
  typedef ::itk::Command                   Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;

  igstkNewMacro(Self)
  igstkTypeMacro(IGSTKErrorObserver, itk::Command)

  /**
   * When an error occurs in an IGSTK component it will invoke this method with 
   * the appropriate error event object as a parameter. */
   void Execute(itk::Object *caller, const itk::EventObject & event) 
  {
    std::map<std::string,std::string>::iterator it;
    std::string className = event.GetEventName();
    it = this->m_ErrorEvent2ErrorMessage.find(className);           
    if(it != this->m_ErrorEvent2ErrorMessage.end()) {
      this->m_ErrorOccured = true;
      std::cout << (*it).second <<std::endl;
    }
  }

  /**
   * When an error occurs in an IGSTK component it will invoke this method with 
   * the appropriate error event object as a parameter. */
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {
    const itk::Object * constCaller = caller;
    this->Execute(constCaller, event);
  }

  /**
   * Clear the current error. */
  void ClearError() 
  {
    this->m_ErrorOccured = false;     
  }

  /**
   * If an error occurs in one of the observed IGSTK components this method 
   * will return true. */
  bool Error() 
  {
    return this->m_ErrorOccured;
  }

protected:

  /**
   * Construct an error observer for all the possible errors that occur in IGSTK
   * components used in the DataAcqusition class. */
  TrackingErrorObserver() : m_ErrorOccured(false)
  {                              //serial communication errors
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::OpenPortErrorEvent().GetEventName(),"Error opening com port."));
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::ClosePortErrorEvent().GetEventName(),"Error closing com port."));
                               //tracker errors
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerOpenErrorEvent().GetEventName(),"Error opening tracker communication."));
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerInitializeErrorEvent().GetEventName(),"Error initializing tracker."));
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerStartTrackingErrorEvent().GetEventName(),"Error starting tracking."));
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerStopTrackingErrorEvent().GetEventName(),"Error stopping tracking."));
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerCloseErrorEvent().GetEventName(),"Error closing tracker communication."));
    this->m_ErrorEvent2ErrorMessage.insert(std::pair<std::string,std::string>((igstk::TrackerUpdateStatusErrorEvent()).GetEventName(),"Error updating transformations from tracker."));
  }

  virtual ~TrackingErrorObserver(){}
private:
  bool m_ErrorOccured;
  std::map<std::string,std::string> m_ErrorEvent2ErrorMessage;

               //purposely not implemented
  TrackingErrorObserver(const Self&);
  void operator=(const Self&); 
};

static int exitFailure( igstk::SerialCommunication::Pointer &communication, 
                       igstk::Tracker::Pointer &tracker);

int exitFailure( igstk::SerialCommunication::Pointer &communication, 
                igstk::Tracker::Pointer &tracker)
{
  tracker->RequestStopTracking();            
  tracker->RequestClose();  
  communication->CloseCommunication();
  return EXIT_FAILURE;
}

/** Test the pivot calibration class as follows:
 *  1. Initialize (InitializationSuccessEvent), compute calibration 
 *     (CalibrationSuccessEvent) and get all the calibration
 *     data, transformation, pivot point and calibration RMSE 
 *     (TransformModifiedEvent, PointEvent, DoubleTypeEvent).
 *  2. Initialize (InitializationSuccessEvent), get all calibration data (
 *     InvalidRequestError).
 *  3. Invalid initialization (InitializationFailureEvent), get all calibration 
 *     data (InvalidRequestError).
 *  4. Initialize (InitializationSuccessEvent), compute calibration 
 *     (CalibrationFailureEvent - failed acquiring transformations) and get all 
 *     the calibration data (InvalidRequestError).
 *  5. Initialize (InitializationSuccessEvent), compute calibration 
 *     (CalibrationFailureEvent - degenerate case, no pivoting) and get all 
 *     the calibration data (InvalidRequestError).
 *  6. Same as 1, Initialize (InitializationSuccessEvent), compute calibration 
 *     (CalibrationSuccessEvent) and get all the calibration
 *     data, transformation, pivot point and calibration RMSE 
 *     (TransformModifiedEvent, PointEvent, DoubleTypeEvent).
 */
 int igstkPivotCalibrationNewTest( int argv, char * argc[] )
{
  igstk::SerialCommunication::Pointer trackerSerialCommunication;
  igstk::Tracker::Pointer tracker;
  TrackingErrorObserver::Pointer errorObserver = TrackingErrorObserver::New();
  PivotCalibrationEventObserver::Pointer pivotCalibrationObserver = 
    PivotCalibrationEventObserver::New();
  igstk::PivotCalibrationNew::Pointer pivotCalibration = 
    igstk::PivotCalibrationNew::New();
  const unsigned int DELAY_BEFORE_REQUEST_CALIBRATION = 5000; //5sec delay 
  const unsigned int DELAY_INCREMENT = 1000;

#ifdef PIVOT_CALIBRATION_SIMULATOR_TEST
  igstk::SerialCommunicationSimulator::Pointer 
    serialComm = igstk::SerialCommunicationSimulator::New();
               // load a previously captured file
  std::string simulationFile = ( IGSTKSandbox_DATA_ROOT + std::string("/Input") + 
                                 std::string("/") + TRACKER_SIMULATION_FILE_NAME );
  serialComm->SetFileName( simulationFile.c_str() );
  trackerSerialCommunication = serialComm;
#else  //a tracker is actually in  use
  trackerSerialCommunication = igstk::SerialCommunication::New();
#endif

#ifdef RECORD_DATA 
  std::string simulationFile = ( IGSTKSandbox_DATA_ROOT + std::string("/Input") + 
                                 std::string("/") + TRACKER_SIMULATION_FILE_NAME );
  trackerSerialCommunication->SetPortNumber( COM_PORT );
  trackerSerialCommunication->SetCaptureFileName( simulationFile.c_str() );
  trackerSerialCommunication->SetCapture( true );
#endif
#endif

  trackerSerialCommunication->SetPortNumber( COM_PORT );
  trackerSerialCommunication->SetParity( PARITY );
  trackerSerialCommunication->SetBaudRate( BAUD_RATE );
  trackerSerialCommunication->SetDataBits( DATA_BITS );
  trackerSerialCommunication->SetStopBits( STOP_BITS );
  trackerSerialCommunication->SetHardwareHandshake( HAND_SHAKE );
                  //observe errors generated by the serial communication
  trackerSerialCommunication->AddObserver( igstk::OpenPortErrorEvent(), 
                                           errorObserver );
  trackerSerialCommunication->AddObserver( igstk::ClosePortErrorEvent(), 
                                           errorObserver );

  igstk::PolarisTracker::Pointer pTracker = igstk::PolarisTracker::New();  
  std::string sromFile = ( IGSTKSandbox_DATA_ROOT + std::string("/Input") + 
                           std::string("/") + TOOL_ROM_FILE_NAME ); 
  pTracker->AttachSROMFileNameToPort( TOOL_PORT,sromFile );
  pTracker->SetCommunication(trackerSerialCommunication);
  tracker = pTracker;
                //observe errors generated by the serial communication
  tracker->AddObserver( igstk::TrackerOpenErrorEvent(), 
                        errorObserver );
  tracker->AddObserver( igstk::TrackerInitializeErrorEvent(), 
                        errorObserver );
  tracker->AddObserver( igstk::TrackerStartTrackingErrorEvent(), 
                        errorObserver );
  tracker->AddObserver( igstk::TrackerStopTrackingErrorEvent(), 
                        errorObserver );
  tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(), 
                        errorObserver );
  tracker->AddObserver( igstk::TrackerCloseErrorEvent(), 
                        errorObserver );
  tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(), 
                        errorObserver );

              //open serial communication
  trackerSerialCommunication->OpenCommunication();  
  if(errorObserver->Error()) {
    errorObserver->ClearError();
    return EXIT_FAILURE;
  }
                 //open tracker communication
  tracker->RequestOpen();
  if(errorObserver->Error()) {    
    errorObserver->ClearError();
    trackerSerialCommunication->CloseCommunication();
    return EXIT_FAILURE;
  }
          //initialize tracker
  tracker->RequestInitialize();
  if(errorObserver->Error()) {
    errorObserver->ClearError();
    tracker->RequestClose();
    trackerSerialCommunication->CloseCommunication();
    return EXIT_FAILURE;
  }
       //start tracking
  tracker->RequestStartTracking();
  if(errorObserver->Error()) {
    errorObserver->ClearError();
    tracker->RequestClose();
    trackerSerialCommunication->CloseCommunication();
    return EXIT_FAILURE;
  }

                  //observe all events generated by PivotCalibration
  pivotCalibration->AddObserver( igstk::InvalidRequestErrorEvent(),
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::PivotCalibrationNew::InitializationFailureEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::PivotCalibrationNew::InitializationSuccessEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::PivotCalibrationNew::DataAcquisitionStartEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::PivotCalibrationNew::CalibrationFailureEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::PivotCalibrationNew::CalibrationSuccessEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::TransformModifiedEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::PointEvent(), 
                                 pivotCalibrationObserver );
  pivotCalibration->AddObserver( igstk::DoubleTypeEvent(), 
                                 pivotCalibrationObserver );
 
        //Test 1. Initialize (InitializationSuccessEvent), compute calibration 
        //(CalibrationSuccessEvent) and get all the calibration
        //data, transformation, pivot point and calibration RMSE 
        //(TransformModifiedEvent, PointEvent, DoubleTypeEvent).

               //initialize the pivot calibration
  std::cout<<"\nTest 1:\n";
  std::cout<<"\t****Please pivot the tracked tool****\n";
  std::cout<<"Next line should report successful initialization.\n";
  pivotCalibration->RequestInitialize( NUMBER_OF_TRANSFORMATIONS, 
                                       tracker, 
                                       TOOL_PORT, 
                                       TOOL_NUMBER_ON_PORT );
  
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::InitializationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
                             //delay before requesting calibration which 
                             //starts data acquisition
  for(unsigned int i=DELAY_BEFORE_REQUEST_CALIBRATION; i>0; i-=DELAY_INCREMENT) {
    std::cout<<"Starting data acquisition in "<< i/1000 <<" seconds.\r";
    ::Sleep(DELAY_INCREMENT);
  }
  std::cout<<endl;
                  //request calibration, starts the data acquisition and 
                  //performs the computation
  std::cout<<"Next lines should report data acqusition and successful";
  std::cout<<" calibration.\n";
  pivotCalibration->RequestComputeCalibration();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::CalibrationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
  
               //don't need to check the following code for generated events
               //because the calibration was successful and the values are
               //available
  std::cout<<"Next line should report the calibration transformation.\n";
  pivotCalibration->RequestCalibrationTransform();
  std::cout<<"Next line should report the calibration RMSE.\n";
  pivotCalibration->RequestCalibrationRMSE();
  std::cout<<"Next line should report the pivot point.\n";
  pivotCalibration->RequestPivotPoint();

        //Test 2. Initialize (InitializationSuccessEvent), get all calibration 
        //data (InvalidRequestError).
  std::cout<<"\nTest 2:\n";
  std::cout<<"Next line should report successful initialization.\n";
  pivotCalibration->RequestInitialize( NUMBER_OF_TRANSFORMATIONS, 
                                       tracker, 
                                       TOOL_PORT, 
                                       TOOL_NUMBER_ON_PORT );
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::InitializationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );

          //no data should be available as we haven't performed the calibration
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestCalibrationTransform();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestCalibrationRMSE();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestPivotPoint();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  

        //Test 3. Invalid initialization (InitializationFailureEvent), get all 
        //calibration data (InvalidRequestError).

           //stop the tracking so that initialization of the pivot calibration
           //fails
  tracker->RequestStopTracking();
/*
  DOESN'T WORK RIGHT NOW BECAUSE THE TRACKER'S STATE MACHINE TABLE ISN'T COMPLETE.
  NO EVENT IS GENERATED WHEN CALLING RequestUpdateStatus() AND THE TRACKER
  IS NOT IN TRACKING STATE, SO INTIALIZATION SEEMS TO SUCCEED WHEN IT SHOULD FAIL
  std::cout<<"\nTest 3:\n";
  std::cout<<"Next line should report failed initialization.\n";
  pivotCalibration->RequestInitialize( NUMBER_OF_TRANSFORMATIONS, 
                                       tracker, 
                                       TOOL_PORT, 
                                       TOOL_NUMBER_ON_PORT );
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::InitializationFailureEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );

          //no data should be available as we haven't performed the calibration
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestCalibrationTransform();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestCalibrationRMSE();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestPivotPoint();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
*/

        // Test 4. Initialize (InitializationSuccessEvent), compute calibration 
        // (CalibrationFailureEvent - failed to acquire transformations), 
        //get all the calibration data (InvalidRequestError).
   
            //restart the tracking
  tracker->RequestStartTracking();

  std::cout<<"\nTest 4:\n";
  std::cout<<"Next line should report successful initialization.\n";
  pivotCalibration->RequestInitialize( NUMBER_OF_TRANSFORMATIONS, 
                                       tracker, 
                                       TOOL_PORT, 
                                       TOOL_NUMBER_ON_PORT );
  
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::InitializationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
                             //delay before requesting calibration which 
                             //starts data acquisition
  std::cout<<"\t****Please place tool outside working volume****\n";
  for(unsigned int i=DELAY_BEFORE_REQUEST_CALIBRATION ; i>0; i-=DELAY_INCREMENT) {
    std::cout<<"Starting data acquisition in "<< i/1000 <<" seconds.\r";
    ::Sleep(DELAY_INCREMENT);
  }
  std::cout<<endl;
                  //request calibration, starts the data acquisition and 
                  //performs the computation, place tool outside work volume
                  //before acquisition is complete, a certain number of data
                  //acquisition retries is attempted before failure is reported                  
  std::cout<<"Next line should report failed calibration.\n";
  pivotCalibration->RequestComputeCalibration();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::CalibrationFailureEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
  
          //no data should be available as calibration should have failed
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestCalibrationTransform();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestCalibrationRMSE();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  
  std::cout<<"Next line should report an invalid request error.\n";
  pivotCalibration->RequestPivotPoint();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::InvalidRequestErrorEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );  

        //Test 5. Initialize (InitializationSuccessEvent), compute calibration 
        //(CalibrationFailureEvent - degenerate case, no pivoting) and get all 
        //the calibration data (InvalidRequestError).
  std::cout<<"\nTest 5:\n";
  std::cout<<"Next line should report successful initialization.\n";
  pivotCalibration->RequestInitialize( NUMBER_OF_TRANSFORMATIONS, 
                                       tracker, 
                                       TOOL_PORT, 
                                       TOOL_NUMBER_ON_PORT );
  
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::InitializationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
                             //delay before requesting calibration which 
                             //starts data acquisition
  std::cout<<"\n\t****Please acquire data with a stationary tool****\n";
  for(unsigned int i=DELAY_BEFORE_REQUEST_CALIBRATION ; i>0; i-=DELAY_INCREMENT) {
    std::cout<<"Starting data acquisition in "<< i/1000 <<" seconds.\r";
    ::Sleep(DELAY_INCREMENT);
  }
  std::cout<<endl;
                  //request calibration, starts the data acquisition and 
                  //performs the computation
  std::cout<<"Next lines should report data acqusition and failed";
  std::cout<<" calibration.\n";
  pivotCalibration->RequestComputeCalibration();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::CalibrationFailureEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );


        //Test 6. Same as Test 1, Initialize (InitializationSuccessEvent), compute 
        //calibration (CalibrationSuccessEvent) and get all the calibration
        //data, transformation, pivot point and calibration RMSE 
        //(TransformModifiedEvent, PointEvent, DoubleTypeEvent).
  std::cout<<"\nTest 6:\n";
  std::cout<<"Next line should report successful initialization.\n";
  pivotCalibration->RequestInitialize( NUMBER_OF_TRANSFORMATIONS, 
                                       tracker, 
                                       TOOL_PORT, 
                                       TOOL_NUMBER_ON_PORT );
  
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::InitializationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
                             //delay before requesting calibration which 
                             //starts data acquisition
  std::cout<<"\t****Please pivot the tracked tool****\n";
  for(unsigned int i=DELAY_BEFORE_REQUEST_CALIBRATION ; i>0; i-=DELAY_INCREMENT) {
    std::cout<<"Starting data acquisition in "<< i/1000 <<" seconds.\r";
    ::Sleep(DELAY_INCREMENT);
  }
  std::cout<<endl;
                  //request calibration, starts the data acquisition and 
                  //performs the computation
  std::cout<<"Next lines should report data acqusition and successful";
  std::cout<<" calibration.\n";
  pivotCalibration->RequestComputeCalibration();
  if( pivotCalibrationObserver->GetLastEventName() != 
      igstk::PivotCalibrationNew::CalibrationSuccessEvent().GetEventName() )
    return exitFailure( trackerSerialCommunication, tracker );
  
               //don't need to check the following code for generated events
               //because the calibration was successful and the values are
               //available
  std::cout<<"Next line should report the calibration transformation.\n";
  pivotCalibration->RequestCalibrationTransform();
  std::cout<<"Next line should report the calibration RMSE.\n";
  pivotCalibration->RequestCalibrationRMSE();
  std::cout<<"Next line should report the pivot point.\n";
  pivotCalibration->RequestPivotPoint();

  return EXIT_SUCCESS;
}
