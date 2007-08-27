/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectImagePlaneRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSpatialObjectImagePlaneRepresentation_h
#define __igstkImageSpatialObjectImagePlaneRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkStateMachine.h"

#include "vtkImagePlaneWidget2D.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"

namespace igstk
{
 

/** @class ImageSpatialObjectImagePlaneRepresentation
 * 
 * @brief This class represents an image object as a slice of the image projected
 * onto a plane. 
 * The statemachine and most parts of this class has been copied from 
 * ImageSpatialObjectRepresentation.
 * 
 * @ingroup ObjectRepresentation
 * @Author Torleif Sandnes, Sintef Health Research
 * @Date 20070823
 */

template < class TImageSpatialObject >
class ImageSpatialObjectImagePlaneRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ImageSpatialObjectImagePlaneRepresentation, \
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
    Axial
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
  
  /** Set the opacity */
  void SetOpacity(float alpha);
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Returns the Minimum and Maximum number of slice available in the current
   * orientation.  */
  void RequestGetSliceNumberBounds();
  
  void SetInteractor(vtkRenderWindowInteractor * interactor)
  { m_ImagePlane->SetInteractor(interactor);}

  void RequestSetCursorPosition(double x, double y, double z)
  { m_ImagePlane->SetCursorPosition(x, y, z);}

protected:

  /** Constructor */
  ImageSpatialObjectImagePlaneRepresentation();
  
  /** Destructor */
  ~ImageSpatialObjectImagePlaneRepresentation();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Observer macro that will received a event with an image as payload and
   * will store it internally. This will be the receptor of the event sent by
   * the ImageSpatialObject when an image is requested. */
   igstkObserverMacro( VTKImage, VTKImageModifiedEvent, 
                       EventHelperType::VTKImagePointerType );


private:

  ImageSpatialObjectImagePlaneRepresentation(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

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

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer         m_ImageSpatialObject;
  ImageSpatialObjectConstPointer         m_ImageSpatialObjectToAdd;

  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImagePlaneWidget2D                * m_ImagePlane;

  /** Variables for managing the Slice number through the StateMachine */
  SliceNumberType      m_SliceNumberToBeSet;
  SliceNumberType      m_SliceNumber;

  /** Variables for managing the Orientation of the slices */
  OrientationType      m_OrientationToBeSet;
  OrientationType      m_Orientation;

  /** Observer to the VTK image events */
  typename VTKImageObserver::Pointer   m_VTKImageObserver;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkImageSpatialObjectImagePlaneRepresentation.txx"
#endif


#endif // __igstkImageSpatialObjectImagePlaneRepresentation_h
