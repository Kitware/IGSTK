/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkObjectProperty.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkObjectProperty_h
#define __igstkObjectProperty_h

#include <itkObject.h>
#include <list>

namespace igstk
{

/** \class ObjectProperty
 * 
 * \brief 
 */
class ObjectProperty 
{

public:

  ObjectProperty( void );
  ~ObjectProperty( void );

  float GetRed() {return m_Color[0];}
  float GetGreen() {return m_Color[1];}
  float GetBlue() {return m_Color[2];}
  float GetOpacity() {return m_Color[3];}

  /** Set the color */
  void SetColor(float r, float g, float b, float a);

protected:

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  float m_Color[4];

};

} // end namespace igstk

#endif // __igstkObject_h
