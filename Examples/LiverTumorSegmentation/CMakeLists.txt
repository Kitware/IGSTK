PROJECT( LiverTumorSegmentation )

#
#  Define the list of source files
#
SET(LIVER_TUMOR_SEGMENTATION_SRCS
  ImageSliceViewer01.cxx
  VolumeViewer.cxx
  LiverTumorSegmentation.cxx
  fltkProgressBar.cxx
  )


SET(LIVER_TUMOR_SEGMENTATION_HEADS
  ImageSliceViewer01.h
  VolumeViewer.h
  LiverTumorSegmentation.h
  fltkProgressBar.h
  )

ADD_EXECUTABLE( LiverTumorSegmentation main.cxx ${LIVER_TUMOR_SEGMENTATION_HEADS} ${LIVER_TUMOR_SEGMENTATION_SRCS})

FLTK_WRAP_UI(LiverTumorSegmentation 
  LiverTumorSegmentationGUI.fl 
  )

INCLUDE_DIRECTORIES(
  ${LiverTumorSegmentation_SOURCE_DIR}
  ${LiverTumorSegmentation_BINARY_DIR}
  )

TARGET_LINK_LIBRARIES( LiverTumorSegmentation IGSTK )


