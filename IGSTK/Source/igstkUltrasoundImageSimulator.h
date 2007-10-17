/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundImageSimulator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUltrasoundImageSimulator_h
#define __igstkUltrasoundImageSimulator_h

#include "igstkMacros.h"
#include "igstkImageSpatialObject.h"
#include "igstkUSImageObject.h"
#include "igstkStateMachine.h"


#include "vtkImageReslice.h"
#include "itkVTKImageImport.h"
#include "vtkImageExport.h"

namespace igstk
{

namespace Friends 
{

/** \class UltrasoundImageSimulatorToImageSpatialObject 
 *
 * \brief This class is intended to make the connection between the
 * UltrasoundImageSimulator and its output the ImageSpatialObject.
 * 
 * With this class it is possible to enforce encapsulation of the
 * SpatialObjectRepresentation and the ImageSpatialObject, and make their
 * GetImage() and SetImage() methods private, so that developers cannot gain
 * access to the ITK or VTK layers of these two classes.
 *
 */
class UltrasoundImageSimulatorToImageSpatialObject
{

public:
  template < class TUltrasoundImageSimulator, class TImageSpatialObject >
  static void 
  GetVTKImage( const TImageSpatialObject * imageSpatialObject,
                TUltrasoundImageSimulator * ultrasoundImageSimulator )
    {
    ultrasoundImageSimulator->SetImage(imageSpatialObject->GetVTKImageData());
    }

  template < class TUltrasoundImageSimulator, class TImageSpatialObject >
  static void 
  SetITKImage( const TUltrasoundImageSimulator * simulator, 
               TImageSpatialObject * imageSpatialObject )
    {
    imageSpatialObject->RequestSetImage( simulator->GetITKImage() );  
    }

}; // end of UltrasoundImageSimulatorToImageSpatialObject class

} // end of Friend namespace


/** \class UltrasoundImageSimulator
 * 
 * \brief This class simulates an ultrasound image by extracting a slice
 * from a 3D MR or CT image. 
 * 
 * Since most ultrasound systems provide images via a live feed, it is
 * difficult to test Ultrasound applications offline. This class is intended to
 * provide a simulation of Ultrasound images by reading a CT or MR image and
 * extracting an slice from it. 
 *
 * \warning This class should ONLY be used for testing purposes, and not as
 * part of a final application.
 *
 * \ingroup  Simulators
 */
template < class TImageGeometricModel >
class UltrasoundImageSimulator : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( 
                UltrasoundImageSimulator,Object)

public:

  typedef TImageGeometricModel               ImageGeometricModelType;
  typedef Transform                          TransformType;

 
  /** Request to set vector 2 on the plane */
  void RequestSetTransform( const TransformType & transform );

  /** Connect this representation class to the spatial object */
  void RequestSetImageGeometricModel( const ImageGeometricModelType * 
                                                 imageGeometricObject );

  /** Request reslice a 3D image */
  void RequestReslice();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Declare the UltrasoundImageSimulator class to be a friend 
   *  in order to give it access to the private method GetVTKImage(). */
  igstkFriendClassMacro( igstk::Friends::\
                UltrasoundImageSimulatorToImageSpatialObject );

  /** Request to get the output image as an event */
  void RequestGetImage();
  
  /** Event type */
  igstkLoadedObjectEventMacro( ImageModifiedEvent, IGSTKEvent, USImageObject);

protected:

  /** Constructor */
  UltrasoundImageSimulator();
  
  /** Destructor */
  ~UltrasoundImageSimulator();

  /** Types required for internal implementation */
  typedef typename ImageGeometricModelType::ConstPointer 
                                              ImageGeometricModelConstPointer;

  typedef typename ImageGeometricModelType::PointType  PointType;
  typedef typename USImageObject::ImageType            USImageType;
  typedef typename ImageGeometricModelType::ImageType  MRImageType;
 
private:

  UltrasoundImageSimulator(const Self&);   // purposely not implemented
  void operator=(const Self&);             // purposely not implemented

  /** Internal itkSpatialObject */
  ImageGeometricModelConstPointer       m_ImageGeometricModel;
  ImageGeometricModelConstPointer       m_ImageGeometricModelToAdd;
    
  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImageData                         * m_ReslicedImageData;
  vtkImageReslice                      * m_ImageReslice;

  typedef itk::VTKImageImport<MRImageType> VTKImageImporterType;

  typename VTKImageImporterType::Pointer  m_VTKImageImporter;

  TransformType                           m_Transform;
  TransformType                           m_TransformToBeSet;

  USImageObject::Pointer                  m_USImage;
  USImageType::Pointer                    m_RescaledUSImage;
  vtkImageExport*                         m_VTKExporter;

  /** Null operation for State Machine transition */
  void NoProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetImageGeometricModelProcessing();

  /** Set the transform for the plane */
  void SetTransformProcessing();

  /** Reslice processing */
  void ResliceProcessing ();

  /** Sets the vtkImageData from the spatial object. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );
  
  /** This function reports the image */
  void ReportImageProcessing();

  /** Declare the observer that will receive a VTK image from the
   * ImageSpatialObject */
  igstkObserverMacro( VTKImage, VTKImageModifiedEvent,
                      EventHelperType::VTKImagePointerType);
   
private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImageSpatialObject );
  igstkDeclareInputMacro( NullImageSpatialObject );
  
  igstkDeclareInputMacro( ValidTransform );
  igstkDeclareInputMacro( InvalidTransform );
     
  igstkDeclareInputMacro( EmptyImageSpatialObject );
  igstkDeclareInputMacro( ConnectVTKPipeline );
 
  igstkDeclareInputMacro( Reslice );
  igstkDeclareInputMacro( GetImage );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullImageSpatialObject );
  igstkDeclareStateMacro( ValidImageSpatialObject );

  igstkDeclareStateMacro( NullTransform );
  igstkDeclareStateMacro( ValidTransform );

  // FIXME : This must be replaced with StateMachine logic
  const USImageType * GetITKImage() const;

  typename VTKImageObserver::Pointer m_VTKImageObserver;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkUltrasoundImageSimulator.txx"
#endif


#endif // __igstkUltrasoundImageSimulator_h
