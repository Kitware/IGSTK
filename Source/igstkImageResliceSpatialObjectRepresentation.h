/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceSpatialObjectRepresentation_h
#define __igstkImageResliceSpatialObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"
#include "igstkImageReslicePlaneSpatialObject.h"

#include "vtkImageActor.h"

class vtkImageActor;
class vtkImageMapToWindowLevelColors;
class vtkLookupTable;
class vtkImageMapToColors;
class vtkImageReslice;
class vtkMatrix4x4;

namespace igstk
{


/** \class ImageResliceSpatialObjectRepresentation
 *
 * \brief This class generates representation of resliced image plane. 
 *
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class ImageResliceSpatialObjectRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro(
                                ImageResliceSpatialObjectRepresentation,ObjectRepresentation )

public:

  /** Typedefs */
  typedef TImageSpatialObject                 ImageSpatialObjectType;
  typedef typename ImageSpatialObjectType::Pointer
                                              ImageSpatialObjectPointer;

  typedef ImageReslicePlaneSpatialObject< ImageSpatialObjectType >
                                                   ReslicePlaneSpatialObjectType;

  typedef typename ReslicePlaneSpatialObjectType::Pointer
                                              ReslicePlaneSpatialObjectPointer;
  
  typedef Transform                           TransformType;

  void RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
                                                             planeSpatialObject);

  void RequestSetImageSpatialObject( const ImageSpatialObjectType * image );

  /** Set the Window Level for the representation */
  void SetWindowLevel( double window, double level );

  /** Set the opacity */
  void SetOpacity(float alpha);
 
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

protected:

  /** Constructor */
  ImageResliceSpatialObjectRepresentation();

  /** Destructor */
  ~ImageResliceSpatialObjectRepresentation();
 
  /** Create the vtkActors */
  virtual void CreateActors();

  /** Delete the vtkActors */
  virtual void DeleteActors();

  /** Observer macro that will received a event with an image as payload and
   * will store it internally. This will be the receptor of the event sent by
   * the ImageSpatialObject when an image is requested. */
   igstkObserverMacro( VTKImage, VTKImageModifiedEvent, 
                       EventHelperType::VTKImagePointerType );
   igstkObserverMacro( ImageTransform, CoordinateSystemTransformToEvent, 
     CoordinateSystemTransformToResult );

  /** Verify time stamp. Use the reslicing tool transform to verify 
    * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  ImageResliceSpatialObjectRepresentation(const Self&);
  void operator=(const Self&);   //purposely not implemented

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImageSpatialObject );
  igstkDeclareInputMacro( InValidImageSpatialObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( ConnectVTKPipeline );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ImageSpatialObjectSet );
  igstkDeclareStateMacro( ReslicePlaneSpatialObjectSet );

  /** Connect VTK pipeline */
  void ConnectVTKPipelineProcessing();
 
  /** Set the image spatial object */
  void SetImageSpatialObjectProcessing( void );

  /** Set the reslice plane spatial object */
  void SetReslicePlaneSpatialObjectProcessing( void );

  /** Report invalid image spatial object type */
  void ReportInvalidImageSpatialObjectProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidReslicePlaneSpatialObjectProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImageActor                        * m_ImageActor;
  vtkLookupTable                       * m_LUT;
  vtkImageMapToColors                  * m_MapColors;
  vtkImageReslice                      * m_ImageReslice;
  vtkMatrix4x4                         * m_ResliceAxes;

  /** Variables that store window and level values for 2D image display */
  double                                 m_Level;
  double                                 m_Window;

  /** Variables for managing image spatial object type */
  ImageSpatialObjectPointer     m_ImageSpatialObjectToBeSet;
  ImageSpatialObjectPointer     m_ImageSpatialObject;

  /** Variables for maanging reslice plane spatial object */
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObject;
  
  /** Observer to the VTK image events */
  typename VTKImageObserver::Pointer         m_VTKImageObserver;
  typename ImageTransformObserver::Pointer   m_ImageTransformObserver;

  /** Transform containing the information about image origin
   *  and image orientation taken from the DICOM input image */
  Transform                                  m_ImageTransform;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageResliceSpatialObjectRepresentation.txx"
#endif


#endif // __igstkImageResliceSpatialObjectRepresentation_h
