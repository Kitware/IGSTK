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
#include <igstkMacros.h>
#include <itkSpatialObject.h>
#include <igstkObjectProperty.h>
#include <itkObject.h>
#include <vtkProp3D.h>


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
  typedef itk::SpatialObject<3>              SpatialObjectType;
  typedef std::vector<vtkProp3D*>            ActorsListType;
  typedef std::vector<SpatialObjectType* >   SpatialObjectsListType;
 

  itkTypeMacro(ObjectRepresentation, itk::Object);

  /** Create the vtkActors */
  virtual void CreateVTKActors()= 0;

  /** Get the VTK actors */
  ActorsListType & GetVTKActors() {return m_ActorList;}

  /** Get the object property */
  ObjectProperty* GetProperty() {return & m_Property;}

  /** Return the SpatialObjects */
  SpatialObjectsListType & GetSpatialObjects() {return m_SpatialObjectList;}

  /** Return the number of children */
  unsigned int GetNumberOfChildren( unsigned int depth, char * name );

protected:

  ObjectRepresentation( void );
  ~ObjectRepresentation( void );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  ActorsListType           m_ActorList;
  SpatialObjectsListType   m_SpatialObjectList;
  ObjectProperty           m_Property;

};

} // end namespace igstk

#endif // __igstkObject_h
