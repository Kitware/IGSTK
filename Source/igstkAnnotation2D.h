/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAnnotation2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAnnotation2D_h
#define __igstkAnnotation2D_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <vector>
#include <string>
#include "itkCommand.h"
#include "itkLogger.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "vtkViewport.h"


namespace igstk
{

class View;

/** \class Annotation2D
 * 
 * \brief .
 *
 * This class is used to add 2D annotations to the viewport. Annotations can be added to 
 * any of the four corners of the viewport.
 *
 *  \image html  igstkAnnotation2D.png  "2D Annotation State Machine Diagram"
 *  \image latex igstkAnnotation2D.eps  "2D Annotation  Machine Diagram" 
 *
 * \ingroup Object
 */
class Annotation2D 
  : public Object
{

public:

  typedef Annotation2D               Self;
  typedef double                             ScalarType;
  typedef itk::SmartPointer < Self >         Pointer;
  typedef itk::SmartPointer < const Self >   ConstPointer;
  typedef Object                             Superclass;
  typedef std::vector< vtkActor2D* >         ActorsListType; 
   

  igstkTypeMacro(Annotation2D, Object);

  /** Has the object been modified */
    bool IsModified() const;
    
  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Declarations needed for the Logging */
  igstkLoggerMacro();
  
protected:

  Annotation2D( void );
  ~Annotation2D( void );

  /** Add an actor */
  void AddActors( vtkActor2D* );

  /** Add annotations */
  void AddAnnotations( vtkViewport* );
 
  /** Add annotation text */
  void AddAnnotationText( int , const std::string );
      
  /** Empty the list of actors */
  void DeleteActors();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  ActorsListType                   m_Actors;
  unsigned long                    m_LastMTime;
  int                              LastSize;

  std::string                      m_AnnotationText[4];
  vtkTextMapper *                  m_AnnotationMapper[4]; 
  vtkActor2D    *                  m_AnnotationActor[4];

  /** Inputs to the State Machine */
  
  /** States for the State Machine */
};

} // end namespace igstk

#endif // __igstkAnnotation2D_h

