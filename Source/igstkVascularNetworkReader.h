/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVascularNetworkReader_h
#define __igstkVascularNetworkReader_h

#include "igstkMacros.h"
#include "igstkSpatialObjectReader.h"

#include "itkSpatialObjectReader.h"
#include "itkObject.h"
#include "itkEventObject.h"

#include "igstkVascularNetworkObject.h"
#include "itkVesselTubeSpatialObject.h"
#include "igstkVesselObject.h"

namespace igstk
{

/** \class VascularNetworkReader
 * 
 * \brief This class reads 3D VascularNetwork in the metaIO format.
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
class VascularNetworkReader : public SpatialObjectReader<3>
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VascularNetworkReader, SpatialObjectReader<3> )

public:

  /** Typedefs */
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef itk::VesselTubeSpatialObject<3>    VesselTubeSpatialObjectType;
  typedef igstk::VascularNetworkObject       VascularNetworkType;
  typedef igstk::VesselObject                VesselObjectType;

  /** Return the output as a group */
  const VascularNetworkType * GetOutput() const;

protected:

  /** Constructor and Destructor */
  VascularNetworkReader();
  ~VascularNetworkReader();

  // Generic event produced from this class
  itkEventMacro( VascularNetworkReaderEvent,IGSTKEvent);
  
  //SpatialObject reading error
  itkEventMacro( VascularNetworkReadingErrorEvent, VascularNetworkReaderEvent );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** This method request Object read. This method is intended to be
   *  invoked ONLY by the State Machine of the superclass. **/
  void AttemptReadObjectProcessing();

private:

  VascularNetworkReader(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  VascularNetworkType::Pointer m_VascularNetwork;

};

} // end namespace igstk


#endif // __igstkVascularNetworkReader_h
