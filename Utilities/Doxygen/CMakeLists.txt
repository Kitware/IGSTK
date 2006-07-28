#
# Build the documentation
#
INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake)

IF (BUILD_DOCUMENTATION)

  INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)

  #
  # Configure the script and the doxyfile, then add target
  #
  IF(NOT DOT_PATH)
    GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
  ENDIF(NOT DOT_PATH)

  FIND_PATH(IGSTK_SANDBOX_SOURCE_DIR igstkSandboxConfigure.h.in
    ${IGSTK_SOURCE_DIR}/Sandbox
    ${IGSTK_SOURCE_DIR}/../Sandbox
    ${IGSTK_SOURCE_DIR}/../IGSTKSandbox
    ${IGSTK_SOURCE_DIR}/../IGSTK-Sandbox)

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/doc_makeall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh)

  FILE(GLOB STATE_MACHINE_DOT_FILES "${IGSTK_BINARY_DIR}/Testing/Temporary/StateMachineDiagrams/*.dot")

  FOREACH(dotfile ${STATE_MACHINE_DOT_FILES})
    GET_FILENAME_COMPONENT(Imagebase ${dotfile} NAME)
    GET_FILENAME_COMPONENT(ImageBase ${Imagebase} NAME_WE)
    GET_FILENAME_COMPONENT(ImagePath ${dotfile} PATH)
    SET(pngfile ${ImagePath}/${ImageBase}.png)

    ADD_CUSTOM_COMMAND( 
    SOURCE    ${dotfile}
    COMMAND   ${DOT}
    ARGS      -Tpng 
              -o ${pngfile}
              ${dotfile}
    TARGET    StateMachineDiagrams
    OUTPUTS   ${pngfile}
    )
    SET(DOT_PNG_DEPS ${DOT_PNG_DEPS} ${pngfile})
  ENDFOREACH(dotfile)

  ADD_CUSTOM_TARGET(StateMachineDiagrams ALL DEPENDS ${DOT_PNG_DEPS})
  
  ADD_CUSTOM_TARGET(Documentation
    ${BASH}
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh
    DEPENDS ${DOT_PNG_DEPS}) 

  ADD_DEPENDENCIES(Documentation StateMachineDiagrams)

ENDIF (BUILD_DOCUMENTATION)
