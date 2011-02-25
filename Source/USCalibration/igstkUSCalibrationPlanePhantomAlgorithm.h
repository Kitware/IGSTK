#ifndef __igstkUSCalibrationPlanePhantomAlgorithm_h
#define __igstkUSCalibrationPlanePhantomAlgorithm_h

#include "igstkUSCalibrationAlgorithm.h"
#include "igstkTransform.h"

namespace igstk
{


struct PlanePhantomUSCalibrationParametersEstimatorDataType {
    Transform T2;  
    itk::Point< double, 2 > q;
};

/** \class USCalibrationPointerAlgorithm
 * 
 *  \brief This class implements the calibrated pointer tip based US calibration 
 *         algorithm.
 *
 */
class USCalibrationPlanePhantomAlgorithm : 
  public USCalibrationAlgorithm< PlanePhantomUSCalibrationParametersEstimatorDataType >
{
public:
  typedef USCalibrationPlanePhantomAlgorithm                      Self;
  typedef ::itk::SmartPointer< Self >                             Pointer;
  typedef ::itk::SmartPointer< const Self >                       ConstPointer;
  typedef PlanePhantomUSCalibrationParametersEstimatorDataType    DataType;
  igstkNewMacro( Self );

protected:

  USCalibrationPlanePhantomAlgorithm( void );
  virtual ~USCalibrationPlanePhantomAlgorithm( void );

  virtual bool InternalComputeCalibrationProcessing();
};

} // end namespace igstk

#endif //__igstkUSCalibrationPlanePhantomAlgorithm_h
