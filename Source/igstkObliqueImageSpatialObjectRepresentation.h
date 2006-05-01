/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueImageSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkObliqueImageSpatialObjectRepresentation_h
#define __igstkObliqueImageSpatialObjectRepresentation_h

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

/** \class ObliqueImageSpatialObjectRepresentationToImageSpatialObject 
 *
 * \brief This class is intended to make the connection between the
 * ObliqueImageSpatialObjectRepresentation and its output, 
 * the ImageSpatialObject.
 * With this class it is possible to enforce encapsulation of the
 * SpatialObjectRepresentation and the ImageSpatialObject, and make their
 * GetImage() and SetImage() methods private, so that developers cannot gain
 * access to the ITK or VTK layers of these two classes.
 *
 */
class ObliqueImageSpatialObjectRepresentationToImageSpatialObject
{

public:
  template < class TSpatialObjectRepresentation, class TImageSpatialObject >
  static void 
  ConnectImage( const TImageSpatialObject * imageSpatialObject,
                TSpatialObjectRepresentation * 
                obliqueImageSpatialObjectRepresentation )
    {
    obliqueImageSpatialObjectRepresentation->SetImage(
                                      imageSpatialObject->GetVTKImageData() );  
    }

}; // end of ObliqueImageSpatialObjectRepresentationToImageSpatialObject class

} // end of Friend namespace


/** \class ObliqueImageSpatialObjectRepresentation
 * 
 * \brief This class represents an oblique image object. 
 * 
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class ObliqueImageSpatialObjectRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( 
                ObliqueImageSpatialObjectRepresentation,ObjectRepresentation )

public:

  typedef TImageSpatialObject                  ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                               ImageSpatialObjectConstPointer;

  typedef typename ImageSpatialObjectType::PointType  PointType;

  typedef itk::Vector< double, 3 > VectorType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Request to set origin point on the plane */
  void RequestSetOriginPointOnThePlane( const PointType & point);

  /** Request to set vector 1 on the plane */
  void RequestSetVector1OnThePlane( const VectorType & vector );

  /** Request to set vector 2 on the plane */
  void RequestSetVector2OnThePlane( const VectorType & vector );

  /** Connect this representation class to the spatial object */
  void RequestSetImageSpatialObject( const ImageSpatialObjectType * 
                                                 ImageSpatialObject );

  /** Request reslice a 3D image */
  void RequestReslice();

  /** Type used for representing the slice number */
  typedef unsigned int SliceNumberType;

  /** Set the Window Level for the representation */
  void SetWindowLevel( double window, double level );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Declare the ObliqueImageSpatialObjectRepresentation class to be a friend 
   *  in order to give it access to the private method GetVTKImage(). */
  igstkFriendClassMacro( igstk::Friends::\
                ObliqueImageSpatialObjectRepresentationToImageSpatialObject );

protected:

  /** Constructor */
  ObliqueImageSpatialObjectRepresentation();
  
  /** Destructor */
  ~ObliqueImageSpatialObjectRepresentation();

  /** Connect the VTK image from the ImageSpatialObject to the
   * ObliqueImageSpatialObjectRepresentation*/
  void ConnectImage();

  /** Overloaded function to delete actors */
  void DeleteActors();

  /** Create the VTK actors for displaying geometry */
  void CreateActors();

private:

  ObliqueImageSpatialObjectRepresentation(const Self&);
  void operator=(const Self&);   //purposely not implemented

  /** Internal itkSpatialObject */
  ImageSpatialObjectConstPointer         m_ImageSpatialObject;
  ImageSpatialObjectConstPointer         m_ImageSpatialObjectToAdd;

  /** Oblique plane parameters */
  PointType                              m_OriginPointOnThePlane;
  PointType                              m_OriginPointOnThePlaneToBeSet;

  VectorType                             m_Vector1OnThePlane;
  VectorType                             m_Vector1OnThePlaneToBeSet;

  VectorType                             m_Vector2OnThePlane;
  VectorType                             m_Vector2OnThePlaneToBeSet;

    
  /** VTK classes that support display of an image */
  vtkImageData                         * m_ImageData;
  vtkImageData                         * m_ReslicedImageData;
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

  /** Set the origin point on the plane */
  void SetOriginPointOnThePlaneProcessing();
  
  /** Set vector 1 on the plane */
  void SetVector1OnThePlaneProcessing();
 
  /** Set vector 2 on the plane */
  void SetVector2OnThePlaneProcessing();
 
  /** Reslice processing */
  void ResliceProcessing ();

  /** Sets the vtkImageData from the spatial object. This method MUST be
   * private in order to prevent unsafe access from the VTK image layer. */
  void SetImage( const vtkImageData * image );
  
  /** Connect VTK pipeline */
  void ConnectVTKPipelineProcessing();
    
private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImageSpatialObject );
  igstkDeclareInputMacro( NullImageSpatialObject );
  
  igstkDeclareInputMacro( ValidOriginPointOnThePlane );
  igstkDeclareInputMacro( InValidOriginPointOnThePlane );

  igstkDeclareInputMacro( ValidVector1OnThePlane );
  igstkDeclareInputMacro( InValidVector1OnThePlane );
 
  igstkDeclareInputMacro( ValidVector2OnThePlane );
  igstkDeclareInputMacro( InValidVector2OnThePlane );
     
  igstkDeclareInputMacro( EmptyImageSpatialObject );
  igstkDeclareInputMacro( ConnectVTKPipeline );
 
  igstkDeclareInputMacro( Reslice );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullImageSpatialObject );
  igstkDeclareStateMacro( ValidImageSpatialObject );

  igstkDeclareStateMacro( NullOriginPointOnThePlane );
  igstkDeclareStateMacro( ValidOriginPointOnThePlane );
  
  igstkDeclareStateMacro( NullVector1OnthePlane );
  igstkDeclareStateMacro( ValidVector1OnThePlane );

  igstkDeclareStateMacro( NullVector2OnthePlane );
  igstkDeclareStateMacro( ValidVector2OnThePlane );
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkObliqueImageSpatialObjectRepresentation.txx"
#endif


#endif // __igstkObliqueImageSpatialObjectRepresentation_h
