
#ifndef __ISIS_DicomImageReader_h__
#define __ISIS_DicomImageReader_h__

#include "DicomImageReaderGUI.h"


namespace ISIS
{

template< class TVolumeType >
class DicomImageReader : public DicomImageReaderGUI
{
public:

  typedef TVolumeType                           VolumeType;
  typedef typename VolumeType::Pointer          VolumePointer;
  typedef itk::ImageSeriesReader< VolumeType >  ReaderType; 
  typedef typename ReaderType::Pointer          ReaderPointer; 

public:
  DicomImageReader( void );

  virtual ~DicomImageReader( void );

  const VolumeType * GetOutput();

  void CollectSeriesAndSelectOne();

  bool IsVolumeLoaded() const;

  void AddObserver( itk::Command * );


protected:

  void ReadVolume();


private:

  ReaderPointer      m_Reader; 
  
  bool               m_VolumeIsLoaded;

  itk::Object::Pointer    m_Notifier;

};


}  // end namespace ISIS

  
#ifndef ITK_MANUAL_INSTANTIATION
#include "DicomImageReader.txx"
#endif

#endif



