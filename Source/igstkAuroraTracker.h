/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_AuroraTracker_h_
#define __igstk_AuroraTracker_h_

#include "igstkTracker.h"

namespace igstk
{
/** \class AuroraTracker
    \brief Implementation of the Aurora Tracker class.

*/

class AuroraTracker : public Tracker
{
public:

  /** Some required typedefs for itk::Object. */

  typedef AuroraTracker                  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  itkTypeMacro(AuroraTracker, Object);

  /** Method for creation of a reference counted object. */
  NewMacro(Self);  

protected:

  AuroraTracker(void);

  virtual ~AuroraTracker(void);

  void AttemptToSetUpCommunicationProcessing( void );


};

}

#endif //__igstk_AuroraTracker_h_
