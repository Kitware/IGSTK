PROJECT( LiverTumorSegmentation )

#
#  Define the list of source files
#
SET(LIVER_TUMOR_SEGMENTATION_SOURCES
  ImageSliceViewer01.cxx
  VolumeViewer.cxx
  LiverTumorSegmentation.cxx
  fltkProgressBar.cxx
  )

ADD_EXECUTABLE( LiverTumorSegmentation main.cxx ${LIVER_TUMOR_SEGMENTATION_SOURCES})

FLTK_WRAP_UI(LiverTumorSegmentation 
  LiverTumorSegmentationGUI.fl 
  )

INCLUDE_DIRECTORIES(
  ${LiverTumorSegmentation_SOURCE_DIR}
  ${LiverTumorSegmentation_BINARY_DIR}
  )

TARGET_LINK_LIBRARIES( LiverTumorSegmentation IGSTK )


