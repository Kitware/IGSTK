/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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

class Scene;   // FIXME this should go
class View;

/** \class ObjectRepresentation
 * 
 * \brief Base class for all the igstk representation objects.
 *
 * This class serves as the base class for all the representation objects that
 * will provide a VTK visualization of the Spatial Objects that are composing a
 * give scene.
 *
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

  igstkTypeMacro(ObjectRepresentation, itk::Object);

  igstkFriendClassMacro( View );

  /** Set the color */
  void SetColor(float r, float g, float b);

  /** Get each color component */
  float GetRed() const   {return m_Color[0];}
  float GetGreen() const {return m_Color[1];}
  float GetBlue() const  {return m_Color[2];}

  /** Set/Get the opacity */
  igstkSetMacro(Opacity,float);
  igstkGetMacro(Opacity,float);

  /** Has the object been modified */
  bool IsModified() const;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected:

  ObjectRepresentation( void );
  ~ObjectRepresentation( void );

  /** Add an actor to the list */
  void AddActor( vtkProp3D * );

  /** Create the vtkActors */
  virtual void CreateActors()= 0;

  /** Get the VTK actors */
  igstkGetMacro( Actors, ActorsListType );

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

  /** update the visual representation with changes in the geometry */
  virtual void RequestUpdateRepresentation( const TimeStamp & time );

  /** update the visual representation with changes in the geometry. Only to be
   * called by the State Machine. This is an abstract method that MUST be
   * overloaded in every derived class. */
  virtual void UpdateRepresentation() = 0;

  /** Set the spatial object for this class */
  void SetSpatialObject(); 

private:

  /** Inputs to the State Machine */
  InputType            m_ValidSpatialObjectInput;
  InputType            m_NullSpatialObjectInput;
  InputType            m_UpdatePositionInput;
  InputType            m_UpdateRepresentationInput;
  
  /** States for the State Machine */
  StateType            m_NullSpatialObjectState;
  StateType            m_ValidSpatialObjectState;

  SpatialObjectType::ConstPointer m_SpatialObjectToAdd;

  /** Time stamp for the time at which the next rendering will take place */
  TimeStamp            m_TimeToRender;

};

} // end namespace igstk

#endif // __igstkObjectRepresentation_h

