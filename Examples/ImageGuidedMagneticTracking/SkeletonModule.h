#ifndef _IGSTK_SKELETONMODULE_H_
#define _IGSTK_SKELETONMODULE_H_


#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkPointSet.h"


template <class TInputImage, class TOutputImage>
class SkeletonModule  
{
public:

  typedef typename TInputImage::PixelType  InputPixelType;

  typedef typename TInputImage::IndexType  InputIndexType;

  typedef typename TInputImage::SizeType   InputSizeType;

  typedef typename TInputImage::RegionType InputRegionType;

  typedef typename TInputImage::SpacingType InputSpacingType;

  typedef typename TInputImage::PointType InputOriginType;
  
  typedef typename TOutputImage::PixelType OutputPixelType;

  typedef typename TOutputImage::Pointer OutputTypePointer;

  typedef itk::ImportImageFilter<signed short, 3> ImportFilterType;

  typedef typename ImportFilterType::Pointer ImportFilterTypePointer;

  typedef itk::PointSet<double> PointSetType;  

public:

  const TInputImage* m_pInputImage;
  
  signed short * m_pConnectedBuffer;

  ImportFilterTypePointer m_pImportFilter;

  void(* m_ProgressCallbackFunc)(void*, double);

  void* m_CallData;

  double m_ClusterRadius;

  PointSetType::Pointer m_Skeleton;

  int m_SkeletonPointNum;

public:

  void SetInput(const TInputImage* pInputImage);

  TOutputImage* GetOutput();

  void ExtractSkeleton();

  void TestSkeleton();

  void DeutschSkeleton(signed char* buffer, int x, int y);

  void MorphologySkeleton(signed char* buffer, int x, int y);
	
  SkeletonModule();
	
  virtual ~SkeletonModule();

  void SetProgressCallback(void(* func)(void*, double), void* cdata);

  void Cluster2DSkeleton(signed char* buffer, int x, int y, int slice);

  void SetClusterRadius(double radius);

  void SmoothSkeleton();

};


#ifndef IGSTK_MANUAL_INSTALLTION
#include "SkeletonModule.txx"
#endif

#endif 
