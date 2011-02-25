#ifndef __igstkUSCalibrationAlgorithm_h
#define __igstkUSCalibrationAlgorithm_h

#include "igstkPrecomputedTransformData.h"
#include "igstkMacros.h"
#include "igstkObject.h"

namespace igstk
{

/** \class USCalibrationAlgorithm
 * 
 *  \brief This class is a base class for all US calibration algorithms.
 *
 */
template<class DataType>
class USCalibrationAlgorithm : public Object
{
public:
    /** Macro with standard traits declarations (Self, SuperClass, State 
   *  Machine etc.). */
  igstkStandardTemplatedAbstractClassTraitsMacro( USCalibrationAlgorithm, Object )

  /** This method adds the given data element to those used to perform the US 
   *  calibration. The method should only be invoked before calibration is 
   *  performed or after it has been reset or it will generate an 
   *  InvalidRequestErrorEvent.*/
  void RequestAddDataElement( const DataType & d );
  
  /** This method adds the given data elements to those used to perform the US 
   *  calibration. The method should only be invoked before calibration is 
   *  performed or after it has been reset or it will generate an 
   *  InvalidRequestErrorEvent.*/
  void RequestAddDataElements( std::vector< DataType > & d );

  /** This method resets the class to its initial state prior to computing a 
   *  calibration (all user provided data is removed).*/
  void RequestResetCalibration();

  /** This method performs the calibration. It generates two events: 
   *  CalibrationSuccessEvent, and CalibrationFailureEvent, denoting success 
   *  or failure of the computation. Note that invoking this method prior to 
   *  setting data is considered valid, it will simply generate a 
   *  CalibraitonFaliureEvent. In this way all sets of degenerate 
   *  transformations are treated alike (empty set, data with low variability 
   *  for example the same data element repeated many times).*/
  void RequestComputeCalibration();

  /** This method is used to request the calibration transformation.
   *  The method should only be invoked after a successful calibration. 
   *  It generates two events: TransformResultEvent, and 
   *  TransformNotAvailableEvent, respectively denoting that a calibration 
   *  transform is and isn't available. */
  void RequestCalibrationTransform();

  /** This method is used to request the calibration estimation error. It 
   *  generates two events: DoubleTypeEvent, and InvalidRequestErrorEvent, 
   *  respectively denoting that the calibration estimation error is and isn't 
   *  available. */
  void RequestCalibrationError();

  /** This event is generated if the calibration computation fails. */
  igstkEventMacro( CalibrationFailureEvent, IGSTKEvent );

  /** This event is generated if the calibration computation succeeds. */
  igstkEventMacro( CalibrationSuccessEvent, IGSTKEvent );

  /** This event is generated when the transformation data is requested and the
   *  data is available. */
  igstkLoadedEventMacro( TransformResultEvent, IGSTKEvent,
                         PrecomputedTransformData::Pointer );

protected:

  USCalibrationAlgorithm  ( void );
  virtual ~USCalibrationAlgorithm ( void );
  
  //this method actually does the work
  virtual bool InternalComputeCalibrationProcessing() = 0;
  //bool InternalComputeCalibrationProcessing() {return false;}

                //data used for US calibration
  std::vector<DataType> m_Data;
           //estimated transformation between tracked coordinate system and the
           //US image
  PrecomputedTransformData::Pointer m_TransformData;

         //estimation error
  double m_EstimationError;

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToComputeCalibration );
  igstkDeclareStateMacro( CalibrationComputed );

  /** List of state machine inputs */
  igstkDeclareInputMacro( AddDataElement );
  igstkDeclareInputMacro( ComputeCalibration );
  igstkDeclareInputMacro( CalibrationComputationSuccess  );
  igstkDeclareInputMacro( CalibrationComputationFailure  );
  igstkDeclareInputMacro( GetTransform  );
  igstkDeclareInputMacro( GetCalibrationError  );
  igstkDeclareInputMacro( ResetCalibration );

  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();  
  void AddDataProcessing();
  void ComputeCalibrationProcessing();
  void ResetCalibrationProcessing();
  void ReportSuccessInCalibrationComputationProcessing();
  void ReportFailureInCalibrationComputationProcessing();
  void GetTransformProcessing();  
  void GetCalibrationErrorProcessing();

             //data the user wants to add, because of the way the 
             //state machine works we need to first store them in 
             //a temporary variable. They will be moved to the m_Data 
             //container only if the state machine is in a state that
             //enables adding data
  std::vector<DataType> m_TmpData;
};

} // end namespace igstk

#include "igstkUSCalibrationAlgorithm.txx" //the implementation is in this file

#endif //__igstkUSCalibrationAlgorithm_h
