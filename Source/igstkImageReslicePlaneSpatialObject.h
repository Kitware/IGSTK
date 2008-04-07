/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReslicePlaneSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImageReslicePlaneSpatialObject_h
#define __igstkImageReslicePlaneSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkImageSpatialObject.h"

namespace igstk
{

/** \class ImageReslicePlaneSpatialObject
 * 
 * \brief This class computes image reslicing plane parameters.  
 *
 * The computed plane is used by igstkImageResliceSpatialObjectRepresentation
 * class to generate a resliced image representation.
 *
 * The class provides three modes of reslicing i.e Orthogonal, OffOrthogonal and Oblique. 
 *
 * In orthogonal mode, three types of orientation are defined: Axial, Coronal and Sagital. 
 * For this mode, the tool tip spatial object provides the reslicing plane postion and orientation
 * information is extraced from the input image spatial object.
 *
 * For OffOrthogonal mode, the three types of orientation are Perpendicular, OffAxial and OffSagittal.
 * The orientation of the reslicing plane is computed using the tracker tool spatial object and
 * the input image spatial object.  The postion is determined from the tracker tool tip postion.
 *
 * For oblique mode, all the orientation and postion information of the reslicing plane are obtained
 * from the tracker tool.
 *
 *
 * \ingroup SpatialObject
 */

class ImageReslicePlaneSpatialObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ImageReslicePlaneSpatialObject, SpatialObject )

public:

  /** Typedefs */
  /** Reslicing modes */
  typedef enum
    { 
    Orthogonal, 
    OffOrthogonal, 
    Oblique 
    } 
  ReslicingMode;

  /** Orthogonal orientation types */
  typedef enum
    { 
    Axial, 
    Sagittal, 
    Coronal, 
    Perpendicular,
    OffAxial,
    OffSagittal
    } 
  OrientationType;

  /** Request the state machine to attempt to set the reslicing mode*/
  void RequestSetReslicingMode( ReslicingMode reslicingMode ); 

  /** Request the state machine to attempt to set orientation type */
  void RequestSetOrientationType( OrientationType orientationType ); 

protected:

  ImageReslicePlaneSpatialObject( void );
  ~ImageReslicePlaneSpatialObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidReslicingMode );
  igstkDeclareInputMacro( InValidReslicingMode );
  igstkDeclareInputMacro( ValidOrientationType );
  igstkDeclareInputMacro( InValidOrientationType );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( ReslicingModeSet );
  igstkDeclareStateMacro( OrientationTypeSet );

  /** Internal itkSpatialObject */

  /** Set the orientation type */
  void SetOrientationTypeProcessing( void );

  /** Set the reslcing mode */
  void SetReslicingModeProcessing( void );

  /** Report invalid reslicing mode */
  void ReportInvalidReslicingModeProcessing( void );

  /** Report invalid orientation type */
  void ReportInvalidOrientationTypeProcessing( void );

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Variables for managing reslicing mode */
  ReslicingMode     m_ReslicingModeToBeSet;
  ReslicingMode     m_ReslicingMode;

  /** Variables for managing orientation type */
  OrientationType     m_OrientationTypeToBeSet;
  OrientationType     m_OrientationType;
};

} // end namespace igstk

#endif // __igstkImageReslicePlaneSpatialObject_h
