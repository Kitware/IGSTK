/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkObjectRepresentation_h
#define __igstkObjectRepresentation_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <vector>
#include "itkCommand.h"
#include "igstkLogger.h"
#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkStateMachine.h"
#include "igstkCoordinateSystem.h"

class vtkProp;

namespace igstk
{

/** \class ObjectRepresentation
 * 
 * \brief An abstract base class for all the igstk representation objects.
 *
 * This class serves as the base class for all the representation objects that
 * will provide a VTK visualization of the Spatial Objects that are composing a
 * given scene.
 *
 * Due to the critical nature of IGSTK applications, it is important to ensure
 * that when we display objects in the surgical scene, we have confidence on
 * the validity of their current location and orientation in space. In IGSTK we
 * do this by managing Transforms with a finite validity time. In this way,
 * when an object stops receiving fresh updated transforms, its old transforms
 * are going to expire and the ObjectRepresentation class will then know that
 * at this moment we can not trust the information of the transform. Objects
 * whose transforms have expired are not displayed in the Views. This
 * functionality is implemented in this current class by providing states of
 * Visibility and Invisibility in an auxiliary state machine.
 *
 * The validity of the SpatialObject transform is checked at every call of the
 * RequestUpdateRepresentation() method. If the transform turns out to be
 * invalid, then this class goes into the Invisible state, and remains there
 * until subsequent calls to RequestUpdateRepresentation() reveal that a valid
 * transform has been provided to the SpatialObject.
 *
 *
 * \image html  igstkObjectRepresentation.png 
 *                  "Object Representation State Machine Diagram"
 * \image latex igstkObjectRepresentation.eps
 *                  "Object Representation State Machine Diagram"
 *
 * \ingroup Object
 */
class ObjectRepresentation 
  : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( ObjectRepresentation, Object )

public:

  typedef std::vector< vtkProp* >            ActorsListType; 

  /** Type for representing the color components */
  typedef     double          ColorScalarType;

  /** Set the color */
  void SetColor(ColorScalarType r, ColorScalarType g, ColorScalarType b);

  /** Get each color component */
  ColorScalarType GetRed() const; 
  ColorScalarType GetGreen() const;
  ColorScalarType GetBlue() const;

  /** Type for representing the opacity of the object. */
  typedef   double            OpacityType;

  /** Set/Get the opacity */
  virtual void SetOpacity(OpacityType alpha);
  igstkGetMacro( Opacity, OpacityType );

  /** Create the vtkActors */
  virtual void CreateActors()= 0;

  /** Get the VTK actors */
  igstkGetMacro( Actors, ActorsListType );

  /** Update the visual representation with changes in the geometry */
  virtual void RequestUpdateRepresentation( 
    const TimeStamp & time, 
    const CoordinateSystem* cs );

protected:

  ObjectRepresentation( void );
  ~ObjectRepresentation( void );

  ActorsListType              m_Actors;

  OpacityType                 m_Opacity;

  /** Add an actor to the list */
  void AddActor( vtkProp * );

  /** Empty the list of actors */
  virtual void DeleteActors();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Request the state machine to set a Spatial Object */
  void RequestSetSpatialObject( const SpatialObject * spatialObject );

  /** Verify the time stamp. A default implementation is provided that checks
    * if the spatial object transform is within the Rendering time period.
    * This method could be overridden in derived classes that can use other 
    * criteria to verify the time stamp. */
  virtual bool VerifyTimeStamp() const; 

  /** Get Time stamp for the time at which the next rendering will take place */
  TimeStamp GetRenderTimeStamp() const;
  
private:

  ObjectRepresentation(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** Update the visual representation with changes in the geometry. Only to be
   * called by the State Machine. This is an abstract method that MUST be
   * overloaded in every derived class. */
  virtual void UpdateRepresentationProcessing() = 0;

  /** This method checks the time stamp of the transform
   *  and modifies the visibility of the objects accordingly. */
  void RequestVerifyTimeStampAndUpdateVisibility();

  /** Null operation for a State Machine transition */
  void NoProcessing();


  /** Set the spatial object for this class */
  void SetSpatialObjectProcessing(); 

  /** Report when an invalid request is made to the StateMachine */
  void ReportInvalidRequestProcessing();

  /** Make Objects Invisible. 
   *  This method is called when the Transform time stamp
   *  has expired with respect to the requested rendering time. */
  void MakeObjectsInvisibleProcessing();

  /** Make Objects Visible. This method is called when the Transform time stamp
   * is valid with respect to the requested rendering time. */
  void MakeObjectsVisibleProcessing();

  /** Receive the Transform from the SpatialObject via a transduction macro.
   *  Once the transform is received, the validity time is verified. */
  void ReceiveSpatialObjectTransformProcessing();

  /** Receive TransformNotAvailable message from the SpatialObject via a
   * transduction macro.  Since no new transform is available, it checks
   * whether the validity time of the previously existing transform has not
   * expired and updates the visibility of the object accordingly. */
  void ReceiveTransformNotAvailableProcessing();

  /** This action calls the RequestGetTransform() method in the spatial
   *  object, and the answer is expected to be processed by the observer
   *  created in the transduction macro */
  void RequestGetTransformProcessing();

  /** Internal method to set an actor's visibility based on the 
   *  visibility state machine. */
  void RequestSetActorVisibility( vtkProp * );

  /** These two methods are used by the visibility state machine 
   *  to set an actor's visibility. */ 
  void SetActorVisibleProcessing();
  void SetActorInvisibleProcessing();

  /** Main color of the representation. This should be RGB components, each one
   * in the range 0.0 to 1.0 */
  ColorScalarType                         m_Color[3];

  /** The associated SpatialObject is non-const because we invoke requests
   * methods that indirectly will modify the state of its internal StateMachine
   * */
  SpatialObject::Pointer                  m_SpatialObject;

  /** Internal temporary variable to use when connecting to a SpatialObject */
  SpatialObject::Pointer                  m_SpatialObjectToAdd;

  /** Time stamp for the time at which the next rendering will take place */
  TimeStamp                               m_TimeToRender;

  /** Transform returned from the Spatial Object */
  Transform                               m_SpatialObjectTransform;
    
  /** Auxiliary State Machine that manages the states of Visibility and
   * Invisibility of the objects. This State Machine is driven by the inputs of
   * the time stamp validity check. */
  StateMachineType                        m_VisibilityStateMachine;

  /** Coordinate system with respect to which Transforms will be computed.
   *  This is typically the coordinate system of a View class, since the
   *  View is the one requesting update representations to this class. */
  CoordinateSystem::ConstPointer m_TargetCoordinateSystem;

  /** Used to store an actor for visibility state machine processing. */
  vtkProp *                               m_VisibilitySetActor;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( NullSpatialObject );
  igstkDeclareInputMacro( ValidSpatialObject );
  igstkDeclareInputMacro( UpdateRepresentation );
  igstkDeclareInputMacro( SpatialObjectTransform );
  igstkDeclareInputMacro( TransformNotAvailable );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullSpatialObject );
  igstkDeclareStateMacro( ValidSpatialObject );
  igstkDeclareStateMacro( AttemptingGetTransform );

  /** Transduction macros that will convert received events 
   *  into StateMachine inputs.
   *  These events are defined in the file
   *  igstkCoordinateSystemInterfaceMacros.h
   *  Most of them map to the input "TransformNotAvailable",
   *  while only the event "CoordinateSystemTransformTo"
   *  maps to the input "SpatialObjectTransform".
   *
   */
  igstkEventTransductionMacro( 
    TransformNotAvailable,
    TransformNotAvailable); 

  // The only event that brings a valid transform.
  igstkLoadedEventTransductionMacro(  
     CoordinateSystemTransformTo,
     SpatialObjectTransform );

  /** Inputs to the Visibility State Machine */
  igstkDeclareInputMacro( ValidTimeStamp );
  igstkDeclareInputMacro( InvalidTimeStamp );
  igstkDeclareInputMacro( SetActorVisibility );

  /** States for the Visibility State Machine */
  igstkDeclareStateMacro( Visible );
  igstkDeclareStateMacro( Invisible );

};

} // end namespace igstk

#endif // __igstkObjectRepresentation_h
