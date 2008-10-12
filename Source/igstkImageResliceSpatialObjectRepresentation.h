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

class vtkLookupTable;
class vtkImageMapToColors;
class vtkImageReslice;
class vtkTexture;
class vtkPolyDataMapper;
class vtkImageActor;
class vtkPlaneSource;


namespace igstk
{
 

/** \class ImageResliceSpatialObjectRepresentation
 * 
 * \brief This class represents an oblique image object. 
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

  typedef TImageSpatialObject                  ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                               ImageSpatialObjectConstPointer;

  typedef typename ImageSpatialObjectType::PointType  PointType;

  typedef ImageReslicePlaneSpatialObject< ImageSpatialObjectType >
                                                   ReslicePlaneSpatialObjectType;

  typedef typename ReslicePlaneSpatialObjectType::Pointer
                                              ReslicePlaneSpatialObjectPointer;  

  typedef itk::Vector< double, 3 > VectorType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  void RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
                                                             planeSpatialObject);

  /** Connect this representation class to the spatial object */
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * 
                                                 ImageSpatialObject );

  /** Type used for representing the slice number */
  typedef unsigned int SliceNumberType;

  /** Set the Window Level for the representation */
  void SetWindowLevel( double window, double level );

  /** Sets the visibility for the representation */
  void SetVisibility(bool visible);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 


protected:

  /** Constructor */
  ImageResliceSpatialObjectRepresentation();
  
  /** Destructor */
  ~ImageResliceSpatialObjectRepresentation();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  ImageResliceSpatialObjectRepresentation(const Self&);
  void operator=(const Self&);   //purposely not implemented

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer         m_ImageSpatialObject;
  ImageSpatialObjectConstPointer         m_ImageSpatialObjectToAdd;

   /** Variables for maanging reslice plane spatial object */
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObject;

    
  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImageActor                        * m_ImageActor;
  //vtkTexture                           * m_Texture;
  vtkPlaneSource                       * m_PlaneSource;
  vtkLookupTable                       * m_LUT;
  vtkImageMapToColors                  * m_MapColors;
  vtkImageReslice                      * m_ImageReslice;
  //vtkPolyDataMapper                    * m_TextureMapper;
  vtkTransform                         * m_ResliceTransform;
  vtkMatrix4x4                         * m_ResliceAxes;
  

  int                  m_ImageDimension[3];
  double               m_ImageOrigin[3];
  double               m_ImageSpacing[3];
  int                  m_ImageExtent[6];
  double               m_ImageBounds[6];

  /** Camera member and parameters */
  vtkCamera                              * m_Camera;
  double                                 m_CameraDistance;

  /** Variables that store window and level values for 2D image display */
  double                                 m_Level;
  double                                 m_Window;

  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Null operation for State Machine transition */
  void NoProcessing(); 

  /** Set the reslice plane spatial object */
  void SetReslicePlaneSpatialObjectProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetImageSpatialObjectProcessing();

   /** Report invalid image spatial object type */
  void ReportInvalidImageSpatialObjectProcessing( void );

  /** Report invalid tool spatial object type */
  void ReportInvalidReslicePlaneSpatialObjectProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** ... */
  void GetVector1(double v1[3]);

  /** ... */
  void GetVector2(double v1[3]);

  /** Sets the vtkImageData from the ImageSpatialObject. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );

  /** Sets the vtkPlaneSource from the ImageReslicePlaneSpatialObject object. 
  * This method MUST be private in order to prevent unsafe access from the 
  * VTK plane source layer. */
  void SetPlane( const vtkPlaneSource * plane );
  
  /** Connect VTK pipeline */
  void ConnectVTKPipelineProcessing();
    
  /** Declare the observer that will receive a VTK image from the
   * ImageSpatialObject */
  igstkObserverMacro( VTKImage, VTKImageModifiedEvent,
                      EventHelperType::VTKImagePointerType);

  /** Declare the observer that will receive a VTK plane source from the
   * ImageResliceSpatialObject */
  igstkObserverMacro( VTKPlane, VTKPlaneModifiedEvent,
                      EventHelperType::VTKPlaneSourcePointerType);

  /** Declare the observer that will receive a slice bounds event from the
   * ImageSpatialObject */
  igstkObserverMacro( SliceBounds, IntegerBoundsEvent, 
                      EventHelperType::IntegerBoundsType );

  /** */
  igstkObserverMacro( ImageBounds, igstk::ImageBoundsEvent, 
                                  igstk::EventHelperType::ImageBoundsType );

  /** */
  igstkObserverMacro( ImageTransform, CoordinateSystemTransformToEvent, 
     CoordinateSystemTransformToResult );

  typename VTKImageObserver::Pointer  m_VTKImageObserver;
  typename ImageTransformObserver::Pointer   m_ImageTransformObserver;
  typename VTKPlaneObserver::Pointer  m_VTKPlaneObserver;

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
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageResliceSpatialObjectRepresentation.txx"
#endif


#endif // __igstkImageResliceSpatialObjectRepresentation_h
