#include "vtkCommand.h"
#include "ImageSliceViewer.h"


namespace ISIS
{

class InteractorObserver : public vtkCommand
{
public:

  static InteractorObserver *New() 
    {return new InteractorObserver;};

  void SetImageSliceViewer( ImageSliceViewer * viewer );
  void Execute(vtkObject * caller, unsigned long eventId, void *callData);
   
protected:

  InteractorObserver();
  virtual ~InteractorObserver();

private:

  ImageSliceViewer   * m_SliceViewer;
  bool                 m_Motion;

};






}



