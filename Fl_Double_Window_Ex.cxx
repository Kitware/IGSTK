// Fl_Double_Window_Ex.cxx: implementation of the Fl_Double_Window_Ex class.
//
//////////////////////////////////////////////////////////////////////

#include "Fl_Double_Window_Ex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "stdio.h"


int Fl_Double_Window_Ex::handle(int e)
{
  switch (e)
  {
  case FL_NO_EVENT:
  case FL_PUSH:
  case FL_RELEASE:
  case FL_ENTER:
  case FL_LEAVE:
  case FL_DRAG:
  case FL_FOCUS:
  case FL_UNFOCUS:
  case FL_KEYDOWN:
  case FL_KEYUP:
  case FL_CLOSE:
  case FL_MOVE:
  case FL_SHORTCUT:
  case FL_DEACTIVATE:
  case FL_ACTIVATE:
  case FL_HIDE:
  case FL_SHOW:
  case FL_PASTE:
  case FL_SELECTIONCLEAR:
  case FL_MOUSEWHEEL:
  case FL_DND_ENTER:
  case FL_DND_DRAG:
  case FL_DND_LEAVE:
  case FL_DND_RELEASE:
    break;
  }

  return Fl_Double_Window::handle(e);
}

Fl_Double_Window_Ex::Fl_Double_Window_Ex(int w, int h, const char *title):Fl_Double_Window(w, h, title)
{
  m_ResizeCallbackFunc = NULL;
}

void Fl_Double_Window_Ex::resize(int x, int y, int w, int h)
{
  Fl_Double_Window::resize(x, y, w, h);

  if (m_ResizeCallbackFunc)
  {
    m_ResizeCallbackFunc(m_CallData);
  }
}

void Fl_Double_Window_Ex::SetResizeCallback(void(* func)(void*), void* cdata)
{
  m_ResizeCallbackFunc = func;
  m_CallData = cdata;
}
