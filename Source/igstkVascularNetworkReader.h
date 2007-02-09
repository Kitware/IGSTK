/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVascularNetworkReader_h
#define __igstkVascularNetworkReader_h

#include "igstkMacros.h"
#include "igstkSpatialObjectReader.h"

#include "igstkVascularNetworkObject.h"
#include "itkVesselTubeSpatialObject.h"
#include "igstkVesselObject.h"

namespace igstk
{

/** \class VascularNetworkReader
 * 
 * \brief This class reads 3D VascularNetwork in the metaIO format.
 *
 * Tubular structures are quite common in human anatomy. This class is 
 * intended to read groups of tubular structures from files in metaIO 
 * format. Typically these structures are the result of a segmentation 
 * method applied on pre-operative images.
 * 
 * \sa MeshReader
 *
 * \ingroup Readers
 */
class VascularNetworkReader : public SpatialObjectReader<3>
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VascularNetworkReader, 
                                 SpatialObjectReader<3> )

public:

  /** Typedefs */
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef itk::VesselTubeSpatialObject<3>    VesselTubeSpatialObjectType;
  typedef igstk::VascularNetworkObject       VascularNetworkType;
  typedef igstk::VesselObject                VesselObjectType;

  /** Return the output vascular Network as an event */
  void RequestGetVascularNetwork();

  /** Event type */
  igstkLoadedObjectEventMacro( VascularNetworkModifiedEvent, IGSTKEvent, 
                               VascularNetworkObject);

protected:

  /** Constructor and Destructor */
  VascularNetworkReader();
  ~VascularNetworkReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Null operation for State Machine transition */
  void NoProcessing();

  /** This method request Object read. This method is intended to be
   *  invoked ONLY by the State Machine of the superclass. **/
  void AttemptReadObjectProcessing();
  
  /** This function reports the vascular network */
  void ReportVascularNetworkProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVascularNetworkObject );
  igstkDeclareInputMacro( NullVascularNetworkObject );

  igstkDeclareInputMacro( GetVascularNetwork );

  /** States for the State Machine */
  igstkDeclareStateMacro( ValidVascularNetworkObject );
  igstkDeclareStateMacro( NullVascularNetworkObject );

  VascularNetworkReader(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  VascularNetworkType::Pointer m_VascularNetwork;

};

} // end namespace igstk


#endif // __igstkVascularNetworkReader_h
