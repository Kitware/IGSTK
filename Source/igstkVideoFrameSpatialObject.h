/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

  Made by SINTEF Health Research - Medical technology:
  http://www.sintef.no/medtech

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoFrameSpatialObject_h
#define __igstkVideoFrameSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"


class vtkImageData;

namespace igstk
{


class VideoFrameSpatialObject 
: public SpatialObject
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VideoFrameSpatialObject, SpatialObject )
  
  typedef itk::SpatialObject<3>          VideoFrameSpatialObjectType;
  
  void Initialize();

  /** Returns wheter the image has any pixels allocated or not */
  virtual bool IsEmpty() const;

  vtkImageData* GetImageData();

  igstkSetMacro(Width, unsigned int);

  igstkSetMacro(Height, unsigned int);

  igstkSetMacro(PixelSizeX, double);

  igstkSetMacro(PixelSizeY, double);

  igstkGetMacro(CalibrationTransform, igstk::Transform);
  igstkSetMacro(CalibrationTransform, igstk::Transform);

  /** Request to get the VTK image as a const pointer payload into an event.
   *  Both the const and non-const versions are needed. */
//  void RequestGetVTKImage();
  void RequestGetVTKImage() const;

  void* GetImagePtr();

protected:
  VideoFrameSpatialObject( void );
  ~VideoFrameSpatialObject( void );
  
private:

  vtkImageData* m_ImageData;

  unsigned int              m_Width;
  unsigned int              m_Height;
  double                    m_PixelSizeX;
  double                    m_PixelSizeY;
  igstk::Transform          m_CalibrationTransform;
};

} // end namespace igstk

#endif // __igstkVideoFrameSpatialObject_h
