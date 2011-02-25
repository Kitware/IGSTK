#ifndef __igstkUSCalibrationPointerAlgorithm_h
#define __igstkUSCalibrationPointerAlgorithm_h

#include "igstkUSCalibrationAlgorithm.h"
#include "igstkTransform.h"

namespace igstk
{


struct CalibratedPointerTargetUSCalibrationParametersEstimatorDataType {
    Transform T2;  
    itk::Point< double, 2 > q;
    itk::Point< double, 3 > p;
};

/** \class USCalibrationPointerAlgorithm
 * 
 *  \brief This class implements the calibrated pointer tip based US calibration 
 *         algorithm.
 *
 */
class USCalibrationPointerAlgorithm : 
  public USCalibrationAlgorithm< CalibratedPointerTargetUSCalibrationParametersEstimatorDataType >
{
public:
  typedef USCalibrationPointerAlgorithm                                    Self;
  typedef ::itk::SmartPointer< Self >                                      Pointer;
  typedef ::itk::SmartPointer< const Self >                                ConstPointer;
  typedef CalibratedPointerTargetUSCalibrationParametersEstimatorDataType  DataType;
  igstkNewMacro( Self );

protected:

  USCalibrationPointerAlgorithm  ( void );
  virtual ~USCalibrationPointerAlgorithm ( void );

  virtual bool InternalComputeCalibrationProcessing();
};

} // end namespace igstk

#endif //__igstkUSCalibrationPointerAlgorithm_h
