/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTubeReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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

namespace igstk
{

namespace Friends 
{

/** \class TubeReaderToTubeSpatialObject
 * \brief This class is intended to make the connection between the TubeReader
 * and its output, the TubeSpatialObject. 
 *
 * With this class it is possible to enforce encapsulation of the Reader and
 * the TubeSpatialObject, and make their GetTube() and SetTube() methods
 * private, so that developers cannot gain access to the ITK or VTK layers of
 * these two classes.
 */
class TubeReaderToTubeSpatialObject
{
public:

  template < class TReader, class TTubeSpatialObject >
  static void 
  ConnectTube(  TReader * reader, 
                TTubeSpatialObject * tubeSpatialObject )
    {
    tubeSpatialObject->SetTubeSpatialObject(
                                    reader->GetITKTubeSpatialObject());
    }

}; // end of TubeReaderToTubeSpatialObject class

} // end of Friend namespace


/** \class TubeReader
 * 
 * \brief This class reads 3D Tube in the metaIO format.
 *
 * Tubular structures are quite common in human anatomy. This class is intended
 * to read groups of tubular structrures from files in metaIO format. Typical
 * these structures are the result of a segmentation method applied on
 * pre-operative images.
 *
 * \image html  igstkTubeReader.png "Tube Reader State Machine Diagram"
 * \image latex igstkTubeReader.eps "Tube Reader State Machine Diagram"
 *
 * \ingroup Readers
 */
class TubeReader : public SpatialObjectReader<3>
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TubeReader, SpatialObjectReader<3> )

public:

  /** Typedefs */
  typedef Superclass::SpatialObjectType      SpatialObjectType;
  typedef Superclass::GroupSpatialObjectType GroupSpatialObjectType;
  typedef SpatialObjectType::ConstPointer    SpatialObjectTypeConstPointer;
  typedef igstk::TubeObject                  TubeType;
  typedef itk::TubeSpatialObject<3>          TubeSpatialObjectType;

  /** Event type */
  igstkEventMacro( TubeReaderEvent,ObjectReaderEvent )
  igstkEventMacro( TubeReadingErrorEvent, ObjectReadingErrorEvent )
  igstkLoadedObjectEventMacro( TubeModifiedEvent, TubeReaderEvent, TubeType )

  /** Declare the TubeReaderToTubeSpatialObject class to be a friend 
   *  in order to give it access to the private method 
   *  GetITKTubeSpatialObject(). */
  igstkFriendClassMacro( igstk::Friends::TubeReaderToTubeSpatialObject );

protected:

  /** Constructor and Destructor */
  TubeReader();
  ~TubeReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** This method request Object read. This method is intended to be
   *  invoked ONLY by the State Machine of the superclass. */
  void AttemptReadObjectProcessing();

  /** This method will invoke the TubeModifiedEvent */
  void ReportObjectProcessing();

  /** Connect the ITK TubeSpatialObject to the output TubeSpatialObject */
  void ConnectTube();

private:

  TubeReader(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  // FIXME : This must be replaced with StateMachine logic
  virtual TubeSpatialObjectType * GetITKTubeSpatialObject() const;

  TubeType::Pointer              m_Tube;
  TubeSpatialObjectType::Pointer m_TubeSpatialObject;
};

} // end namespace igstk


#endif // __igstkTubeReader_h
