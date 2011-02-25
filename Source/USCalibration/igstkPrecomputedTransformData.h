/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPrecomputedTransformData.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPrecomputedTransformData_h
#define __igstkPrecomputedTransformData_h

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkTransformBase.h"


namespace igstk
{
/** \class PrecomputedTransformData
 * 
 *  \brief This class is a container for precomputed transformations. 
 *         The container holds the transformation, its associated estimation 
 *         error, computation date, and human readable description.
 *
 *  This class allows the programmer to load a precomputed transformation 
 *  for use in an IGSTK based application. The transformation is arbitrary 
 *  (e.g rigid, affine, perspective...). This container is usefull for loading 
 *  calibration data for tools, and preoperative registration of image data such 
 *  as CT to MR. 
 */
class PrecomputedTransformData : public Object
{
public:

  /**Macro with standard traits declarations (Self, SuperClass, state machine 
    * etc.). */
  igstkStandardClassTraitsMacro( PrecomputedTransformData, Object )

  typedef igstk::TransformBase  TransformType;
  typedef std::string           DateType;
  typedef double                ErrorType;


  /** This method Initializes the transformation information according to the 
   *  given data. All of the data is required at this time in a manner similar 
   *  to standard object construction.
   *  The method generates two types of eventes: InitializationSuccessEvent,
   *  InitializationFailureEvent. Initialization fails if the transformation
   *  pointer is of a type that is not supported or is NULL. 
   *  NOTE: The temporal validity of the transformation is assumed to be 
   *        infinite.
   */
  void RequestInitialize( TransformType *transform,
                          const DateType &date,
                          const std::string &description,
                          const ErrorType err );


  /** This method is used to request the calibration/registration 
   *  transformation. 
   *  It generates three events: TransformationType3DEvent, 
   *  PerspectiveTransformTypeEvent and InvalidRequestErrorEvent 
   * (denoting that the transformation data is not available). */
  void RequestTransform();


  /** This method is used to request the calibration/registration error (e.g. 
   *  RMS error associated with pivot calibration, or expected TRE for 
   *  paired point rigid registration). 
   *  It generates two events: TransformErrorTypeEvent, and 
   *  InvalidRequestErrorEvent (denoting that the error data is not 
   *  available). */
  void RequestEstimationError();


  /** This method is used to request the calibration/registration description 
   *  (e.g. "Claron bayonet pointer", or "CT to MR transformation").
   *  It generates two events: StringEvent, and InvalidRequestErrorEvent 
   *  (denoting that the error data is not available). */
  void RequestTransformDescription();


  /** This method is used to request the calibration/registration date. 
   *  (e.g. "Fri May 14 16:00:00 1948").
   *  It generates two events: TransformDateTypeEvent, and 
   *  InvalidRequestErrorEvent (denoting that the error data is not 
   *  available). */
  void RequestComputationDateAndTime();

  /** \class This event is generated when the transformation is requested and 
    *        the data was initialized. */
  class  TransformTypeEvent : public IGSTKEvent
    {
    public:  
    TransformTypeEvent() {} 
    virtual ~TransformTypeEvent() {} 
    virtual const char * GetEventName() const 
      { 
      return "TransformTypeEvent"; 
      } 
    virtual bool CheckEvent(const ::itk::EventObject* e) const 
      { return dynamic_cast<const TransformTypeEvent*>(e); } 
    virtual ::itk::EventObject* MakeObject() const 
      { return new TransformTypeEvent(); } 
    TransformTypeEvent(const TransformTypeEvent&s) : IGSTKEvent(s){}; 
    PrecomputedTransformData::TransformType* Get() const 
      { return m_Payload; }  
    void Set( PrecomputedTransformData::TransformType* p ) 
      { m_Payload = p; }  
    private: 
    void operator=(const TransformTypeEvent &);  
    TransformType*  m_Payload; 
    };


  /** This event is generated when the transformation estimation error is 
   *  requested and the data was initialized. */
  igstkLoadedEventMacro( TransformErrorTypeEvent, IGSTKEvent,
                         ErrorType );


  /** This event is generated when the transformation computation date is 
   *  requested and the data was initialized. */
  igstkLoadedEventMacro( TransformDateTypeEvent, IGSTKEvent,
                         DateType );

    /** This event is generated if the initialization succeeds. */
  igstkEventMacro( InitializationSuccessEvent, IGSTKEvent );

  /** This event is generated if the initialization fails (e.g. given tool 
    *  is null). */
  igstkEventMacro( InitializationFailureEvent, IGSTKEvent );

protected:

  PrecomputedTransformData( void );
  virtual ~PrecomputedTransformData( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( Initialized );
  
  /** List of state machine inputs */
  igstkDeclareInputMacro( Initialize );
  igstkDeclareInputMacro( Failed  );
  igstkDeclareInputMacro( Succeeded  );
  igstkDeclareInputMacro( GetTransform  );
  igstkDeclareInputMacro( GetEstimationError  );
  igstkDeclareInputMacro( GetDescription  );
  igstkDeclareInputMacro( GetDate  );


  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();  
  void InitializeProcessing();
  void ReportInitializationFailureProcessing();
  void ReportInitializationSuccessProcessing();
  void GetTransformProcessing();
  void GetEstimationErrorProcessing();
  void GetDescriptionProcessing();
  void GetDateProcessing();

  TransformType *m_TmpTransform;
  TransformType *m_Transform;

  ErrorType m_TmpTransformationError;
  ErrorType m_TransformationError;
  
  std::string m_TmpTransformationDescription;
  std::string m_TransformationDescription;

  DateType m_TmpComputationDateAndTime;
  DateType m_ComputationDateAndTime;
};

} // end namespace igstk

#endif // __igstkPrecomputedTransformData_h
