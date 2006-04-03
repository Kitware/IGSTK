/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkObject_h
#define __igstkObject_h


#include "itkObject.h"
#include "itkLogger.h"

#include "igstkMacros.h"


namespace igstk
{
/** \class Object
 *
 *  \brief Base class for all the IGSTK classes
 *
 *  This class derives from the ITK Object and it is used for 
 *  introducion additional elemets that are common to IGSTK
 *  Objects.
 *
 */

class Object  : public ::itk::Object
{
 
public: 
  
  /** General Typedefs. Note that the igstkStandardClassTraitsMacro() cannot be
   * used here because that macro invokes calls in the superclass, for example
   * SetLogger, that will not be available in the ITK Object class. */
  typedef Object                            Self;  
  typedef ::itk::Object                     Superclass; 
  typedef ::itk::SmartPointer< Self >       Pointer; 
  typedef ::itk::SmartPointer< const Self > ConstPointer; 
  
  igstkTypeMacro( Object, ::itk::Object );  
  igstkNewMacro( Self );  
  
  typedef ::itk::Logger                  LoggerType; 

  /** Connect the Logger for this class */
  void SetLogger( LoggerType * logger );


protected: 

  LoggerType * GetLogger() const;

  
protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  Object(void);

  virtual ~Object(void);

  /** Print the object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private: 
  
  mutable LoggerType::Pointer   m_Logger; 


};

} // end of namespace igstk

#endif //__igstk_Object_h_
