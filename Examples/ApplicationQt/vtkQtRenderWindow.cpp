/*=========================================================================
  vtkQtRenderWindow.cpp - copyright 2001 Matthias Koenig 
  koenig@isg.cs.uni-magdeburg.de
  http://wwwisg.cs.uni-magdeburg.de/~makoenig
  =========================================================================*/
/*=========================================================================
  This module is an extension of the "Visualization Toolkit 
  ( copyright (c) 1993-2000 Ken Martin, Will Schroeder, Bill Lorensen )".
  and combines it with "Qt (copyright (C) 1992-2000 Troll Tech AS)".
  =========================================================================*/
/*=========================================================================

  Module:    vtkQtRenderWindow.cpp
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

#include "vtkQtRenderWindow.h"
#include "vtkQtRenderWindowInteractor.h"
 
#define MAX_LIGHTS 8

QApplication* vtkQtRenderWindow::qtapp = NULL;

void vtkQtRenderWindow::initApp() {
  if (qApp)
    return;
  int ac = 0; char **av = NULL;
  qtapp = new QApplication(ac, av);
}

vtkQtRenderWindow* vtkQtRenderWindow::New() {
  initApp();
  vtkQtRenderWindow* tmp = new vtkQtRenderWindow;
  tmp->show();
  return tmp;
}

vtkQtRenderWindow::vtkQtRenderWindow(const QGLFormat &format, QWidget *parent, const char *name, const QGLWidget *shareWidget, WFlags f)
  :QGLWidget (format, parent, name, shareWidget, f) {
  initvtkQtRenderWindow();
}

vtkQtRenderWindow::vtkQtRenderWindow(QWidget *parent, const char *name, const QGLWidget *shareWidget, WFlags f)
  :QGLWidget (parent, name, shareWidget, f) {
  initvtkQtRenderWindow();
}

void vtkQtRenderWindow::initvtkQtRenderWindow() {
  vtkDebugMacro(<< " vtkQtRenderWindow constructor\n");
#ifdef _WIN32
  WindowId = QWidget::winId();
  DeviceContext = GetDC(WindowId);
  this->MultiSamples = 8;
  this->SetWindowName("Visualization Toolkit - Qt+OpenGL");
#else
  DisplayId = QPaintDevice::x11AppDisplay();
  ColorMap = QPaintDevice::x11AppColormap();
  WindowId = QPaintDevice::handle();
  #ifndef VTK_USE_MESA
    this->MultiSamples = GetGlobalMaximumNumberOfMultiSamples();
  #endif
  if ( this->WindowName ) 
    delete[] this->WindowName;
  this->WindowName = new char[strlen("Visualization Toolkit - Qt+OpenGL")+1];
  strcpy( this->WindowName, "Visualization Toolkit - Qt+OpenGL" );
  setCaption(this->WindowName);
#endif
  SetSwapBuffers(false);
  this->Interactor = NULL;
  qtRenWinInt = NULL;
  setFocusPolicy(QWidget:: WheelFocus);
}

vtkQtRenderWindow::~vtkQtRenderWindow() {
  short cur_light;
  vtkOpenGLRenderer *ren;
  
  makeCurrent();
  for (cur_light = GL_LIGHT0; cur_light < GL_LIGHT0+MAX_LIGHTS; cur_light++) {
    glDisable((GLenum)cur_light);
  }

  // tell each of the renderers that this render window/graphics context
  // is being removed (the RendererCollection is removed by vtkRenderWindow's
  // destructor)
  this->Renderers->InitTraversal();
  for ( ren = (vtkOpenGLRenderer *) this->Renderers->GetNextItemAsObject();
        ren != NULL;
        ren = (vtkOpenGLRenderer *) this->Renderers->GetNextItemAsObject() ) {
    ren->SetRenderWindow(NULL);
  }
  glFinish();
}

// Initialize the window for rendering.
void vtkQtRenderWindow::initializeGL (void) {
  vtkDebugMacro(<< " InitializeGL\n");
    
  vtkDebugMacro(<< " glMatrixMode ModelView\n");
  glMatrixMode( GL_MODELVIEW );
    
  vtkDebugMacro(<< " zbuffer enabled\n");
  glDepthFunc( GL_LEQUAL );
  glEnable( GL_DEPTH_TEST );
    
  vtkDebugMacro(" texture stuff\n");
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
  // initialize blending for transparency
  vtkDebugMacro(<< " blend func stuff\n");
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable(GL_BLEND);
    
  glEnable( GL_NORMALIZE );
  glAlphaFunc(GL_GREATER,0);
    
  this->Mapped = 1;
}

void vtkQtRenderWindow::resizeGL(int x,int y) {
  vtkDebugMacro(<< " SetSize\n");
  if ((this->Size[0] != x)||(this->Size[1] != y)) {
    this->Modified();
    this->Size[0] = x;
    this->Size[1] = y;
  }
}

void vtkQtRenderWindow::PrintSelf(ostream& os, vtkIndent indent) {
  this->vtkRenderWindow::PrintSelf(os,indent);
  os << indent << "MultiSamples: " << this->MultiSamples << "\n";
}

void vtkQtRenderWindow::paintGL() {
  vtkDebugMacro(<< " paintGL()\n");
  vtkRenderWindow::Render();
}

vtkRenderWindowInteractor* vtkQtRenderWindow::MakeRenderWindowInteractor() {
  qtRenWinInt = vtkQtRenderWindowInteractor::New();
  this->Interactor = qtRenWinInt;
  qtRenWinInt->SetRenderWindow(this);
  return qtRenWinInt;
}

void vtkQtRenderWindow::SetInteractor(vtkQtRenderWindowInteractor* arenwin) {
  qtRenWinInt = arenwin;
  vtkRenderWindow::SetInteractor(arenwin);
}

void vtkQtRenderWindow::mousePressEvent(QMouseEvent *me) {
  if (qtRenWinInt)
    qtRenWinInt->mousePressEvent(me);
}

void vtkQtRenderWindow::mouseReleaseEvent(QMouseEvent *me) {
  if (qtRenWinInt)
    qtRenWinInt->mouseReleaseEvent(me);
}

void vtkQtRenderWindow::mouseMoveEvent(QMouseEvent *me) {
  if (qtRenWinInt)
    qtRenWinInt->mouseMoveEvent(me);
}

void vtkQtRenderWindow::keyPressEvent(QKeyEvent *ke) {
  if (qtRenWinInt)
    qtRenWinInt->keyPressEvent(ke);
}




