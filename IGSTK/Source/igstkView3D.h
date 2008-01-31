/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView3D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkView3D_h
#define __igstkView3D_h

#include "igstkView.h"

namespace igstk {

/** \class View3D
 *
 * \brief Provides the functionality of rendering a scene in a 3D window. 
 *
 * This class derives from the View class, and represents the
 * abstraction of a window in a GUI in which 3D objects will be displayed but
 * from a point of view in which the camera can change its orientation and
 * position.
 *
 *
 *  \image html  igstkView3D.png  "View3D State Machine Diagram"
 *  \image latex igstkView3D.eps  "View3D State Machine Diagram" 
 *
 *  \sa View
 *  \sa View2D
 *
 *  \ingroup View
 *
 */
class View3D : public View 
{
public:

  igstkStandardClassTraitsMacro( View3D, View ); 

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

protected:
  /** Constructor */
  View3D( );

  /** Destructor */
  virtual ~View3D( void );

private:
  View3D(const View3D& ); // purposely not implemented
  View3D& operator=(const View3D& ); // purposely not implemented

};

} // end namespace igstk

#endif 
