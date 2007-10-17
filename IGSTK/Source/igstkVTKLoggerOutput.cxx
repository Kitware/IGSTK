/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVTKLoggerOutput.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <vtkObjectFactory.h>
#include <itkIndent.h>
#include "igstkVTKLoggerOutput.h"


namespace igstk
{
vtkStandardNewMacro(VTKLoggerOutput);


/** Constructor */
VTKLoggerOutput::VTKLoggerOutput()
{
  m_InUnRegister = 0;

  m_NumberOfWarningMessages = 0;

  m_NumberOfErrorMessages = 0;
}


/** Destructor. */
VTKLoggerOutput::~VTKLoggerOutput()
{
}

/** Override the default VTK Window */
void VTKLoggerOutput::OverrideVTKWindow()
{ 
  vtkOutputWindow::SetInstance( this );
  this->Delete();
}

/** Special UnRegister handling since vtkOutputWindow holds a reference */
void VTKLoggerOutput::UnRegister(vtkObjectBase *o)
{
  // because calling vtkOutputWindow::SetInstance(0) will cause
  // vtkOutputWindow to try to destruct us, we need to make sure
  // that vtkObject::UnRegister() is not called twice.
  if (m_InUnRegister)
    {
    this->ReferenceCount--;
    return;
    }

  if (this->ReferenceCount == 2 && vtkOutputWindow::GetInstance() == this)
    {
    m_InUnRegister = 1;

    // tell vtkOutputWindow to forget us
    vtkOutputWindow::SetInstance( 0 );

    // tell vtkOutputWindow to recreate the default output window
    vtkOutputWindow::GetInstance();

    m_InUnRegister = 0;
    }

  this->vtkObject::UnRegister(o);
}

/** Send a string to display. */
void VTKLoggerOutput::DisplayText(const char* t)
{
  if( m_Logger )
    {
    m_Logger->Write(itk::Logger::INFO, t);
    }
}


/** Send a string as an error message to display.
 * The default implementation calls DisplayText() but subclasses
 * could present this message differently. */
void VTKLoggerOutput::DisplayErrorText(const char *t)
{
  m_NumberOfErrorMessages++;

  if( m_Logger )
    {
    m_Logger->Write(itk::Logger::CRITICAL, t);
    }
}


/** Send a string as a warningmessage to display.
 * The default implementation calls DisplayText() but subclasses
 * could present this message differently. */
void VTKLoggerOutput::DisplayWarningText(const char *t)
{
  m_NumberOfWarningMessages++;

  if( m_Logger )
    {
    m_Logger->Write(itk::Logger::WARNING, t);
    }
}


/** Send a string as a message to display.
 * The default implementation calls DisplayText() but subclasses
 * could present this message differently. */
void VTKLoggerOutput::DisplayGenericWarningText(const char *t)
{
  if( m_Logger )
    {
    m_Logger->Write(itk::Logger::WARNING, t);
    }
}


/** Send a string as a debug message to display.
 * The default implementation calls DisplayText() but subclasses
 * could present this message differently. */
void VTKLoggerOutput::DisplayDebugText(const char *t)
{
  if( m_Logger )
    {
    m_Logger->Write(itk::Logger::DEBUG, t);
    }
}

/** Print information about this object */
void VTKLoggerOutput::PrintSelf(StdOStreamType& os, itk::Indent indent) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  indent = indent.GetNextIndent();

  if( m_Logger )
    {
    os << indent;
    m_Logger->Print(os, indent);
    }
}

/** Print information about this object */
void VTKLoggerOutput::Print(StdOStreamType& os)
{
  itk::Indent indent;

  os << indent << this->GetClassName() << " (" << this << ")\n";
  this->PrintSelf(os, indent.GetNextIndent());
  os << indent << "\n";
}

/** Operator<< for output stream */
std::ostream& operator<<(std::ostream& os, VTKLoggerOutput& o)
{
  o.Print(os);
  return os;
}
  
} // the end of the namespace igstk
