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
#include "itkLogger.h"
#include "vtkProp.h"
#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkStateMachine.h"
#include "igstkCoordinateReferenceSystem.h"

namespace igstk
{

/** \class ObjectRepresentation
 * 
 * \brief Base class for all the igstk representation objects.
 *
 * This class serves as the base class for all the representation objects that
 * will provide a VTK visualization of the Spatial Objects that are composing a
 * given scene.
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

  typedef double                             ScalarType;
  typedef SpatialObject                      SpatialObjectType;
  typedef std::vector< vtkProp* >            ActorsListType; 

  /** Set the color */
  void SetColor(float r, float g, float b);

  /** Get each color component */
  float GetRed() const   {return m_Color[0];}
  float GetGreen() const {return m_Color[1];}
  float GetBlue() const  {return m_Color[2];}

  /** Set/Get the opacity */
  virtual void SetOpacity(float alpha);
  igstkGetMacro( Opacity, float );

  /** Create the vtkActors */
  virtual void CreateActors()= 0;

  /** Get the VTK actors */
  igstkGetMacro( Actors, ActorsListType );

  /** update the visual representation with changes in the geometry */
  virtual void RequestUpdateRepresentation( const TimeStamp & time, 
                                            const CoordinateReferenceSystem* cs );


  /** DEPRECATED */
  virtual void RequestUpdateRepresentation( const TimeStamp & time );
  /** DEPRECATED */

  /** DEPRECATED !!! DELETE THIS METHOD */
  void RequestUpdatePosition( const TimeStamp & time ) {};
  /** DEPRECATED !!! DELETE THIS METHOD */

  /** DEPRECATED !!! DELETE THIS METHOD */
  void RequestVerifyTimeStamp() {};
  /** DEPRECATED !!! DELETE THIS METHOD */

protected:

  ObjectRepresentation( void );
  ~ObjectRepresentation( void );

  ActorsListType              m_Actors;
  float                       m_Opacity;

  /** Add an actor to the list */
  void AddActor( vtkProp * );

  /** Empty the list of actors */
  virtual void DeleteActors();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Request the state machine to set a Spatial Object */
  void RequestSetSpatialObject( const SpatialObjectType * spatialObject );
  
  
private:

  float                       m_Color[3];

  /** The associated SpatialObject is non-const because we invoke requests
   * methods that indirectly will modify the state of its internal StateMachine
   * */
  SpatialObjectType::Pointer  m_SpatialObject;

  /** update the visual representation with changes in the geometry. Only to be
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

  /** Receive TransformNotAvailable message from the SpatialObject via a transduction macro.
   *  Since no new transform is available, it checks whether the validity time
   *  of the previously existing transform has not expired and updates the
   *  visibility of the object accordingly. */
  void ReceiveTransformNotAvailableProcessing();

  /** This action calls the RequestGetTransform() method in the spatial
   *  object, and the answer is expected to be processed by the observer
   *  created in the transduction macro */
  void RequestGetTransformProcessing();

private:

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
   *  into StateMachine inputs */
  igstkEventTransductionMacro( TransformNotAvailable, 
                                     TransformNotAvailable); 

  igstkLoadedEventTransductionMacro( 
                            CoordinateReferenceSystemTransformTo
                                                , SpatialObjectTransform );

  /** Internal temporary variable to use when connecting to a SpatialObject */
  SpatialObjectType::Pointer    m_SpatialObjectToAdd;

  /** Time stamp for the time at which the next rendering will take place */
  TimeStamp                     m_TimeToRender;

  /** Transform returned from the Spatial Object */
  Transform                     m_SpatialObjectTransform;
    
  /** Auxiliary State Machine that manages the states of Visibility and
   * Invisibility of the objects. This State Machine is driven by the inputs of
   * the time stamp validity check. */
  StateMachineType     m_VisibilityStateMachine;

  /** Inputs to the Visibility State Machine */
  igstkDeclareInputMacro( ValidTimeStamp );
  igstkDeclareInputMacro( InvalidTimeStamp );
  
  /** States for the Visibility State Machine */
  igstkDeclareStateMacro( Visible );
  igstkDeclareStateMacro( Invisible );

  CoordinateReferenceSystem::ConstPointer m_TargetCoordinateSystem;
};

} // end namespace igstk

#endif // __igstkObjectRepresentation_h
