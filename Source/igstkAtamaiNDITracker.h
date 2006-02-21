/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAtamaiNDITracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkAtamaiNDITracker_h
#define __igstkAtamaiNDITracker_h

#include "igstkTracker.h"

class vtkNDITracker;
class vtkTrackerTool;
class vtkCallbackCommand;

namespace igstk
{
/** \class AtamaiNDITracker
 *  \brief A wrapper around the Atamai vtkNDITracker class.
 */

class AtamaiNDITracker : public igstk::Tracker
{
public:

  /** Some required typedefs for itk::Object. */
  typedef AtamaiNDITracker               Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef vtkTrackerTool                       VTKTrackerToolType;
  typedef VTKTrackerToolType*                  VTKTrackerToolPointer;
  typedef std::vector< VTKTrackerToolPointer > VTKTrackerToolVectorType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(AtamaiNDITracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  /** The VTK class that communicates with the tracking device */
  vtkNDITracker        *m_VTKTracker;
  /** The tools */
  VTKTrackerToolVectorType m_VTKTrackerTools;
  /** A callback command for catching VTK errors */
  vtkCallbackCommand   *m_VTKErrorCommand;
  /** A callback function that the vtkCallbackCommand uses */
  friend void AtamaiNDITrackerErrorCallback(vtkObject *, unsigned long,
                                            void *, void *);
  /** A flag that is set when the VTK class generates an error */
  int                   m_VTKError;

  AtamaiNDITracker(void);

  virtual ~AtamaiNDITracker(void);

  virtual ResultType InternalOpen( void );

  virtual ResultType InternalClose( void );

  virtual ResultType InternalActivateTools( void );

  virtual ResultType InternalDeactivateTools( void );

  virtual ResultType InternalStartTracking( void );

  virtual ResultType InternalStopTracking( void );

  virtual ResultType InternalUpdateStatus( void );

  virtual ResultType InternalReset( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

}

#endif //__igstk_AtamaiNDITracker_h_
