/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewNew3D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkViewNew3D_h
#define __igstkViewNew3D_h

#include "igstkViewNew.h"

namespace igstk {

/** \class ViewNew3D
 *
 * \brief ViewNew3D provies the functionality of rendering a scene in a 3D
 * window. This class derives from the ViewNew class, and represents the
 * abstraction of a window in a GUI in which 3D objects will be displayed but
 * from a point of view in which the camera can change its orientation and
 * position.
 *
 *
 *  \image html  igstkViewNew3D.png  "ViewNew3D State Machine Diagram"
 *  \image latex igstkViewNew3D.eps  "ViewNew3D State Machine Diagram" 
 *
 */
class ViewNew3D : public ViewNew 
{
public:

  igstkStandardClassTraitsMacro( ViewNew3D, ViewNew ); 

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

protected:
  /** Constructor */
  ViewNew3D( );

  /** Destructor */
  ~ViewNew3D( void );

};

} // end namespace igstk

#endif 
