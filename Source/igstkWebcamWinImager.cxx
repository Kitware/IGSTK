/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebcamWinImager.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif


#include "igstkWebcamWinImager.h"
#include "igstkEvents.h"
#include "vtkImageData.h"
#include <itksys/SystemTools.hxx>

#include "vtkWin32VideoSource.h"
#include "vtkImageViewer2.h"
#include "vtkImageData.h"
#include "vtkImageIterator.h"
#include <vtkPNGWriter.h>

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"

#define MAX_DEVICES 64
#define MAX_FORMATS 64


namespace igstk
{

}
}
