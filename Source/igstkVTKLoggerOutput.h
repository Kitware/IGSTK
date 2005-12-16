/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVTKLoggerOutput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVTKLoggerOutput_h
#define __igstkVTKLoggerOutput_h

#include <vtkOutputWindow.h>
#include <vtkSmartPointer.h>
#include <itkLogger.h>
#include "igstkMacros.h"


namespace igstk
{
/** \class VTKLoggerOutput
 * \brief This class overrides vtk::OutputWindow 
 * to redirect messages from VTK to itk::Logger.
 *
 * Text messages that the system should display to the user are sent to 
 * this object (or subclasses of this class).
 *
 * Although this is a VTK class, do not call Delete() if you have
 * called OverrideVTKWindow().  After you call OverrideVTKWindow(),
 * the VTKLoggerOutput object will automatically be deleted when
 * the program exits.
 *
 * \ingroup Logging
 */
class VTKLoggerOutput : public vtkOutputWindow 
{
public:
  /** Standard class typedefs. */
  typedef VTKLoggerOutput              Self;
  typedef vtkOutputWindow              Superclass;
  typedef vtkSmartPointer<Self>        Pointer;
  typedef vtkSmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  igstkTypeMacro(VTKLoggerOutput, vtkOutputWindow);

  /** Create an object */
  static VTKLoggerOutput* New();

  /** Send a string to display. */
  virtual void DisplayText(const char* t);

  /** Send a string as an error message to display.
   * The default implementation calls DisplayText() but subclasses
   * could present this message differently. */
  virtual void DisplayErrorText(const char *t);

  /** Send a string as a warningmessage to display.
   * The default implementation calls DisplayText() but subclasses
   * could present this message differently. */
  virtual void DisplayWarningText(const char *t);

  /** Send a string as a message to display.
   * The default implementation calls DisplayText() but subclasses
   * could present this message differently. */
  virtual void DisplayGenericWarningText(const char *t);

  /** Send a string as a debug message to display.
   * The default implementation calls DisplayText() but subclasses
   * could present this message differently. */
  virtual void DisplayDebugText(const char *t);

  /** Overrides the default VTK log window.  Although this is a VTK class, do
   * not call Delete() if you have called OverrideVTKWindow().  After you call
   * OverrideVTKWindow(), the VTKLoggerOutput object will automatically be
   * deleted when the program exits. */
  virtual void OverrideVTKWindow();

  /** Type definition for the standard output stream */ 
  typedef std::ostream StdOStreamType;

  /** Print information about this object */
  void Print(StdOStreamType& os);

  /** Declarations related to the Logger */
  igstkLoggerMacro();

protected:
  /** Constructor */
  VTKLoggerOutput();

  /** Destructor */
  virtual ~VTKLoggerOutput();

  /** Override UnRegister for special reference count handling */
  void UnRegister(vtkObjectBase *o);

  /** Print information about this object */
  void PrintSelf(StdOStreamType& os, itk::Indent indent) const;

private:

  /** Special flag for safe reference counting */
  int m_InUnRegister;

};
  
/** operator << for printing out through the output stream */
std::ostream& operator<<(std::ostream& os, VTKLoggerOutput& o);
} // end namespace igstk


#endif  // __igstkVTKLoggerOutput_h
