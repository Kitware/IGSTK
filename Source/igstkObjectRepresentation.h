/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

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
#include "igstkMacros.h"
#include "itkObject.h"
#include "igstkSpatialObject.h"
#include "vtkProp3D.h"
#include "itkCommand.h"
#include "igstkStateMachine.h"


namespace igstk
{

class Scene;

/** \class Object
 * 
 * \brief Base class for all the igstk objects
 * \ingroup Object
 */
class ObjectRepresentation 
  : public itk::Object
{

public:

  typedef ObjectRepresentation               Self;
  typedef double                             ScalarType;
  typedef itk::SmartPointer < Self >         Pointer;
  typedef itk::SmartPointer < const Self >   ConstPointer;
  typedef itk::Object                        Superclass;
  typedef SpatialObject                      SpatialObjectType;
  typedef std::vector<vtkProp3D*>            ActorsListType; 

  itkTypeMacro(ObjectRepresentation, itk::Object);

  typedef StateMachine< Self >                       StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer   ActionType;
  typedef StateMachineType::StateType                StateType;
  typedef StateMachineType::InputType                InputType;

  FriendClassMacro( StateMachineType );
  FriendClassMacro( Scene );

  /** Set the color */
  void SetColor(float r, float g, float b);

  /** Get each color component */
  float GetRed() const   {return m_Color[0];}
  float GetGreen() const {return m_Color[1];}
  float GetBlue() const  {return m_Color[2];}

  /** Set/Get the opacity */
  SetMacro(Opacity,float);
  GetMacro(Opacity,float);

  /** Has the object been modified */
  bool IsModified() const;

protected:

  ObjectRepresentation( void );
  ~ObjectRepresentation( void );

  /** Add an actor to the list */
  void AddActor( vtkProp3D * );

  /** Create the vtkActors */
  virtual void CreateActors()= 0;

  /** Get the VTK actors */
  GetMacro( Actors, ActorsListType );

  /** Empty the list of actors */
  void DeleteActors();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Request the state machine to set a Spatial Object */
  void RequestSetSpatialObject( const SpatialObjectType * spatialObject );
  
private:

  ActorsListType              m_Actors;
  float                       m_Color[3];
  float                       m_Opacity;
  unsigned long               m_LastMTime;

  SpatialObjectType::ConstPointer  m_SpatialObject;

  typedef itk::SimpleMemberCommand< Self >   ObserverType;

  ObserverType::Pointer       m_PositionObserver;
  ObserverType::Pointer       m_OrientationObserver;
  ObserverType::Pointer       m_GeometryObserver;

  /** Request updating the position of the visual representation by using the
   * information from the Spatial Object. */
  void RequestUpdatePosition();  
  void RequestUpdateOrientation();  

  /** update the visual representation with changes in the geometry */
  virtual void RequestUpdateRepresentation();


  /** Update the position of the visual representation by using the information
   * from the Spatial Object. Only to be called by the State Machine. */
  void UpdatePositionFromGeometry();  
  void UpdateOrientationFromGeometry();  

  /** update the visual representation with changes in the geometry. Only to be
   * called by the State Machine. */
  virtual void UpdateRepresentationFromGeometry();

  /** Set the spatial object for this class */
  void SetSpatialObject(); 

private:

  StateMachineType     m_StateMachine;
  
  /** Inputs to the State Machine */
  InputType            m_ValidSpatialObjectInput;
  InputType            m_NullSpatialObjectInput;
  InputType            m_UpdateOrientationInput;
  InputType            m_UpdatePositionInput;
  InputType            m_UpdateRepresentationInput;
  
  /** States for the State Machine */
  StateType            m_NullSpatialObjectState;
  StateType            m_ValidSpatialObjectState;

  SpatialObjectType::ConstPointer m_SpatialObjectToAdd;
};

} // end namespace igstk

#endif // __igstkObjectRepresentation_h

