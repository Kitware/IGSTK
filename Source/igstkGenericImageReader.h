/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkGenericImageReader.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkGenericImageReader_h
#define __igstkGenericImageReader_h

#include "igstkObject.h"

// forward declaration.
class vtkKWImage;
class vtkKWImageIO;

namespace igstk {

/** \class GenericImageReader
 * 
 * \brief Class for reading image of all modalities supported by ITK.
 *
 * Implementation based on vtkKWImageIO class.
 * http://insight-journal.org/dspace/handle/1926/495
 *
 * \ingroup Object
 */

class GenericImageReader : public Object
{

public:
    
  typedef GenericImageReader                   Self;
  typedef Object                               Superclass;

  /** Constructor. */
  GenericImageReader();

  /** Destructor */
  ~GenericImageReader();

private:

  const vtkKWImageIO *  m_ImageIO;

};

} // end namespace igstk

#endif
