// Fl_Double_Window_Ex.h: interface for the Fl_Double_Window_Ex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FL_DOUBLE_WINDOW_EX_H__B28E1F2C_928D_42D1_B146_F87B68C49203__INCLUDED_)
#define AFX_FL_DOUBLE_WINDOW_EX_H__B28E1F2C_928D_42D1_B146_F87B68C49203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Fl/Fl_Double_Window.h"

class Fl_Double_Window_Ex : public Fl_Double_Window  
{

public:
  
  void(* m_ResizeCallbackFunc)(void*);

  void* m_CallData;

	void SetResizeCallback(void(* func)(void*), void*);
	
  void resize(int x, int y, int w, int h);

  Fl_Double_Window_Ex(int w, int h, const char *title = 0);

  int handle(int e);

};

#endif // !defined(AFX_FL_DOUBLE_WINDOW_EX_H__B28E1F2C_928D_42D1_B146_F87B68C49203__INCLUDED_)
