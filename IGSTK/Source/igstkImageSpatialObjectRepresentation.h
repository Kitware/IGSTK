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
#include "vtkImageReslice.h"

namespace igstk
{
 
namespace Friends 
{

/** \class ImageSpatialObjectRepresentationToImageSpatialObject 
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
class ImageSpatialObjectRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ImageSpatialObjectRepresentation, \
                                          ObjectRepresentation )

public:

  typedef TImageSpatialObject                      ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                           ImageSpatialObjectConstPointer;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Orientation Type: Publically declared
   * orientation types supported for slice viewing. */
  typedef enum
    { 
    Sagittal, 
    Coronal, 
    Axial,
    Oblique 
    } 
  OrientationType;

  /** Connect this representation class to the spatial object */
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * 
                                                ImageSpatialObject );

  /** Type used for representing the slice number */
  typedef unsigned int SliceNumberType;

  /** Request the state machine to attempt to select a slice number */
  void RequestSetSliceNumber( SliceNumberType slice );

  /** Request the state machine to attempt to select a slice orientation */
  void RequestSetOrientation( OrientationType orientation );

  /** Set the Window Level for the representation */
  void SetWindowLevel( double window, double level );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Declare the ImageReaderToImageSpatialObject class to be a friend 
   *  in order to give it access to the private method GetITKImage(). */
  igstkFriendClassMacro( 
     igstk::Friends::ImageSpatialObjectRepresentationToImageSpatialObject );

  /** Returns the Minimum and Maximum number of slice available in the current
   * orientation.  */
  void RequestGetSliceNumberBounds();

protected:

  /** Constructor */
  ImageSpatialObjectRepresentation();
  
  /** Destructor */
  ~ImageSpatialObjectRepresentation();

  /** Connect the VTK image from the ImageSpatialObject to the
   * ImageSpatialObjectRepresentation*/
  void ConnectImage();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

private:

  ImageSpatialObjectRepresentation(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer         m_ImageSpatialObject;
  ImageSpatialObjectConstPointer         m_ImageSpatialObjectToAdd;

  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImageActor                        * m_ImageActor;
  vtkLookupTable                       * m_LUT;
  vtkImageMapToColors                  * m_MapColors;
  vtkImageReslice                      * m_ImageReslice;

  /** Variables that store window and level values for 2D image display */
  double                                 m_Level;
  double                                 m_Window;

  /** Update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Null operation for State Machine transition */
  void NoProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetImageSpatialObjectProcessing();

  /** Sets the vtkImageData from the spatial object. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );
  
  /** Attempt to set the Slice Number. */
  void AttemptSetSliceNumberProcessing();

  /** Actually set the Slice Number. */
  void SetSliceNumberProcessing();

  /** Actually set the Slice Orientation. */
  void SetOrientationProcessing();
      
  /** Reports the minimum and maximum slice numbers on the current
   *  orientation */
  void ReportSliceNumberBoundsProcessing();

  /** Connect VTK pipeline */
  void ConnectVTKPipelineProcessing();
    
private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImageSpatialObject );
  igstkDeclareInputMacro( NullImageSpatialObject );
  igstkDeclareInputMacro( EmptyImageSpatialObject );
  igstkDeclareInputMacro( SetSliceNumber );
  igstkDeclareInputMacro( ValidSliceNumber );
  igstkDeclareInputMacro( InvalidSliceNumber );
  igstkDeclareInputMacro( ValidOrientation );
  igstkDeclareInputMacro( RequestSliceNumberBounds ); 
  igstkDeclareInputMacro( ConnectVTKPipeline );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullImageSpatialObject );
  igstkDeclareStateMacro( ValidImageSpatialObject );
  igstkDeclareStateMacro( ValidImageOrientation );
  igstkDeclareStateMacro( ValidSliceNumber );
  igstkDeclareStateMacro( AttemptingToSetSliceNumber );

  /** Variables for managing the Slice number through the StateMachine */
  SliceNumberType      m_SliceNumberToBeSet;
  SliceNumberType      m_SliceNumber;

  /** Variables for managing the Orientation of the slices */
  OrientationType      m_OrientationToBeSet;
  OrientationType      m_Orientation;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageSpatialObjectRepresentation.txx"
#endif


#endif // __igstkImageSpatialObjectRepresentation_h
