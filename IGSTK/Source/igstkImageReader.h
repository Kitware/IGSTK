/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageReader_h
#define __igstkImageReader_h

#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkObject.h"

#include "itkImageSpatialObject.h"

namespace igstk
{
 
namespace Friends 
{

/** \class ImageReaderToImageSpatialObject
 * 
 * \brief This class is intended to make the connection between the ImageReader
 * and its output, the ImageSpatialObject. 
 *
 * With this class it is possible to enforce encapsulation of the Reader and
 * the ImageSpatialObject, and make their GetImage() and SetImage() methods
 * private, so that developers cannot gain access to the ITK or VTK layers of
 * these two classes.
 *
 */
class ImageReaderToImageSpatialObject
{
public:

  template < class TReader, class TImageSpatialObject >
  static void 
  ConnectImage( const TReader * reader, 
                TImageSpatialObject * imageSpatialObject )
    {
    imageSpatialObject->RequestSetImage( reader->GetITKImage() );  
    }

}; // end of ImageReaderToImageSpatialObject class

} // end of Friend namespace


  
/** \class ImageReader
 * 
 * \brief This class is a base class for all image reader classes.
 * 
 * This class reads image data stored in files and outputs
 * image spatial object. This class is templated over pixeltype and dimension 
 * parameters
 *
 * \ingroup Object
 */
template < class TImageSpatialObject >
class ImageReader : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedAbstractClassTraitsMacro( ImageReader, Object )

public:

  /** Some convenient typedefs for input image */
  typedef TImageSpatialObject                   ImageSpatialObjectType;  

  /** Declare the ImageReaderToImageSpatialObject class to be a friend 
   *  in order to give it access to the private method GetITKImage(). */
  igstkFriendClassMacro( igstk::Friends::ImageReaderToImageSpatialObject );

protected:

  ImageReader();
  ~ImageReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  /** Connect the ITK image to the output ImageSpatialObject */
  void ConnectImage();

  /** Some convenient typedefs for internal ITK image. 
   *  These types must not be exposed in the API of this class. */
  typedef typename ImageSpatialObjectType::ImageType  ImageType;
  typedef typename ImageType::ConstPointer            ImagePointer;
  typedef typename ImageType::RegionType              ImageRegionType; 

  typename ImageSpatialObjectType::Pointer   m_ImageSpatialObject;

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  ImageReader(const Self&);         //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

  // FIXME : This must be replaced with StateMachine logic
  virtual const ImageType * GetITKImage() const = 0;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageReader.txx"
#endif

#endif // __igstkImageReader_h
