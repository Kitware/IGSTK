/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkObject_h
#define __igstkObject_h


#include "itkObject.h"
#include "itkLogger.h"

#include "igstkLogger.h"
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
  
  typedef igstk::Logger                    LoggerType;

  /** Connect the Logger for this class */
  void SetLogger( LoggerType * logger );

  void RemoveObserver(unsigned long tag) const;

protected: 

  LoggerType * GetLogger() const;

  
protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  Object(void);

  virtual ~Object(void);

  /** Print the object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Register observed objects in an internal array so that they 
   *  can be disconnected upon destruction */
  void RegisterObservedObject( const ::igstk::Object * object, unsigned long tag );

  /** Remove observers that this object may have connected to other objects */
  void RemoveFromObservedObjects();

private: 
  
  typedef std::vector< unsigned long >           ObserverTagContainer;
  typedef std::vector< const ::igstk::Object * > ObservedObjectsContainer;

  mutable LoggerType::Pointer   m_Logger; 
  ObservedObjectsContainer      m_ObservedObjectsContainer;
  ObserverTagContainer          m_ObserverTagsContainer;

};

} // end of namespace igstk

#endif //__igstk_Object_h_
