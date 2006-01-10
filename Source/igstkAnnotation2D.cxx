/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAnnotation2D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAnnotation2D.h" 
#include "vtkActor2D.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"


namespace igstk 
{ 

/** Constructor */
Annotation2D::Annotation2D():m_StateMachine(this),m_Logger(NULL)
{
  for (int i = 0; i < 4; i++)
  {
    this->m_AnnotationText[i] = "";
    this->m_AnnotationMapper[i] = vtkTextMapper::New();
    this->m_AnnotationActor[i]  = vtkActor2D::New();
    this->m_AnnotationActor[i]->SetMapper(this->m_AnnotationMapper[i]);
    this->AddActors( m_AnnotationActor[i] );
  }
} 

/** Destructor */
Annotation2D::~Annotation2D()  
{ 
  // Delete the actors
  this->DeleteActors();

  for (int i = 0; i < 4; i++)
  {
    this->m_AnnotationMapper[i]->Delete();
  }
}

/** Add actor */
void Annotation2D::AddActors( vtkActor2D * actor )
{
  m_Actors.push_back( actor );
}


/** Add annotation */
void Annotation2D::AddAnnotationText( int i, const std::string text )
{
  if ( i < 0 && i > 3 ) 
  {
    return;
  }
  
  m_AnnotationText[i] = text;
  m_AnnotationMapper[i]->SetInput( m_AnnotationText[i].c_str() );
  this->m_AnnotationActor[i]->SetMapper(this->m_AnnotationMapper[i]);
}

void Annotation2D::AddAnnotations( int  * vSize )
{
  this->m_AnnotationActor[0]->SetPosition(10,10);
  this->m_AnnotationActor[1]->SetPosition(vSize[0] -60 ,10);
  this->m_AnnotationActor[2]->SetPosition(10, vSize[1] - 10);
  this->m_AnnotationActor[3]->SetPosition(vSize[0] - 60, vSize[1] - 10);
}

/** Empty the list of actors */
void Annotation2D::DeleteActors()
{
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->Delete();
    it++;
    }

  // Reset the list of actors
  m_Actors.clear();
}

/** Print Self function */
void Annotation2D::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

