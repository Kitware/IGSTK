/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    ModelBasedRegistration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ModelBasedRegistration_h
#define __ModelBasedRegistration_h

#include "DeckOfCardRobotRegistrationBase.h"
#include "itkCommand.h"

// Define an itk command subclass to observe the optimizer progress
template < class TOptimizer >
class IterationCallback : public itk::Command 
{
public:
  typedef IterationCallback   Self;
  typedef itk::Command  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  
  itkTypeMacro( IterationCallback, Superclass );
  itkNewMacro( Self );

  /** Type defining the optimizer. */
  typedef    TOptimizer     OptimizerType;

  /** Method to specify the optimizer. */
  void SetOptimizer( OptimizerType * optimizer )
    { 
      m_Optimizer = optimizer;
      m_Optimizer->AddObserver( itk::IterationEvent(), this );
    }

  /** Execute method will print data at each iteration */
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object *, const itk::EventObject & event)
    {
      if( typeid( event ) == typeid( itk::StartEvent ) )
        {
        std::cout << std::endl << "Position              Value";
        std::cout << std::endl << std::endl;
        }    
      else if( typeid( event ) == typeid( itk::IterationEvent ) )
        {
        std::cout << m_Optimizer->GetCurrentIteration() << "   ";
        std::cout << m_Optimizer->GetValue() << "   ";
        std::cout << m_Optimizer->GetCurrentPosition() << std::endl;
        }
      else if( typeid( event ) == typeid( itk::EndEvent ) )
        {
        std::cout << std::endl << std::endl;
        std::cout << "After " << m_Optimizer->GetCurrentIteration();
        std::cout << "  iterations " << std::endl;
        std::cout << "Solution is    = " << m_Optimizer->GetCurrentPosition();
        std::cout << std::endl;
        }
    }

protected:
  IterationCallback() {};
  itk::WeakPointer<OptimizerType>   m_Optimizer;
};

class ModelBasedRegistration: 
  public DeckOfCardRobotRegistrationBase
{

public:

  typedef ModelBasedRegistration                  Self;
  typedef DeckOfCardRobotRegistrationBase         Superclass;
  typedef itk::SmartPointer< Self >               Pointer;
  typedef itk::SmartPointer< const Self >         ConstPointer;

  itkNewMacro( Self );
   
  virtual bool Execute();

protected:
  ModelBasedRegistration();
  virtual ~ModelBasedRegistration(){};
  void PrintSelf( std::ostream& os, itk::Indent indent );

private:

};

#endif
