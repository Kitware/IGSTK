
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

/** \class ImageSpatialObjectRepresentation
 * 
 * \brief This class represents an image object. The parameters of the object
 * are ... The image object is rendered in a VTK scene using the
 * ... object.
 * 
 * \ingroup ObjectRepresentation
 */

class ImageSpatialObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Typedefs */
  typedef ImageSpatialObjectRepresentation         Self;
  typedef ObjectRepresentation                     Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;
  itkStaticConstMacro(ImageDimension, unsigned int, 3);
  typedef signed short                             PixelType;
  typedef itk::Image< PixelType, ImageDimension >  ImageType;
  typedef itk::ImageSeriesReader< ImageType >      ReaderType;
  typedef ImageSpatialObject                       ImageSpatialObjectType;

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

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

private:

  /** Internal itkSpatialObject */
  ImageSpatialObjectType::ConstPointer   m_ImageSpatialObject;

  /** VTK classes that support display of an image */
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

private:

  /** Inputs to the State Machine */
  InputType            m_ValidImageSpatialObjectInput;
  InputType            m_NullImageSpatialObjectInput;
  
  /** States for the State Machine */
  StateType            m_NullImageSpatialObjectState;
  StateType            m_ValidImageSpatialObjectState;

  ImageSpatialObjectType::ConstPointer m_ImageSpatialObjectToAdd;
};


} // end namespace igstk

#endif // __igstkImageSpatialObjectRepresentation_h
