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


  RenderWindow::RenderWindow()
  {
    m_RenderWindow = vtkRenderWindow::New();
    
    m_Renderer = vtkRenderer::New();
    
    m_Camera = vtkCamera::New();
    
  }



  RenderWindow::~RenderWindow()
  {
  }
  

}
