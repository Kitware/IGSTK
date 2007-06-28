/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewNew2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkViewNew2D_h
#define __igstkViewNew2D_h

#include "igstkViewNew.h"

namespace igstk {


/** \class ViewNew2D
 *
 * \brief ViewNew2D provies the functionality of rendering a scene in a 2D
 * window. This class derives from the View class, and represents the
 * abstraction of a window in a GUI in which 3D objects will be displayed but
 * from a point of view in which the camera never changes the orientation of
 * the field of view. Zooming and Panning operations are possible in this
 * window.
 *
 *
 *  \image html  igstkViewNew2D.png  "ViewNew2D State Machine Diagram"
 *  \image latex igstkViewNew2D.eps  "ViewNew2D State Machine Diagram" 
 *
 */
class ViewNew2D : public ViewNew 
{
public:

  typedef ViewNew2D    Self;
  typedef ViewNew      Superclass;

  igstkTypeMacro( ViewNew2D, ViewNew );

  igstkNewMacro( ViewNew2D );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

  /** Orientation Type: Publically declared
   * orientation types supported for slice viewing.  */
  typedef enum
    { 
    Sagittal, 
    Coronal, 
    Axial 
    } 
  OrientationType;


  /** Select a slice orientation */
  void RequestSetOrientation( const OrientationType & orientation );

protected:
  /** Constructor */
  ViewNew2D( );

  /** Destructor */
  ~ViewNew2D( void );

private:

  /** Variables for managing the Orientation of the slices */
  OrientationType      m_OrientationToBeSet;
  OrientationType      m_Orientation;

};

} // end namespace igstk

#endif 
