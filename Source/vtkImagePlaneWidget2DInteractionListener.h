#ifndef IMAGEPLANESTARTINTERACTIONLISTENER_H_
#define IMAGEPLANESTARTINTERACTIONLISTENER_H_

#include <vtkCommand.h>
#include <vtkImagePlaneWidget2D.h>
#include <vector>
#include <vtkObject.h>


/**
 * An observer class to listen for achieving uniform results from user
 * interaction across several vtkImagePlaneWidget2Ds.
 * @date 200.09.04
 * @author Torleif Sandnes, Sintef Health Research
 */
class vtkImagePlaneWidget2DInteractionListener : public vtkCommand
{
public:
  typedef std::vector<vtkImagePlaneWidget2D *> SubscriberVector;

  static vtkImagePlaneWidget2DInteractionListener *New(); 

  virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData);
   

  void SubscribeToWindowLevel(vtkImagePlaneWidget2D *imagePlaneWidget);

  void SubscribeToSliceIndex(vtkImagePlaneWidget2D *imagePlaneWidget);

private:
  vtkImagePlaneWidget2DInteractionListener();
  ~vtkImagePlaneWidget2DInteractionListener();

  void NotifyWindowLevelSubscribers(double window, double level);
  void NotifySliceIndexSubscribers(int sliceIndex);
  
  SubscriberVector mWindowLevelSubscribers;
  SubscriberVector mSliceIndexSubscribers;
};
#endif
