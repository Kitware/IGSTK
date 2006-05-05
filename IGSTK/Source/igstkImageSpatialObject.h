/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSpatialObject_h
#define __igstkImageSpatialObject_h

#include "igstkSpatialObject.h"

#include "itkImageSpatialObject.h"

#include "itkVTKImageExport.h"
#include "vtkImageImport.h"
#include "vtkImageData.h"

namespace igstk
{

namespace Friends 
{
class ImageReaderToImageSpatialObject;
class ImageSpatialObjectRepresentationToImageSpatialObject;
class ObliqueImageSpatialObjectRepresentationToImageSpatialObject;
class UltrasoundImageSimulatorToImageSpatialObject;
}

class MR3DImageToUS3DImageRegistration;


/** \class ImageSpatialObject
 * 
 * \brief This class represents an image object. 
 * 
 * This class is the base for all the image data objects in the toolkit. I
 * associates an internal ITK image and a VTK importer in such a way that
 * internally it can make available both image format to ITK and VTK classes.
 * The ITK and VTK layers are concealed in order to enforce the safety of the
 * IGSTK layer.
 *
 * \ingroup Object
 */
template < class TPixelType, unsigned int TDimension >
class ImageSpatialObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ImageSpatialObject, SpatialObject )

public:

  typedef itk::ImageSpatialObject< TDimension, TPixelType > 
                                                       ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ImageType        ImageType;
  typedef typename ImageType::ConstPointer                  ImageConstPointer;
  typedef typename ImageSpatialObjectType::PointType        PointType;
  typedef typename ImageType::IndexType                     IndexType;

  /** Test whether a point is inside or outside the object. */
  virtual bool IsInside( const PointType & point ) const;

  /** Transform physical point to index. */
  virtual bool TransformPhysicalPointToIndex ( const PointType & point, 
                                             IndexType & index ) const;

  /** Returns wheter the image have any pixels allocated or not */
  virtual bool IsEmpty() const;

  /** The ImageReaderToImageSpatialObject class is declared as a friend in
   * order to be able to set the input image */
  igstkFriendClassMacro( igstk::Friends::ImageReaderToImageSpatialObject );
  igstkFriendClassMacro( 
     igstk::Friends::ImageSpatialObjectRepresentationToImageSpatialObject );
  igstkFriendClassMacro( igstk::Friends::
              ObliqueImageSpatialObjectRepresentationToImageSpatialObject );
  igstkFriendClassMacro( igstk::Friends::
                             UltrasoundImageSimulatorToImageSpatialObject );
  igstkFriendClassMacro( igstk::MR3DImageToUS3DImageRegistration );

  /** Event type */
  igstkLoadedTemplatedConstObjectEventMacro( ITKImageModifiedEvent, IGSTKEvent,
                                             ImageType);


protected:

  ImageSpatialObject( void );
  ~ImageSpatialObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Get the ITK image as a const pointer
   *  This is only accessible friend classes */
  void RequestGetITKImage();

private:

  /** Internal itkImageSpatialObject */
  typename ImageSpatialObjectType::Pointer       m_ImageSpatialObject;

  /** Get the VTK image data (converted from the ITK pipeline) */
  const vtkImageData * GetVTKImageData() const;

  /** Set method to be invoked only by friends of this class */
  void RequestSetImage( const ImageType * image );

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  ImageSpatialObject(const Self&);     //purposely not implemented
  void operator=(const Self&);         //purposely not implemented

private:

  /** State Machine Inputs */
  igstkDeclareInputMacro( InvalidImage );
  igstkDeclareInputMacro( ValidImage );
  
  /** State Machine States */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ImageSet );
  igstkDeclareInputMacro( RequestITKImage );

  /** This method is intended to be called by the state machine */
  void SetImageProcessing();
  void ReportInvalidImageProcessing();

  /** This function reports the image */
  void ReportITKImageProcessing();

  /** This is the variable holding the real data container in the form of an
   * ITK image. This image should never be exposed at the IGSTK API. */
  ImageConstPointer  m_ImageToBeSet;
  ImageConstPointer  m_Image;

  /** Filters for exporting the ITK image as a vtkImageData class. 
   *  This VTK representation should never be exposed to the IGSTK API. */
  typedef itk::VTKImageExport< ImageType >      ITKExportFilterType;
  typedef vtkImageImport                        VTKImportFilterType;

  typedef typename ITKExportFilterType::Pointer ITKExportFilterPointer;
  typedef          VTKImportFilterType      *   VTKImportFilterPointer;

  /** Classes to connect an ITK pipeline to a VTK pipeline */
  ITKExportFilterPointer             m_itkExporter;
  VTKImportFilterPointer             m_vtkImporter;


};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageSpatialObject.txx"
#endif

#endif // __igstkImageSpatialObject_h
