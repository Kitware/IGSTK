/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkRenderWindow.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkRenderWindow.h"

namespace igstk
{

/** Constructor */
RenderWindow::RenderWindow()
{
  m_RenderWindow = vtkRenderWindow::New();
  m_Renderer = vtkRenderer::New();
  m_RenderWindow->AddRenderer(m_Renderer);
  m_Camera = m_Renderer->GetActiveCamera();
  m_RenderWindow->BordersOff();
  m_Renderer->SetBackground(0.5,0.5,0.5);
}

/** Destructor */
RenderWindow::~RenderWindow()
{
  m_RenderWindow->Delete();
  m_Renderer->Delete();
}
  

} // end namespace igstk
