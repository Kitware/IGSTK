#include "InteractorObserver.h"

#include <FL/Fl.H>

namespace ISIS
{


InteractorObserver
::InteractorObserver()
{
  m_SliceViewer = NULL;
  m_Motion = false;
  m_FirstMove = false;
}




InteractorObserver
::~InteractorObserver()
{
  m_SliceViewer = NULL;
}



void
InteractorObserver
::SetImageSliceViewer( ImageSliceViewer * viewer )
{
  m_SliceViewer = viewer;
}



void 
InteractorObserver
::Execute(vtkObject * caller, unsigned long eventId, void *callData)
{
  int x, y;

  if ( eventId == ::vtkCommand::LeftButtonPressEvent )
  {
    if ( m_SliceViewer )
    {
      m_LButtonDownPoint[0] = Fl::event_x();
      m_LButtonDownPoint[1] = Fl::event_y();
    }
    m_Motion = true;
    m_FirstMove = true;
  }
  else if ( eventId == ::vtkCommand::LeftButtonReleaseEvent )
  {
    x = Fl::event_x();
    y = Fl::event_y();
    if ( m_SliceViewer && m_LButtonDownPoint[0] == x && m_LButtonDownPoint[1] == y)
    {      
      m_SliceViewer->SelectPoint( x, y );
    }
    m_Motion = false;
    m_FirstMove = false;
  }
  else if ( eventId == ::vtkCommand::MouseMoveEvent )
  {
    if ( m_SliceViewer && m_Motion )
    {
      x = Fl::event_x();
      y = Fl::event_y();
      m_SliceViewer->MovePoint( x, y, m_FirstMove );
      m_FirstMove = false;
    } 
  }
  else if ( eventId == ::vtkCommand::RightButtonPressEvent )
  {
    x = Fl::event_x();
    y = Fl::event_y();
    m_SliceViewer->SelectRightPoint( x, y );
  }
}



}



