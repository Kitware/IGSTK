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


namespace igstk
{

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

  /** Create the vtkActors */
  virtual void CreateActors()= 0;

  /** Empty the list of actors */
  void DeleteActors();

  /** Get the VTK actors */
  GetMacro( Actors, ActorsListType );

  /** Add an actor to the list */
  void AddActor( vtkProp3D * );

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

  /** Set the spatial object for this class */
  void SetSpatialObject( const SpatialObjectType *);

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

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

  /** Update the position of the visual representation by using the information
   * from the Spatial Object. */
  void UpdatePositionFromGeometry();  
  void UpdateOrientationFromGeometry();  

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationFromGeometry();

 
};

} // end namespace igstk

#endif // __igstkObject_h

