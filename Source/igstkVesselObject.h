/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVesselObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVesselObject_h
#define __igstkVesselObject_h

#include "igstkSpatialObject.h"
#include <itkVesselTubeSpatialObject.h>

namespace igstk
{

class VascularNetworkReader;

/** \class VesselObject
 * 
 * \brief This class represents a Vessel object.
 * The Vessel is basically defined by a set of points 
 * representing its centerline.
 * Each point has a position and an associated radius value.
 *
 * \ingroup Object
 */

class VesselObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VesselObject, SpatialObject )

public:

  /** Internal typedef */
  typedef itk::VesselTubeSpatialObject<3>         VesselSpatialObjectType;
  typedef VesselSpatialObjectType::SpacingType    SpacingType;
  typedef VesselSpatialObjectType::TubePointType  PointType;
  typedef VesselSpatialObjectType::PointListType  PointListType;

  /** Add a point to the Vessel */
  void AddPoint(PointType & point);

  /** Return the number of points in the Vessel */
  unsigned int GetNumberOfPoints(void) const;

  /** Return a given point */
  const PointType * GetPoint(unsigned int pointId) const;

  /** Return the internal list of points */
  const PointListType GetPoints() const;

  /** Return the spacing of the vessel */
  const double* GetSpacing() const;

  /** Remove all the points in the list */
  void Clear( void );

  /** The TubeReaderToTubeSpatialObject class is declared as a friend in
   * order to be able to set the input mesh */
  igstkFriendClassMacro( igstk::VascularNetworkReader );


protected:

  /** Constructor */
  VesselObject();

  /** Destructor */
  ~VesselObject();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** These two methods must be declared and note be implemented
  *  in order to enforce the protocol of smart pointers. */
  VesselObject(const Self&);          //purposely not implemented
  void operator=(const Self&);        //purposely not implemented
 
  /** Set method to be invoked only by friends of this class */
  void SetVesselSpatialObject( VesselSpatialObjectType * vessel );

  /** Internal itkSpatialObject */
  VesselSpatialObjectType::Pointer   m_VesselSpatialObject;
};

} // end namespace igstk

#endif // __igstkVesselObject_h
