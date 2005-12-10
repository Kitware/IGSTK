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

#include "itkSpatialObjectReader.h"
#include "itkObject.h"
#include "itkEventObject.h"

#include "igstkTubeObject.h"
#include "itkTubeSpatialObject.h"
#include "igstkTubeGroupObject.h"

namespace igstk
{

/** \class TubeReader
 * 
 * \brief This class reads 3D Tube in the metaIO format.
 *
 * Tubular structures are quite common in human anatomy. This class is intended
 * to read groups of tubular structrures from files in metaIO format. Typical
 * these structures are the result of a segmentation method applied on
 * pre-operative images.
 * 
 * \sa MeshReader
 *
 * \ingroup Readers
 */
class TubeReader : public SpatialObjectReader<3>
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TubeReader, SpatialObjectReader<3> );

public:

  /** Typedefs */
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef igstk::TubeObject                  TubeType;
  typedef itk::TubeSpatialObject<3>          TubeSpatialObjectType;
  typedef igstk::TubeGroupObject             GroupObjectType;


  /** Return the output as a group */
  const GroupObjectType * GetOutput() const;

protected:

  /** Constructor and Destructor */
  TubeReader();
  ~TubeReader();

  // Generic event produced from this class
  itkEventMacro( TubeReaderEvent,IGSTKEvent);
  
  //SpatialObject reading error
  itkEventMacro( TubeReadingErrorEvent, TubeReaderEvent );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** This method request Object read. This method is intended to be
   *  invoked ONLY by the State Machine of the superclass. **/
  void AttemptReadObject();

private:

  TubeReader(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  GroupObjectType::Pointer m_Group;

};

} // end namespace igstk


#endif // __igstkTubeReader_h
