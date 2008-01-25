#ifndef CXIMAGESLICE2DREPRESENTATION_H_
#define CXIMAGESLICE2DREPRESENTATION_H_

#include <igstkMacros.h>
#include <igstkObjectRepresentation.h>
#include <igstkImageSpatialObject.h>
#include <igstkStateMachine.h>

#include <vtkCommand.h>

#include "vtkImagePlaneWidget2D.h"

namespace cx
{
 
/** @class ImageSlice2DRepresentation
 * 
 * @brief This class represents an image object as a slice of the image projected
 * onto a plane. 
 * The statemachine and most parts of this class has been copied from 
 * (igstk)ImageSpatialObjectRepresentation.
 * 
 * @Author Torleif Sandnes, Sintef Health Research
 * @Date 20070823
 */

template < class TImageSpatialObject >
class ImageSlice2dRepresentation : public igstk::ObjectRepresentation
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro(ImageSlice2dRepresentation, \
                                         igstk::ObjectRepresentation )

  typedef TImageSpatialObject ImageSpatialObjectType;

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
  void RequestSetImageSpatialObject(const ImageSpatialObjectType * 
                                    ImageSpatialObject );

  /** Type used for representing the slice number */
  typedef unsigned int SliceNumberType;

  /** Request the state machine to attempt to select a slice number */
  void RequestSetSliceNumber(SliceNumberType slice);

  /** Request the state machine to attempt to select a slice orientation */
  void RequestSetOrientation(OrientationType orientation);

  /** Set the Window Level for the representation */
  void SetWindowLevel(double window, double level);
  
  /** Set the opacity */
  void SetOpacity(float alpha);
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Returns the Minimum and Maximum number of slice available in the current
   * orientation.  */
  void RequestGetSliceNumberBounds();
  
  void SetInteractor(vtkRenderWindowInteractor * interactor)
  { m_ImagePlane->SetInteractor(interactor);}

  // Necessary to synch other imageplanewidgets 
  void RequestSetCursorPosition(double x, double y, double z)
    { m_ImagePlane->SetCursorPosition(x, y, z);}

  void ConnectWindowLevel(typename Self::Pointer representation)
    {
      this->m_ImagePlane->ConnectWindowLevel(representation->m_ImagePlane);
    }
  void Enable(bool enable) 
  { 
    if((m_ImagePlane->GetEnabled()) && !enable)
      m_ImagePlane->SetEnabled(false);
    else if( (!m_ImagePlane->GetEnabled()) && enable)
      m_ImagePlane->SetEnabled(true);
  }
  
protected:

  /** Constructor */
  ImageSlice2dRepresentation();
  
  /** Destructor */
  ~ImageSlice2dRepresentation();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Observer macro that will received a event with an image as payload and
   * will store it internally. This will be the receptor of the event sent by
   * the ImageSpatialObject when an image is requested. */
   igstkObserverMacro( VTKImage, igstk::VTKImageModifiedEvent, 
                       igstk::EventHelperType::VTKImagePointerType );


private:

  ImageSlice2dRepresentation(const Self&);   //purposely not implemented
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

  static void CursoringCallback(vtkObject *, unsigned long eventId, void *clientData, 
                         void *callData);
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

}

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "cximageslice2drepresentation.txx"
#endif

#endif
