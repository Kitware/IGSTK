/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQView3D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkQView3D_h
#define __igstkQView3D_h

#include "igstkQView.h"
#include <QWidget.h>

namespace igstk {

/** \class QView3D
 *
 * \brief QView3D provies the functionality of rendering a scene in a 3D
 * window. This class derives from the QView class, and represents the
 * abstraction of a window in a GUI in which 3D objects will be displayed but
 * from a point of view in which the camera can change its orientation and
 * position.
 *
 *
 *  \image html  igstkView3D.png  "View3D State Machine Diagram"
 *  \image latex igstkView3D.eps  "View3D State Machine Diagram" 
 *
 */
class QView3D : public QView 
{
public:
  typedef QView3D    Self;
  typedef QView      Superclass;

  igstkTypeMacro( QView3D, QView );
   
  /** Constructor. The parameters of this constructor are related to the FLTK
   * box class. They include the screen coordinates of the upper left
   * coordinate, its width and height, and a string associated to the label */
//  QView3D( int x, int y, int w, int h, const char *l="");
#if QT_VERSION < 0x040000
  //! constructor for Qt 3
  QView3D(QWidget* parent = NULL, const char* name = NULL, Qt::WFlags f = 0);
#else
  //! constructor for Qt 4
  QView3D(QWidget* parent = NULL, Qt::WFlags f = 0);
#endif

  /** Destructor */
  ~QView3D( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

protected:

  /** This method implements the user interactions with the view. It is an
   * overload of a virtual medthod defined for FLTK classes. */
//  int  handle( int event );

};

} // end namespace igstk

#endif 
