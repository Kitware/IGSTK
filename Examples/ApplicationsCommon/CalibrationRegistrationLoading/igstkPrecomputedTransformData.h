#ifndef __igstkPrecomputedTransformData_h
#define __igstkPrecomputedTransformData_h

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include <itkTransform.h>

namespace igstk
{
/** \class PrecomputedTransformData
 * 
 *  \brief This template is a container for precomputed transformations. 
 *         The container holds the transformation, its associated estimation 
 *         error, computation date, and human readable description.
 *
 *  This template allows the programmer to load a precomputed transformation 
 *  for use in an IGSTK based application. The transformation is arbitrary 
 *  (i.e. rigid or non-rigid). This container is usefull for loading calibration
 *  data for tools, and preoperative registration of image data such as CT to 
 *  MR. 
 *  
 *  Note that the transformation is limited to decendants of 
 *  itk::Transform<double,3,3>.
 */
class PrecomputedTransformData : public Object
{
public:

  /**Macro with standard traits declarations (Self, SuperClass, state machine 
    * etc.). */
  igstkStandardClassTraitsMacro( PrecomputedTransformData, Object )

  typedef ::itk::Transform<double, 3, 3>  TransformType;
  typedef std::string                     DateType;
  typedef double                          ErrorType;


  /** This method Initializes the transformation information according to the 
   *  given data. All of the data is required at this time in a manner similar 
   *  to standard object construction.
   *  NOTE: The temporal validity of the transformation is not checked.*/
  void RequestInitialize( TransformType::Pointer transform,
                          const DateType &date,
                          const std::string &description,
                          const ErrorType err );


  /** This method is used to request the calibration/registration 
   *  transformation. 
   *  It generates two events: TransformationTypeEvent, and 
   *  InvalidRequestErrorEvent (denoting that the transformation data
   *  is not available). */
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

  /** This event is generated when the transformation is requested and the
   *  data was initialized. */
  igstkLoadedEventMacro( TransformTypeEvent, IGSTKEvent,
                         TransformType::Pointer );

  /** This event is generated when the transformation estimation error is 
   *  requested and the data was initialized. */
  igstkLoadedEventMacro( TransformErrorTypeEvent, IGSTKEvent,
                         ErrorType );

  /** This event is generated when the transformation computation date is 
   *  requested and the data was initialized. */
  igstkLoadedEventMacro( TransformDateTypeEvent, IGSTKEvent,
                         DateType );
protected:

  PrecomputedTransformData( void );
  ~PrecomputedTransformData( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( Initialized );
  
  /** List of state machine inputs */
  igstkDeclareInputMacro( Initialize );
  igstkDeclareInputMacro( GetTransform  );
  igstkDeclareInputMacro( GetEstimationError  );
  igstkDeclareInputMacro( GetDescription  );
  igstkDeclareInputMacro( GetDate  );


  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();  
  void InitializeProcessing();
  void GetTransformProcessing();
  void GetEstimationErrorProcessing();
  void GetDescriptionProcessing();
  void GetDateProcessing();

  TransformType::Pointer m_TmpTransform;
  TransformType::Pointer m_Transform;

  ErrorType m_TmpTransformationError;
  ErrorType m_TransformationError;
  
  std::string m_TmpTransformationDescription;
  std::string m_TransformationDescription;

  DateType m_TmpComputationDateAndTime;
  DateType m_ComputationDateAndTime;
};

} // end namespace igstk

#endif // __igstkPrecomputedTransformData_h
