/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEvents.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_Events_h_
#define __igstk_Events_h_

/** 
 * This file contains the declarations of IGSTK Events used to communicate among components.
 */

namespace igstk 
{

itkEventMacro( IGSTKEvent,               itk::UserEvent );
itkEventMacro( PulseEvent,               IGSTKEvent );
itkEventMacro( RefreshEvent,             IGSTKEvent );

}


#endif
