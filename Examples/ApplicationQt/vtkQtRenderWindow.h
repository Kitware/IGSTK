/*=========================================================================
  vtkQtRenderWindow.h - copyright 2001 Matthias Koenig 
  koenig@isg.cs.uni-magdeburg.de
  http://wwwisg.cs.uni-magdeburg.de/~makoenig
  =========================================================================*/
/*=========================================================================
  This module is an extension of the "Visualization Toolkit 
  ( copyright (c) 1993-2000 Ken Martin, Will Schroeder, Bill Lorensen )".
  and combines it with "Qt (copyright (C) 1992-2000 Troll Tech AS)".
  =========================================================================*/
/*=========================================================================

  Module:    vtkQtRenderWindow.h
  Date:      $Date$
  Version:   $Revision$

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  
  * Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
   
  * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
   
  * Modified source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  
  =========================================================================*/

#ifndef _vtkQtRenderWindow_h
#define _vtkQtRenderWindow_h

#include <qgl.h>
#include <qapplication.h>
#include <qpaintdevice.h>
#include "vtkToolkits.h"
#include "vtkOpenGLRenderer.h"
#include "vtkRendererCollection.h"
#ifdef _WIN32
  #include "vtkWin32OpenGLRenderWindow.h"
  #include "vtkWin32RenderWindowInteractor.h"
#else 
  #ifdef VTK_USE_MESA
    #include "vtkXMesaRenderWindow.h"
    #include "vtkMesaRenderer.h"
  #else 
    #include "vtkXOpenGLRenderWindow.h"
    #include "vtkOpenGLRenderer.h"
  #endif
#endif 

class vtkQtRenderWindowInteractor;
#ifdef _WIN_32
  #define VTK_QT_EXPORT __declspec (dllexport)
#else
  #define VTK_QT_EXPORT
#endif
class VTK_QT_EXPORT vtkQtRenderWindow : public QGLWidget,
#ifdef _WIN32
  public vtkWin32OpenGLRenderWindow
#else 
  #ifdef VTK_USE_MESA
    public vtkXMesaRenderWindow
  #else
   public vtkXOpenGLRenderWindow
  #endif
#endif
{
 protected:
  int MultiSamples;
  long OldMonitorSetting;

 public:
  vtkQtRenderWindow(QWidget *parent = 0, const char* name = 0, const QGLWidget *shareWidget = 0, WFlags f = 0);
  vtkQtRenderWindow(const QGLFormat &format, QWidget *parent = 0, const char *name = 0, const QGLWidget *shareWidget = 0, WFlags f = 0);
  ~vtkQtRenderWindow();
  static vtkQtRenderWindow *New();
  static void initApp();
  void PrintSelf(ostream& os, vtkIndent indent);

  // vtk mapping to Qt
  // Description
  //! use updateGL, not Render
  void Render(); //inline 
  // Description:
  //! Make this window the current OpenGL context.
  void MakeCurrent(); //inline 
  // Description:
  //! Specify the size of the rendering window.
  void SetSize(int w,int h); //inline
  void SetSize(int a[2]); //inline
  void GetSize(int &w, int &h); //inline
    
  vtkRenderWindowInteractor* MakeRenderWindowInteractor();
  void SetInteractor(vtkQtRenderWindowInteractor*);

#ifdef _WIN32
  vtkTypeMacro(vtkQtRenderWindow, vtkWin32OpenGLRenderWindow);
#else
  #ifdef VTK_USE_MESA
    vtkTypeMacro(vtkQtRenderWindow, vtkMesaRenderWindow);
  #else
    vtkTypeMacro(vtkQtRenderWindow, vtkOpenGLRenderWindow);
  #endif
  //! need only for X11(!) 
  int GetDesiredDepth() { return QPaintDevice::x11Depth(); }
  //! need only for X11(!) 
  Colormap GetDesiredColormap() { return QPaintDevice::x11Colormap(); }
  //! need only for X11(!) 
  Visual *GetDesiredVisual() { return (Visual*) QPaintDevice::x11Visual(); }
#endif 
  // qt functions
  QSizePolicy sizePolicy() const; //inline
  QSize sizeHint() const;  //inline
  QSize minimumSizeHint() const;  //inline

  void initializeGL();
  void paintGL();
  void resizeGL(int, int);

  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void keyPressEvent(QKeyEvent*);
  virtual void focusInEvent(QFocusEvent*);
  virtual void focusOutEvent(QFocusEvent*); 
    
 private:
  void initvtkQtRenderWindow();
  vtkQtRenderWindowInteractor* qtRenWinInt;
  static QApplication *qtapp;
};

inline void vtkQtRenderWindow::SetSize(int w, int h)   { resizeGL(w, h); QGLWidget::resize(w, h); }
inline void vtkQtRenderWindow::SetSize(int a[2])       { this->SetSize(a[0], a[1]); }
inline void vtkQtRenderWindow::GetSize(int &w, int &h) { w = Size[0]; h = Size[1]; }

inline QSizePolicy vtkQtRenderWindow::sizePolicy() const { return QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding); }
inline QSize vtkQtRenderWindow::sizeHint() const         { return QSize(50, 50); }
inline QSize vtkQtRenderWindow::minimumSizeHint() const  { return QSize(50, 50); }

inline void vtkQtRenderWindow::focusInEvent(QFocusEvent*)  {}
inline void vtkQtRenderWindow::focusOutEvent(QFocusEvent*) {} 

inline void vtkQtRenderWindow::Render()      { updateGL(); }
inline void vtkQtRenderWindow::MakeCurrent() { QGLWidget::makeCurrent(); }

#endif



