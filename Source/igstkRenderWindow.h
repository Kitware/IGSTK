/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkRenderWindow.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_RenderWindow_h_
#define __igstk_RenderWindow_h_

// ITK headers
#include "itkObject.h"

// VTK headers
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"


#include "igstkMacros.h"
#include "igstkStateMachine.h"



namespace igstk
{

/** \class RenderWindow
    \brief Window for presenting visualizations.

    This class provides an implementation of window that can be attached to a
    Graphical User Interface and in which a VTK pipeline will be rendered.
*/

class RenderWindow : public itk::Object
{

private:
    typedef igstk::StateMachine< RenderWindow > StateMachineType;
    typedef StateMachineType::TMemberFunctionPointer ActionType;
    typedef StateMachineType::StateType              StateType;
    typedef StateMachineType::InputType              InputType;
    typedef StateMachineType::StateIdentifierType    StateIdentifierType;

    FriendClassMacro( StateMachineType );

public:

    /** Some required typedefs for itk::Object. */
    typedef RenderWindow                   Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;

    /**  Run-time type information (and related methods). */
    itkTypeMacro(RenderWindow, Object);

    /** Method for creation of a reference counted object. */
    NewMacro(Self);
    
    /** Get the vtk RenderWindow */
    vtkRenderWindow* GetVTKRenderWindow() {return m_RenderWindow;}
    
    /** Get the vtk Renderer */
    vtkRenderer* GetVTKRenderer() {return m_Renderer;}
    
    /** Get the vtk Camera */
    vtkCamera* GetVTKCamera() {return m_Camera;}

protected:

    RenderWindow(void);
    ~RenderWindow(void);

private:

    vtkRenderWindow    * m_RenderWindow;
    vtkRenderer        * m_Renderer;
    vtkCamera          * m_Camera;

};

}

#endif //__igstk_TrackerTool_h_
