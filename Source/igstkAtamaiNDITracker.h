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

#ifndef __igstk_AtamaiNDITracker_h_
#define __igstk_AtamaiNDITracker_h_

#include "igstkTracker.h"
#include "vtkNDITracker.h"

namespace igstk
{
/** \class AtamaiNDITracker
    \brief A wrapper around the Atamai vtkNDITracker class.

*/

class AtamaiNDITracker : public Tracker
{
public:

  /** Some required typedefs for itk::Object. */

  typedef AtamaiNDITracker               Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(AtamaiNDITracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  vtkNDITracker *m_VTKTracker;

  AtamaiNDITracker(void);

  virtual ~AtamaiNDITracker(void);

private:

};

}

#endif //__igstk_AtamaiNDITracker_h_
