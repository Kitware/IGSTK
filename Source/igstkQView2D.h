/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQView2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkQView2D_h
#define __igstkQView2D_h

#include "igstkQView.h"

namespace igstk {


/** \class QView2D
 *
 * \brief QView2D provies the functionality of rendering a scene in a 2D
 * window. This class derives from the View class, and represents the
 * abstraction of a window in a GUI in which 3D objects will be displayed but
 * from a point of view in which the camera never changes the orientation of
 * the field of view. Zooming and Panning operations are possible in this
 * window.
 *
 *
 *  \image html  igstkView2D.png  "View2D State Machine Diagram"
 *  \image latex igstkView2D.eps  "View2D State Machine Diagram" 
 *
 */
class QView2D : public QView 
{
public:
  typedef QView2D    Self;
  typedef QView      Superclass;

  igstkTypeMacro( QView2D, QView );
   
  /** Constructor. The parameters of this constructor are related to the FLTK
   * box class. They include the screen coordinates of the upper left
   * coordinate, its width and height, and a string associated to the label */
//  QView2D( int x, int y, int w, int h, const char *l="");
#if QT_VERSION < 0x040000
  //! constructor for Qt 3
  QView2D(QWidget* parent = NULL, const char* name = NULL, Qt::WFlags f = 0);
#else
  //! constructor for Qt 4
  QView2D(QWidget* parent = NULL, Qt::WFlags f = 0);
#endif

  /** Destructor */
  ~QView2D( void );

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

  /** This method implements the user interactions with the view. It is an
   * overload of a virtual medthod defined for FLTK classes. */
//  int  handle( int event );

private:

  /** Variables for managing the Orientation of the slices */
  OrientationType      m_OrientationToBeSet;
  OrientationType      m_Orientation;

};

} // end namespace igstk

#endif 
