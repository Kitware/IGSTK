SET(MTCMajorVersion 0)
SET(MTCMinorVersion 0)
SET(MTCRevision 0)
SET(MTCVERSION_STR "")

IF(MTC_INCLUDE_DIRECTORY AND EXISTS "${MTC_INCLUDE_DIRECTORY}/MTC.h")
  # Read the whole file:
  #
  FILE(READ "${MTC_INCLUDE_DIRECTORY}/MTC.h" MTC_VERSION_H_CONTENTS)

  # Split it into lines:
  #
  STRING(REGEX REPLACE ";" "\\\\;" MTC_VERSION_H_CONTENTS "${MTC_VERSION_H_CONTENTS}")
  STRING(REGEX REPLACE "\n" "EOL;" MTC_VERSION_H_CONTENTS "${MTC_VERSION_H_CONTENTS}")

  FOREACH(line ${MTC_VERSION_H_CONTENTS})
    # Save MTC_VERSION:
    #
    IF("${line}" MATCHES "^#define MTCMajorVersion ([0-9]+).*EOL$")
      STRING(REGEX REPLACE
        "^#define MTCMajorVersion ([0-9]+).*EOL$" "\\1"
        MTCMajorVersion "${line}")
    ENDIF("${line}" MATCHES "^#define MTCMajorVersion ([0-9]+).*EOL$")

    IF("${line}" MATCHES "^#define MTCMinorVersion ([0-9]+).*EOL$")
      STRING(REGEX REPLACE
        "^#define MTCMinorVersion ([0-9]+).*EOL$" "\\1"
        MTCMinorVersion "${line}")
    ENDIF("${line}" MATCHES "^#define MTCMinorVersion ([0-9]+).*EOL$")
 
  ENDFOREACH(line)

ENDIF(MTC_INCLUDE_DIRECTORY AND EXISTS "${MTC_INCLUDE_DIRECTORY}/MTC.h")

#MESSAGE("MTCMajorVersion='${MTCMajorVersion}'")
#MESSAGE("MTCMinorVersion='${MTCMinorVersion}'")
#MESSAGE("MTCVersion='${MTCVersion}'")

