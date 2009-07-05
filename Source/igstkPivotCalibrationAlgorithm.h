/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationAlgorithm.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPivotCalibrationAlgorithm_h
#define __igstkPivotCalibrationAlgorithm_h

#include <vector>
#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkTransform.h"

namespace igstk
{

/** \class PivotCalibrationAlgorithm
 * 
 *  \brief This class implements the pivot calibration algorithm, locating a 
 *         tools tip relative to a tracked coordinate system. 
 *
 *  We are interested in the location of a tool's tip relative to a tracked 
 *  coordinate system. By rotating the tool while its tip location remains 
 *  fixed we compute both the tip location relative to the tracked coordinate 
 *  system and the fixed point's coordinates in the tracker's coordinate 
 *  system. Every transformation \f$[R_i,\mathbf{t_i}]\f$ acquired by the 
 *  tracker during the rotation yields three equations in six unknowns,
 *  \f$R_i \mathbf{t_{tip}} + \mathbf{t_i} = \mathbf{t_{fixed}}\f$, with the 
 *  unknown 3x1 vectors \f$\mathbf{t_{tip}}\f$ and \f$\mathbf{t_{fixed}}\f$. 
 *  The solution is obtained in a least squares manner by acquiring multiple 
 *  transformation while rotating the tool around its tip resulting in the 
 *  following overdetermined equation system:
 *  \f$$ \left[
 *       \begin{array}{ccc}
 *         R_0 &        & -I \\
 *             & \vdots &    \\
 *         R_n &        & -I
 *       \end{array}
 *     \right]
 *     \left[
 *       \begin{array}{c}
 *         \bf{t_{tip}}\\
 *         \bf{t_{fixed}}
 *       \end{array}
 *     \right]
 *     =
 *     \left[
 *       \begin{array}{c}
 *         \bf{-t_0}\\
 *         \vdots\\
 *         \bf{-t_n}
 *       \end{array}
 *     \right] $$\f 
 * Which is solved using the pseudoinverse (singular value decomposition).
 */
class PivotCalibrationAlgorithm : public Object
{

public:
  
  /** Macro with standard traits declarations (Self, SuperClass, State 
   *  Machine etc.). */
  igstkStandardClassTraitsMacro( PivotCalibrationAlgorithm, Object )

  typedef itk::Point< double, 3 >    PointType;
  typedef igstk::Transform           TransformType;
  typedef std::vector<TransformType> TransformContainerType;

  /** This method adds the given transform to those used to perform the pivot 
   *  calibration. The method should only be invoked before calibration is 
   *  performed or after it has been reset or it will generate an 
   *  InvalidRequestErrorEvent.
   *  NOTE: The transform is assumed to be valid. */
   void RequestAddTransform( const TransformType & t );
  
  /** This method adds the given transforms to those used to perform the pivot 
   *  calibration. The method should only be invoked before calibration is 
   *  performed or after it has been reset or it will generate an 
   *  InvalidRequestErrorEvent.
   *  NOTE: The transforms are assumed to be valid. */
  void RequestAddTransforms( std::vector< TransformType > & t );

  /** This method resets the class to its initial state prior to computing a 
   *  calibration (all user provided transformations are removed). */
  void RequestResetCalibration();

  /** This method performs the calibration. It generates two events: 
   *  CalibrationSuccessEvent, and CalibrationFailureEvent, denoting success 
   *  or failure of the computation. Note that invoking this method prior to 
   *  setting transfromations is considered valid, it will simply generate a 
   *  CalibraitonFaliureEvent. In this way all sets of degenerate 
   *  transformations are treated alike (empty set, data with low variability 
   *  for example the same transform repeated many times).*/
  void RequestComputeCalibration();

  /** This method is used to request the calibration transformation.
   *  The method should only be invoked after a successful calibration. 
   *  It generates two events: CoordinateSystemTransformToResult, and 
   *  TransformNotAvailableEvent, respectively denoting that a calibration 
   *  transform is and isn't available. */
  void RequestCalibrationTransform();

  /** This method is used to request the pivot point, given in the coordinate 
   *  system in which the user supplied transforms were given.
   *  It generates two events: PointEvent, and InvalidRequestErrorEvent, 
   *  respectively denoting that the pivot point is and isn't available. */
  void RequestPivotPoint();

  /** This method is used to request the Root Mean Square Error (RMSE) of the 
   *  linear equation system used to compute the pivot point and calibration 
   *  transformation (RMSE of the vector Ax-b, see class description for more 
   *  details). It generates two events: DoubleTypeEvent, and 
   *  InvalidRequestErrorEvent, respectively denoting that the RMSE is and 
   *  isn't available. */
  void RequestCalibrationRMSE();

  /** This method sets the tolerance for the singular values of the matrix A 
   *  below which they are considered to be zero. The default setting is 
   *  PivotCalibrationAlgorithm::DEFAULT_SINGULAR_VALUE_THRESHOLD .
   *  The solution exists only if rank(A)=6 (columns are linearly independent). 
   *  The rank is evaluated using SVD(A) = USV^T, after zeroing out all 
   *  singular values (diagonal entries of S) that are less than the 
   *  tolerance. */
  void RequestSetSingularValueThreshold( double threshold );

  /** This event is generated if the pivot calibration computation fails. */
  igstkEventMacro( CalibrationFailureEvent, IGSTKEvent );

  /** This event is generated if the pivot calibration computation succeeds. */
  igstkEventMacro( CalibrationSuccessEvent, IGSTKEvent );

  /** Default threshold value under which singular values are considered to be 
   *  zero. */
  static const double DEFAULT_SINGULAR_VALUE_THRESHOLD;

protected:

  PivotCalibrationAlgorithm  ( void );
  virtual ~PivotCalibrationAlgorithm ( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  /** Check that there are transformations, and that their variability is 
   *  sufficient (rank(A) = 6) */
  bool CheckCalibrationDataValidity();

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToComputeCalibration );
  igstkDeclareStateMacro( CalibrationComputed );

  /** List of state machine inputs */
  igstkDeclareInputMacro( AddTransform );
  igstkDeclareInputMacro( SetSingularValueThreshold );
  igstkDeclareInputMacro( ComputeCalibration );
  igstkDeclareInputMacro( GetTransform  );
  igstkDeclareInputMacro( GetPivotPoint  );
  igstkDeclareInputMacro( GetRMSE  );
  igstkDeclareInputMacro( ResetCalibration );
  igstkDeclareInputMacro( CalibrationComputationSuccess  );
  igstkDeclareInputMacro( CalibrationComputationFailure  );

  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();  
  void AddTransformProcessing();
  void SetSingularValueThresholdProcessing();
  void ComputeCalibrationProcessing();
  void ResetCalibrationProcessing();
  void ReportSuccessInCalibrationComputationProcessing();
  void ReportFailureInCalibrationComputationProcessing();
  void GetTransformProcessing();
  void GetPivotPointProcessing();
  void GetRMSEProcessing();

  //transformations used for pivot calibration
  TransformContainerType m_Transforms;
  //transformations the user wants to add, because of the way the 
  //state machine works we need to first store them in 
  //a temporary variable. They will be moved to the m_Transforms 
  //container only if the state machine is in a state that
  //enables adding transformations
  TransformContainerType m_TmpTransforms;

  //computed translation between tracked coordinate system and pivot 
  //point
  TransformType m_Transform;

  //computed pivot point in the tracker's coordinate system
  itk::Point< double,3 > m_PivotPoint;

  //root mean squared error of the vector [Ax-b]
  double m_RMSE;

  //below this threshold the singular values of SVD(A) are set to zero
  double m_SingularValueThreshold;
  double m_TmpSingularValueThreshold;

};

} // end namespace igstk

#endif //__igstkPivotCalibrationAlgorithm_h
