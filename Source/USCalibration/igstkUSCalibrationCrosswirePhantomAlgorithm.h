#ifndef __igstkUSCalibrationCrosswirePhantomAlgorithm_h
#define __igstkUSCalibrationCrosswirePhantomAlgorithm_h

#include "igstkUSCalibrationAlgorithm.h"
#include "igstkTransform.h"

namespace igstk
{

/** \class USCalibrationCrosswirePhantomAlgorithm
 * 
 *  \brief This class implements the crosswire based US calibration 
 *         algorithm.
 *
 */
class USCalibrationCrosswirePhantomAlgorithm : 
  public USCalibrationAlgorithm< std::pair< Transform, itk::Point< double, 2 > > >
{
public:
  typedef USCalibrationCrosswirePhantomAlgorithm  Self;
  typedef ::itk::SmartPointer< Self >       Pointer;
  typedef ::itk::SmartPointer< const Self > ConstPointer;
  igstkNewMacro( Self );

  typedef itk::Point< double, 2 >                     PointType;
  typedef igstk::Transform                            InputTransformType;
  typedef std::pair< InputTransformType, PointType >  DataType;

protected:

  USCalibrationCrosswirePhantomAlgorithm  ( void );
  virtual ~USCalibrationCrosswirePhantomAlgorithm ( void );

  virtual bool InternalComputeCalibrationProcessing();
};

} // end namespace igstk

#endif //__igstkUSCalibrationCrosswirePhantomAlgorithm_h
