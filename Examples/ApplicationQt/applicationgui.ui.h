/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "qfiledialog.h"

#include <vector>
#include <string>

void ApplicationGUI::OnLoad()
{
  
}


void ApplicationGUI::OnRender()
{
  
}


bool ApplicationGUI::SelectFilesDirectory( char* dir )
{
  QString dirpath;
  
  dirpath = QFileDialog::getExistingDirectory(); 

  if (dirpath)
  {
    sprintf(dir, dirpath );
    return true;
  }
  else
  {
    return false;
  }
}

void ApplicationGUI::init()
{
  m_pApplication = NULL;
  
  AxialViewerInteractor = vtkQtRenderWindowInteractor::New();
  CoronalViewerInteractor = vtkQtRenderWindowInteractor::New();
  SagittalViewerInteractor = vtkQtRenderWindowInteractor::New();
  VolumeViewerInteractor = vtkQtRenderWindowInteractor::New();
}


void ApplicationGUI::OnAxialSlider(int i)
{

}


void ApplicationGUI::OnCoronalSlider(int i)
{

}


void ApplicationGUI::OnSagittalSlider(int i)
{

}


void ApplicationGUI::SetAxialSliderRange( int a, int b )
{
  AxialSlider->setRange(a, b);
}


void ApplicationGUI::SetCoronalSliderRange( int a, int b )
{
  CoronalSlider->setRange(a, b);
}


void ApplicationGUI::SetSagittalSliderRange( int a, int b )
{
  SagittalSlider->setRange(a, b);
}


void ApplicationGUI::SetAxialSliderPos( int i )
{
  AxialSlider->setValue(i);
}


void ApplicationGUI::SetCoronalSliderPos( int i )
{
  CoronalSlider->setValue(i);
}


void ApplicationGUI::SetSagittalSliderPos( int i )
{
  SagittalSlider->setValue(i);
}


void ApplicationGUI::OnExit()
{
  if (m_pApplication)
  {
    m_pApplication->exit();
  }
}


void ApplicationGUI::SetApplication( QApplication * pApplication )
{
  m_pApplication = pApplication;
}


void ApplicationGUI::destroy()
{
  if ( AxialViewerInteractor)
  {
    AxialViewerInteractor->Delete();
  }
  if ( CoronalViewerInteractor)
  {
    CoronalViewerInteractor->Delete();
  }
  if ( SagittalViewerInteractor)
  {
    SagittalViewerInteractor->Delete();
  }
  if ( VolumeViewerInteractor)
  {
    VolumeViewerInteractor->Delete();
  }  
}
