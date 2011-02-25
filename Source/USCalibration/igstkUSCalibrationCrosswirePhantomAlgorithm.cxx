#include "igstkUSCalibrationCrosswirePhantomAlgorithm.h"
#include "SinglePointTargetUSCalibrationParametersEstimator.h"
#include "RANSAC.h"
#include "igstkAffineTransform.h"
#include "itksys/SystemTools.hxx"

namespace igstk
{ 


USCalibrationCrosswirePhantomAlgorithm::USCalibrationCrosswirePhantomAlgorithm()
{
}


USCalibrationCrosswirePhantomAlgorithm::~USCalibrationCrosswirePhantomAlgorithm()
{
}


bool
USCalibrationCrosswirePhantomAlgorithm::InternalComputeCalibrationProcessing()
{
  if( this->m_Data.empty() ) 
  {
    return false;
  }

          //translate the data from IGSTK based to LSQRRecipes based
  typedef lsqrRecipes::SingleUnknownPointTargetUSCalibrationParametersEstimator::DataType lsqrRecipesDataType; 
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

  lsqrRecipesDataType dataElement;
  std::vector< lsqrRecipesDataType > data;

  std::vector<DataType>::iterator it, dataEnd;
  dataEnd = this->m_Data.end();
  for( it=this->m_Data.begin(); it!=dataEnd; it++ ) 
  {
            //get the transformation
    InputTransformType &T = ( *it ).first;
    T.ExportTransform( *matrix );
    dataElement.T2.setTranslation( matrix->GetElement(0, 3), 
                                   matrix->GetElement(1, 3), 
                                   matrix->GetElement(2, 3));
    dataElement.T2.setRotationMatrix(
      matrix->GetElement( 0, 0 ), matrix->GetElement( 0, 1 ), matrix->GetElement( 0, 2 ), 
      matrix->GetElement( 1, 0 ), matrix->GetElement( 1, 1 ), matrix->GetElement( 1, 2 ), 
      matrix->GetElement( 2, 0 ), matrix->GetElement( 2, 1 ), matrix->GetElement( 2, 2 ));
                //get the 2D point
    PointType &q = (*it).second;
    dataElement.q[0] = q[0];
    dataElement.q[1] = q[1];
    data.push_back( dataElement );
  }
  matrix->Delete();

  double maxDistanceBetweenPoints = 1.0;
  lsqrRecipes::SingleUnknownPointTargetUSCalibrationParametersEstimator 
    usCalibration(maxDistanceBetweenPoints);

                          //estimate using the RANSAC algorithm
  std::vector<double> estimatedUSCalibrationParameters;
  double desiredProbabilityForNoOutliers = 0.999;
  double percentageOfDataUsed;
  std::vector<bool> consensusSet;
  percentageOfDataUsed = 
    lsqrRecipes::RANSAC< lsqrRecipesDataType, double>::compute( estimatedUSCalibrationParameters, 
                                                    &usCalibration, 
                                                    data, 
                                                    desiredProbabilityForNoOutliers,
                                                    &consensusSet );
  
  if( estimatedUSCalibrationParameters.size() == 0 ) 
  {
    return false;
  }
  else 
  {
             //assess the error associated with the parameter values, we are
             //only interested in the data elements used for the estimate, this
             //is the in-sample error
    std::vector<double> errors;
    double minErr, maxErr, meanErr;

    std::vector<lsqrRecipesDataType> dataUsed;
    size_t i,n;
    n = data.size();
    for( i=0; i<n; i++ ) 
    {
    if( consensusSet[i] )
     dataUsed.push_back( data[i] );
    }

    lsqrRecipes::SingleUnknownPointTargetUSCalibrationParametersEstimator::getDistanceStatistics( estimatedUSCalibrationParameters, dataUsed,
                                                                                                  errors, minErr, maxErr, meanErr );
    this->m_EstimationError = meanErr;
     std::string estimationDate = 
       itksys::SystemTools::GetCurrentDateTime( "%Y %b %d %H:%M:%S" );
     igstk::AffineTransform *transform = new AffineTransform();

          //set the IGSTK transform
    AffineTransform::MatrixType m;
    AffineTransform::OffsetType o;
    
    o[0] =estimatedUSCalibrationParameters[3];
    o[1] =estimatedUSCalibrationParameters[4];
    o[2] =estimatedUSCalibrationParameters[5];

    m[0][0] = estimatedUSCalibrationParameters[11]; 
    m[0][1] = estimatedUSCalibrationParameters[14];
    m[0][2] = estimatedUSCalibrationParameters[17];
    m[1][0] = estimatedUSCalibrationParameters[12];
    m[1][1] = estimatedUSCalibrationParameters[15];
    m[1][2] = estimatedUSCalibrationParameters[18];
    m[2][0] = estimatedUSCalibrationParameters[13]; 
    m[2][1] = estimatedUSCalibrationParameters[16];
    m[2][2] = estimatedUSCalibrationParameters[19];
    
    transform->SetMatrixAndOffset( m, o, this->m_EstimationError,
                                   itk::NumericTraits<AffineTransform::TimePeriodType>::max() );
    this->m_TransformData = igstk::PrecomputedTransformData::New();
    this->m_TransformData->RequestInitialize( transform, estimationDate,
                                              std::string("US Calibration - Crosswire Phantom"),  
                                              this->m_EstimationError );
    return true;
  }
}


} //end namespace igstk
