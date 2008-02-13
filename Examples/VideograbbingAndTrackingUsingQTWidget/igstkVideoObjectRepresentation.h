/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoObjectRepresentation.h
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
#ifndef __igstkVideoObjectRepresentation_h
#define __igstkVideoObjectRepresentation_h

#include "igstkVideoSpatialObject.h"
#include "igstkObjectRepresentation.h"
#include "igstkQuickTimeGrabber.h"

class vtkImageData;
class vtkActor;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;

namespace igstk
{
class VideoObjectRepresentation : public ObjectRepresentation
{
public:
  igstkStandardClassTraitsMacro( VideoObjectRepresentation, 
                                 ObjectRepresentation );
      
  /** Connect this representation class to the spatial object */
  void RequestSetVideoSpatialObject(const VideoSpatialObject * spatialObject);
  
  /** Create the VTK actors for displaying geometry */
  void CreateActors();
  
  /** Start video grabbing */
  void StartGrabbing();
  
  /** Stop video grabbing */
  void StopGrabbing();
protected:
  VideoObjectRepresentation(void);
  
  ~VideoObjectRepresentation(void);
    
private:
    
  /** VTK classes that support display of an image */
  vtkImageData              *m_ImageData;
  vtkActor                  *m_ImageActor;
  vtkPlaneSource            *m_PlaneSource;
  vtkPolyDataMapper         *m_VideoMapper; 
  vtkTexture                *m_VideoTexture;
  QuickTimeGrabber::Pointer m_VideoGrabber;
    
  /** Update the visual representation with changes in the geometry */
  void UpdateRepresentationProcessing();  
};

} // end namespace igstk

#endif // __igstkVideoObjectRepresentation_h
