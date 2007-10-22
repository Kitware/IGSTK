/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerToolNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTrackerToolNew_h
#define __igstkPolarisTrackerToolNew_h

#include "igstkTrackerToolNew.h"

namespace igstk
{
/** \class PolarisTrackerToolNew
  * \brief A Polaris-specific TrackerTool class.
  *
  * This class is a for providing Polaris-specific functionality
  * for TrackerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an Polaris tool.
  *
  * \ingroup Tracker
  *
  */

class PolarisTrackerToolNew : public TrackerToolNew
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PolarisTrackerToolNew, TrackerToolNew )

  /** Request setting the port number. The tool can only be attached to a
   * Tracker after its port number has been defined */
  void RequestSetPort( unsigned int portNumber );
      
protected:

  PolarisTrackerToolNew();
  ~PolarisTrackerToolNew();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  PolarisTrackerToolNew(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** Port number that identifies this tool in the Tracker. */
  unsigned int m_PortNumber;
  unsigned int m_PortNumberToBeSet;
};  


} // namespace igstk


#endif  // __igstk_PolarisTrackerToolNew_h_
