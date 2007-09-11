#include "vtkImagePlaneWidget2DInteractionListener.h"

vtkImagePlaneWidget2DInteractionListener::
vtkImagePlaneWidget2DInteractionListener() : 
  vtkCommand()
{}

vtkImagePlaneWidget2DInteractionListener::
~vtkImagePlaneWidget2DInteractionListener()
{}

vtkImagePlaneWidget2DInteractionListener *
vtkImagePlaneWidget2DInteractionListener::New()
{
  return new vtkImagePlaneWidget2DInteractionListener();
}

void vtkImagePlaneWidget2DInteractionListener::
Execute(vtkObject *caller, unsigned long eventId, void *callData)
{
  vtkImagePlaneWidget2D *imagePlane  =
    dynamic_cast<vtkImagePlaneWidget2D *>(caller);
  if(imagePlane == NULL)
  {
    vtkGenericWarningMacro("Calling object is not a vtkImagePlaneWidget2D.");
    return;
  }
  int state = imagePlane->GetState();
  if( state == vtkImagePlaneWidget2D::WINDOWLEVELLING)
  {
    double windowLevel[2];
    imagePlane->GetWindowLevel(windowLevel);
    NotifyWindowLevelSubscribers(windowLevel[0], windowLevel[1]);
  }
  else if(state == vtkImagePlaneWidget2D::PUSHING)
  {
    int sliceIndex = imagePlane->GetSliceIndex();
    NotifySliceIndexSubscribers(sliceIndex);
  }
}

void vtkImagePlaneWidget2DInteractionListener::
SubscribeToWindowLevel(vtkImagePlaneWidget2D *imagePlaneWidget)
{
  SubscriberVector::const_iterator it = mWindowLevelSubscribers.begin();
  for(; it != mWindowLevelSubscribers.end(); ++it)
  {
    if(*it == imagePlaneWidget)
    {
      return;
    }
  }
  mWindowLevelSubscribers.push_back(imagePlaneWidget);
}

void vtkImagePlaneWidget2DInteractionListener::
SubscribeToSliceIndex(vtkImagePlaneWidget2D *imagePlaneWidget)
{
  SubscriberVector::const_iterator it = 
    mSliceIndexSubscribers.begin();
  for(; it != mSliceIndexSubscribers.end(); ++it)
  {
    if(*it == imagePlaneWidget)
    {
      return;
    }
  }
  mSliceIndexSubscribers.push_back(imagePlaneWidget);
}

void vtkImagePlaneWidget2DInteractionListener::
NotifyWindowLevelSubscribers(double window, double level)
{
  SubscriberVector::iterator it = mWindowLevelSubscribers.begin();
  for(; it != mWindowLevelSubscribers.end(); ++it)
  {
    (*it)->SetWindowLevel(window, level);
  }
}

void vtkImagePlaneWidget2DInteractionListener::
NotifySliceIndexSubscribers(int sliceIndex)
{
  SubscriberVector::iterator it = mSliceIndexSubscribers.begin();
  for(; it != mSliceIndexSubscribers.end(); ++it)
  {
    (*it)->SetSliceIndex(sliceIndex);
  }
}
