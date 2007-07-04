/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGenericImageSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkGenericImageSpatialObject_h
#define __igstkGenericImageSpatialObject_h

#include "igstkSpatialObject.h"

#include "itkImageBase.h"

class vtkImageData;
class vtkImageImport;

namespace igstk
{

namespace Friends 
{
class ImageReaderToGenericImageSpatialObject;
class UltrasoundImageSimulatorToGenericImageSpatialObject;
}


/** \class GenericImageSpatialObject
 * 
 * \brief This class represents an image object. 
 * 
 * This class is intended to store a generic image for which the modality and
 * pixel type can be determined at run time.  It is intended to be used only in
 * prototype applications and its use in applications targeted to the surgery
 * room is strongly discouraged.
 *
 * Internally it can make available both image formats to ITK and VTK classes.
 * The ITK and VTK layers are concealed in order to enforce the safety of the
 * IGSTK layer.
 *
 * \ingroup Object
 */
class GenericImageSpatialObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( GenericImageSpatialObject, SpatialObject );

public:

  typedef itk::SpatialObject<3>                 GenericSpatialObjectType;

  typedef itk::ImageBase< 3 >                   ImageBaseType;
  typedef ImageBaseType::ConstPointer           ImageConstPointer;

  typedef itk::Point<double,3>                  PointType;
  typedef itk::Index<3>                         IndexType;
  typedef itk::ContinuousIndex< double, 3 >     ContinuousIndexType;

  /** Test whether a point is inside or outside the object. */
  virtual bool IsInside( const PointType & point ) const;

  /** Transform physical point to index. */
  virtual bool TransformPhysicalPointToIndex ( const PointType & point, 
                                             IndexType & index ) const;

  /** Transform physical point to index. */
  virtual bool TransformPhysicalPointToContinuousIndex ( 
                                     const PointType & point, 
                                     ContinuousIndexType & index ) const;

  /** Returns wheter the image have any pixels allocated or not */
  virtual bool IsEmpty() const;

  /** The ImageReaderToGenericImageSpatialObject class is declared as a friend
   * in order to be able to set the input image */
  igstkFriendClassMacro( 
    igstk::Friends::ImageReaderToGenericImageSpatialObject );

  /** The UltrasoundImageSimulatorToGenericImageSpatialObject class is declared
   * as a friend in order to be able to set the input image */
  igstkFriendClassMacro( 
      igstk::Friends::UltrasoundImageSimulatorToGenericImageSpatialObject );

  /** Request to get the ITK image as a const pointer payload into an event.
      Both the const and non-const versions are needed. */
  void RequestGetITKImage();
  void RequestGetITKImage() const;

  /** Request to get the VTK image as a const pointer payload into an event.
   *  Both the const and non-const versions are needed. */
  void RequestGetVTKImage();
  void RequestGetVTKImage() const;

  /** Event types */
  igstkLoadedConstObjectEventMacro( 
    ITKImageModifiedEvent, IGSTKEvent, ImageBaseType);

  igstkEventMacro( ImageNotAvailableEvent, IGSTKEvent );

protected:

  GenericImageSpatialObject( void );
  ~GenericImageSpatialObject( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkGenericImageSpatialObject */
  GenericSpatialObjectType::Pointer       m_GenericSpatialObject;

  /** Set method to be invoked only by friends of this class */
  void RequestSetImage( const ImageBaseType * image );

  /** Declarations needed for the Logger */
  igstkLoggerMacro();


private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  GenericImageSpatialObject(const Self&);     //purposely not implemented
  void operator=(const Self&);         //purposely not implemented

private:

  /** State Machine Inputs */
  igstkDeclareInputMacro( ValidImage );
  igstkDeclareInputMacro( InvalidImage );
  igstkDeclareInputMacro( RequestITKImage );
  igstkDeclareInputMacro( RequestVTKImage );
  
  /** State Machine States */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ImageSet );

  /** This method is intended to be called by the state machine */
  void SetImageProcessing();
  void ReportInvalidImageProcessing();

  /** This function reports the image */
  void ReportITKImageProcessing();
  void ReportVTKImageProcessing();
  void ReportImageNotAvailableProcessing();

  /** This is the variable holding the real data container in the form of an
   * ITK image. This image should never be exposed at the IGSTK API. */
  ImageConstPointer  m_ImageToBeSet;
  ImageConstPointer  m_Image;

  /** Filters for exporting the ITK image as a vtkImageData class. 
   *  This VTK representation should never be exposed to the IGSTK API. */
  typedef vtkImageImport             VTKImportFilterType;
  typedef VTKImportFilterType      * VTKImportFilterPointer;

  /** Classes to connect an ITK pipeline to a VTK pipeline */
  itk::ProcessObject::Pointer        m_ItkExporter;
  VTKImportFilterPointer             m_VtkImporter;

};

} // end namespace igstk

#endif // __igstkGenericImageSpatialObject_h
