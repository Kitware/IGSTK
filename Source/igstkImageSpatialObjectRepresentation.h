/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSpatialObjectRepresentation_h
#define __igstkImageSpatialObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

#include "vtkImageActor.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"

namespace igstk
{
 
namespace Friends 
{

/** class ImageSpatialObjectRepresentationToImageSpatialObject 
 *
 * \brief This class is intended to make the connection between the
 * ImageSpatialObjectRepresentation and its output, the ImageSpatialObject.
 * With this class it is possible to enforce encapsulation of the
 * SpatialObjectRepresentation and the ImageSpatialObject, and make their
 * GetImage() and SetImage() methods private, so that developers cannot gain
 * access to the ITK or VTK layers of these two classes.
 *
 */
class ImageSpatialObjectRepresentationToImageSpatialObject
{
  public:
    template < class TSpatialObjectRepresentation, class TImageSpatialObject >
    static void 
    ConnectImage( const TImageSpatialObject * imageSpatialObject,
                  TSpatialObjectRepresentation * imageSpatialObjectRepresentation )
    {
       imageSpatialObjectRepresentation->SetImage( 
                 imageSpatialObject->GetVTKImageData() );  
    }

}; // end of ImageSpatialObjectRepresentationToImageSpatialObject class

} // end of Friend namespace



/** \class ImageSpatialObjectRepresentation
 * 
 * \brief This class represents an image object. The parameters of the object
 * are ... The image object is rendered in a VTK scene using the
 * ... object.
 * 
 * \ingroup ObjectRepresentation
 */

  template < class TImageSpatialObject >
class ImageSpatialObjectRepresentation 
                      : public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef ImageSpatialObjectRepresentation         Self;
  typedef ObjectRepresentation                     Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  typedef TImageSpatialObject                      ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                                   ImageSpatialObjectConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( ImageSpatialObjectRepresentation, ObjectRepresentation );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * ImageSpatialObject );

  ImageSpatialObjectRepresentation( void );
  ~ImageSpatialObjectRepresentation( void );

  void SetZSlice( int slice );

  void SetWindowLevel( double window, double level );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Declare the ImageReaderToImageSpatialObject class to be a friend 
   *  in order to give it access to the private method GetITKImage(). */
  igstkFriendClassMacro( igstk::Friends::ImageSpatialObjectRepresentationToImageSpatialObject );

  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

protected:

  /** Connect the VTK image from the ImageSpatialObject to the
   * ImageSpatialObjectRepresentation*/
  void ConnectImage();

private:

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer         m_ImageSpatialObject;
  ImageSpatialObjectConstPointer         m_ImageSpatialObjectToAdd;

  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImageActor                        * m_ImageActor;
  vtkLookupTable                       * m_LUT;
  vtkImageMapToColors                  * m_MapColors;

  /** Variables that store window and level values for 2D image display */
  double                                 m_Level;
  double                                 m_Window;

  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentation();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetImageSpatialObject();

  /** Sets the vtkImageData from the spatial object. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );
  
private:

  /** Inputs to the State Machine */
  InputType            m_ValidImageSpatialObjectInput;
  InputType            m_NullImageSpatialObjectInput;
  
  /** States for the State Machine */
  StateType            m_NullImageSpatialObjectState;
  StateType            m_ValidImageSpatialObjectState;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageSpatialObjectRepresentation.txx"
#endif


#endif // __igstkImageSpatialObjectRepresentation_h

