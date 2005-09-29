/*=========================================================================

  Program:   SpatialObject Guided Surgery Software Toolkit
  Module:    igstkTubeReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTubeReader_h
#define __igstkTubeReader_h

#include "igstkMacros.h"
#include "igstkSpatialObjectReader.h"

#include "igstkStateMachine.h"
#include "itkLogger.h"

#include "itkSpatialObjectReader.h"
#include "itkObject.h"
#include "itkEventObject.h"

#include "igstkEvents.h"
#include "igstkStringEvents.h"
#include "igstkTubeObject.h"
#include "itkTubeSpatialObject.h"
#include "igstkTubeGroupObject.h"

namespace igstk
{

/** \class TubeReader
 * 
 * \brief This class reads 3D Tube in the metaIO format
 * 
 * \ingroup Readers
 */
class TubeReader : public SpatialObjectReader<3>
{

public:

  /** Typedefs */
  typedef TubeReader                         Self;
  typedef SpatialObjectReader<3>             Superclass;
  typedef itk::SmartPointer< Self >          Pointer;
  typedef itk::SmartPointer< const Self >    ConstPointer;
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef itk::Logger                        LoggerType;
  typedef igstk::TubeObject                  TubeType;
  typedef itk::TubeSpatialObject<3>          TubeSpatialObjectType;
  typedef igstk::TubeGroupObject             GroupObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( TubeReader, SpatialObjectReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );
  
  /** This method request Object read **/
  void AttemptReadObject();

  /** Return the output as a group */
  const GroupObjectType * GetOutput() const;

protected:

  TubeReader( void );
  ~TubeReader( void );

  itkEventMacro( TubeReaderEvent,IGSTKEvent);
  
  //SpatialObject reading error
  itkEventMacro( TubeReadingErrorEvent, TubeReaderEvent );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  GroupObjectType::Pointer m_Group;

};

} // end namespace igstk


#endif // __igstkTubeReader_h
