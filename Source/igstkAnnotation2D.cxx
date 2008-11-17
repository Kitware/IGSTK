/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAnnotation2D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAnnotation2D.h" 
#include "vtkTextActor.h"
#include "vtkTextProperty.h"


namespace igstk 
{ 

/** Constructor */
Annotation2D::Annotation2D():m_StateMachine(this),m_Logger(NULL)
{
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  for (int i = 0; i < 4; i++)
    {
    this->m_AnnotationText[i] = "";
    this->m_TextProperty[i] = vtkTextProperty::New();
    this->m_AnnotationActor[i]  = vtkTextActor::New();
    this->m_AnnotationActor[i]->SetTextProperty(this->m_TextProperty[i]);
    m_ActorToBeAdded = m_AnnotationActor[i];
    this->AddActorProcessing( );
    }
  
  igstkAddInputMacro( ValidAnnotationIndex );
  igstkAddInputMacro( InvalidAnnotationIndex );
  igstkAddInputMacro( ValidViewPort );
  igstkAddInputMacro( ValidAnnotations );
  igstkAddInputMacro( InvalidAnnotations );
  igstkAddInputMacro( ValidColorProperty );
  igstkAddInputMacro( InvalidColorProperty );
  igstkAddInputMacro( ValidFontSizeProperty );
  igstkAddInputMacro( InvalidFontSizeProperty );

  
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ViewPortSet );
  igstkAddStateMacro( AnnotationsAdded ); 

  igstkAddTransitionMacro ( Idle, ValidAnnotationIndex , 
                            Idle, SetAnnotationText );  

  igstkAddTransitionMacro ( Idle, InvalidAnnotationIndex , 
                            Idle, ReportInvalidAnnotationIndex );  

  igstkAddTransitionMacro ( Idle, ValidViewPort, ViewPortSet, SetViewPort );

  igstkAddTransitionMacro ( ViewPortSet, ValidAnnotations , 
                            AnnotationsAdded , AddAnnotations );  

  igstkAddTransitionMacro ( AnnotationsAdded, ValidAnnotationIndex , 
                            AnnotationsAdded, SetAnnotationText );  

  igstkAddTransitionMacro ( AnnotationsAdded, ValidColorProperty , 
                            AnnotationsAdded, ChangeTextColor );  

  igstkAddTransitionMacro ( Idle, ValidColorProperty , 
                            Idle, ChangeTextColor );  

  igstkAddTransitionMacro ( ViewPortSet, ValidColorProperty , 
                            ViewPortSet, ChangeTextColor );  

  igstkAddTransitionMacro ( AnnotationsAdded, ValidFontSizeProperty , 
                            AnnotationsAdded, ChangeFontSize );  

  igstkAddTransitionMacro ( Idle, ValidFontSizeProperty , 
                            Idle, ChangeFontSize );  

  igstkAddTransitionMacro ( ViewPortSet, ValidFontSizeProperty , 
                            ViewPortSet, ChangeFontSize );  


  igstkAddTransitionMacro ( AnnotationsAdded, ValidViewPort, AnnotationsAdded
                                             , SetViewPort );

  igstkAddTransitionMacro ( ViewPortSet, ValidViewPort , 
                            ViewPortSet , SetViewPort );  

  //Invalid requests
  igstkAddTransitionMacro ( Idle, ValidAnnotations, Idle, ReportInvalidRequest);
  
  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
Annotation2D::~Annotation2D()  
{ 
  igstkLogMacro( DEBUG, "Destructor() called ...\n");
  
  this->DeleteActors();

  for (int i = 0; i < 4; i++)
    {
    this->m_TextProperty[i]->Delete();
    }
}

/** Add actor */
void Annotation2D::AddActorProcessing( )
{
  igstkLogMacro( DEBUG, "AddActorProcessing called ...\n");
  m_Actors.push_back( m_ActorToBeAdded );
}

/** Add annotation text */
void Annotation2D::RequestSetAnnotationText( int i, const std::string  & text )
{
  igstkLogMacro( DEBUG, "RequestSetAnnotationText called ...\n");  

  m_IndexForAnnotationToBeAdded = i;
  m_AnnotationTextToBeAdded = text;
 
  if ( m_IndexForAnnotationToBeAdded < 0 || m_IndexForAnnotationToBeAdded > 3 ) 
    {
    igstkPushInputMacro( InvalidAnnotationIndex );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro ( ValidAnnotationIndex );
    m_StateMachine.ProcessInputs();
    }
}

/** Add annotation text processing */
void Annotation2D::SetAnnotationTextProcessing( )
{
  igstkLogMacro( DEBUG, "SetAnnotationTextProcessing called ...\n");

  m_AnnotationText[m_IndexForAnnotationToBeAdded] = m_AnnotationTextToBeAdded;
  this->m_AnnotationActor[m_IndexForAnnotationToBeAdded]->
    SetInput( m_AnnotationText[m_IndexForAnnotationToBeAdded].c_str() );
}

/** Request set annotation view port */
void Annotation2D::RequestSetAnnotationsViewPort( int horizontalSize, 
                                                  int verticalSize )
{
  igstkLogMacro( DEBUG, "RequestSetAnnotationsViewPort called ....\n"  );
 
  m_ViewPortHorizontalSizeToBeSet = horizontalSize;
  m_ViewPortVerticalSizeToBeSet   = verticalSize;
  igstkPushInputMacro( ValidViewPort );
  m_StateMachine.ProcessInputs();
}

/** Request font color */
void Annotation2D
::RequestSetFontColor( int index, double red, double green, double blue )
{
  igstkLogMacro( DEBUG, "RequestSetFontColor() called ....\n"  );

  m_AnnotationIndexFontColorToBeChanged = index;
  m_FontColor[0] = red;
  m_FontColor[1] = green;
  m_FontColor[2] = blue;

  // TODO Check for valid color property 
  igstkPushInputMacro ( ValidColorProperty );
  m_StateMachine.ProcessInputs();
}

/** Request set font size */
void Annotation2D
::RequestSetFontSize( int index, int size)
{
  igstkLogMacro( DEBUG, "RequestSetFontColor() called ....\n"  );

  m_AnnotationIndexFontSizeToBeChanged= index;
  m_FontSize = size;

  // TODO Check for valid font size property 
  igstkPushInputMacro ( ValidFontSizeProperty );
  m_StateMachine.ProcessInputs();
}
 
/** Change the font color */ 
void Annotation2D::ChangeTextColorProcessing( )
{
  igstkLogMacro( DEBUG, "ChangeTextColorProcessing() called ....\n"  );

  this->m_TextProperty[this->m_AnnotationIndexFontColorToBeChanged]->SetColor(
    m_FontColor[0], m_FontColor[1], m_FontColor[2]); 

  this->m_AnnotationActor[this->m_AnnotationIndexFontColorToBeChanged]->
    SetTextProperty( 
      this->m_TextProperty[m_AnnotationIndexFontColorToBeChanged]);
}
 
/** Change the font size */ 
void Annotation2D::ChangeFontSizeProcessing( )
{
  igstkLogMacro( DEBUG, "ChangeFontSizeProcessing() called ....\n"  );

  m_TextProperty[m_AnnotationIndexFontSizeToBeChanged]->SetFontSize(
                                  m_FontSize);

  this->m_AnnotationActor[m_AnnotationIndexFontSizeToBeChanged]
     ->SetTextProperty( m_TextProperty[m_AnnotationIndexFontSizeToBeChanged]);
}
 
 
/** Request Add annotations */
void Annotation2D::RequestAddAnnotations()
{
  igstkLogMacro( DEBUG, "RequestAddAnnotations called ....\n");
  igstkPushInputMacro( ValidAnnotations );
  m_StateMachine.ProcessInputs();
}

/** */
void Annotation2D::SetViewPortProcessing( )
{
  igstkLogMacro( DEBUG, "SetViewPortProcessing called ....\n"  );
  
  this->m_ViewPortHorizontalSize = m_ViewPortHorizontalSizeToBeSet;
  this->m_ViewPortVerticalSize = m_ViewPortVerticalSizeToBeSet;

  this->m_AnnotationActor[0]->SetPosition(10,10);
  this->m_AnnotationActor[1]->SetPosition( 
                                        this->m_ViewPortHorizontalSize-60,10);
  this->m_AnnotationActor[2]->SetPosition(10, 
                                          this->m_ViewPortVerticalSize - 10);
  this->m_AnnotationActor[3]->SetPosition(this->m_ViewPortHorizontalSize-60, 
                                          this->m_ViewPortVerticalSize - 10);
}


void Annotation2D::AddAnnotationsProcessing(  )
{
  igstkLogMacro( DEBUG, "AddAnnotationsProcessing called ...\n");

  this->m_AnnotationActor[0]->SetPosition(10,10);
  this->m_AnnotationActor[1]->SetPosition( 
                                        this->m_ViewPortHorizontalSize-60,10);
  this->m_AnnotationActor[2]->SetPosition(10, 
                                          this->m_ViewPortVerticalSize - 10);
  this->m_AnnotationActor[3]->SetPosition(this->m_ViewPortHorizontalSize-60, 
                                          this->m_ViewPortVerticalSize - 10);
}

/** Empty the list of actors */
void Annotation2D::DeleteActors()
{
  igstkLogMacro( DEBUG, "DeleteActors called....\n" );

  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->Delete();
    it++;
    }

  // Reset the list of actors
  m_Actors.clear();
}

/** */
void Annotation2D::ReportInvalidAnnotationIndexProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidAnnotationIndexProcessing....\n" );
}

/** */
void Annotation2D::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing....\n" );
}


/** Print Self tion */
void Annotation2D::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk
