#include "IGSTKErrorObserver.h"
#include <igstkTracker.h>
#include <igstkEvents.h>

IGSTKErrorObserver::IGSTKErrorObserver() : errorOccured(false)
{                              //serial communication errors
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::OpenPortErrorEvent().GetEventName(),"Error opening com port."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::ClosePortErrorEvent().GetEventName(),"Error closing com port."));
                              //tracker errors
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerOpenErrorEvent().GetEventName(),"Error opening tracker communication."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerInitializeErrorEvent().GetEventName(),"Error initializing tracker."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerStartTrackingErrorEvent().GetEventName(),"Error starting tracking."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerStopTrackingErrorEvent().GetEventName(),"Error stopping tracking."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerUpdateStatusErrorEvent().GetEventName(),"Error getting data from tracker."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>(igstk::TrackerCloseErrorEvent().GetEventName(),"Error closing tracker communication."));
  this->errorEvent2ErrorMessage.insert(std::pair<std::string,std::string>((igstk::TrackerUpdateStatusErrorEvent()).GetEventName(),"Error updating transformations from tracker."));
}
/*****************************************************************************/
void IGSTKErrorObserver::Execute(itk::Object *caller, const itk::EventObject & event) throw (std::exception)
{
  std::map<std::string,std::string>::iterator it;
  std::string className = event.GetEventName();
  it = this->errorEvent2ErrorMessage.find(className);
               //listening to an event that wasn't entered into the map of events we listen to
  if(it == this->errorEvent2ErrorMessage.end()) {
    std::string logicalErrorMessage = std::string("ErrorObserver: listening to an event (") + className + std::string(") which is not one of the expected events.");
    itkExceptionMacro(<< logicalErrorMessage.c_str());
  }
  this->errorOccured = true;
  this->errorMessage = (*it).second;
}
/*****************************************************************************/
void IGSTKErrorObserver::Execute(const itk::Object *caller, const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}
