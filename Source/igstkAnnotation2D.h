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
// Disabling warning C4355: 'this' : used in base member initializer list
#pragma warning ( disable : 4355 )
#endif

#include <string>
#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "vtkViewport.h"


namespace igstk
{

class View;

/** \class Annotation2D
 * 
 * \brief Add 2D annotations to the viewport.
 *
 * This class is used to add 2D annotations to the viewport. 
 * Annotations can be added to any of the four corners of the viewport.
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
  
  
  /* Macro with standard traits declarations. */  
  igstkStandardClassTraitsMacro( Annotation2D, Object )

  /* Add annotation text */
  void RequestAddAnnotationText( int , const std::string & );

  /** Request to Set viewport*/
  void RequestSetAnnotationsViewPort( int horizontal, int vertical );

  /** Request add annotations */
  void RequestAddAnnotations();

  /** Declarations needed for the Logging */
  igstkLoggerMacro();
  
  /** Type defining the container of actors */
  typedef std::vector< vtkActor2D* >         ActorsListType; 

  /** Get the VTK actors */
  igstkGetMacro( Actors, ActorsListType );

protected:

  Annotation2D( void );
  ~Annotation2D( void );


  /** Add an actor */
  void AddActors( vtkActor2D* );

  /** Delete Actors */
  void DeleteActors( );

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  
private:

  Annotation2D( const Self & ); //purposely not implemented
  void operator=(const Self&);  //purposely not implemented
  
  ActorsListType                   m_Actors;

  std::string                      m_AnnotationText[4];
  std::string                      m_AnnotationTextToBeAdded;
  vtkTextMapper *                  m_AnnotationMapper[4]; 
  vtkActor2D    *                  m_AnnotationActor[4];
  vtkActor2D    *                  m_ActorToBeAdded;

  int                              m_ViewPortHorizontalSize;
  int                              m_ViewPortVerticalSize;
  int                              m_ViewPortHorizontalSizeToBeSet;
  int                              m_ViewPortVerticalSizeToBeSet;
 
  /** Private functions that only be invoked through the state machine */
  void AddActorProcessing();
  void AddAnnotationTextProcessing();
  void SetViewPortProcessing();
  void AddAnnotationsProcessing();
  void ReportInvalidAnnotationIndexProcessing();
  void ReportInvalidRequestProcessing();
  
  /** Annotation index */
  int                              m_IndexForAnnotationToBeAdded;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidAnnotations );
  igstkDeclareInputMacro( InvalidAnnotations );
  igstkDeclareInputMacro( ValidViewPort );
  igstkDeclareInputMacro( ValidAnnotationIndex );
  igstkDeclareInputMacro( InvalidAnnotationIndex );

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ViewPortSet );
  igstkDeclareStateMacro( AnnotationsAdded );
};

} // end namespace igstk

#endif // __igstkAnnotation2D_h
