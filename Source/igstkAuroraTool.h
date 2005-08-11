/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_AuroraTool_h_
#define __igstk_AuroraTool_h_

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class AuroraTool
  * \brief An Aurora-specific TrackerTool class.
  *
  * This class is a for providing Aurora-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Aurora tool.
*/

class AuroraTool : public TrackerTool
{
public:

  typedef AuroraTool                     Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Run-time type information (and related methods). */
  igstkTypeMacro(AuroraTool, TrackerTool);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  AuroraTool();
  ~AuroraTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};  


} // namespace igstk


#endif  // __igstk_AuroraTool_h_

