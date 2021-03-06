#
#  Copyright (c) 2010 Insight Software Consortium
#
#  Redistribution and use is allowed according to the terms of the
#  Apache 2.0 License.
#


FIND_PATH(WEBP_INCLUDE_DIR vp8.h 
  /usr/local/include
  /usr/include
  /usr/local/include/vpx
  )

FIND_LIBRARY(WEBP_LIBRARY
  NAMES vpx
  PATHS /usr/lib /usr/local/lib
  )

IF (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)
    SET(WEBP_LIBRARIES ${WEBP_LIBRARY})
    SET(WEBP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
    SET(WEBP_FOUND "YES")
ELSE (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)
  SET(WEBP_FOUND "NO")
ENDIF (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)


IF (WEBP_FOUND)
   IF (NOT WEBP_FIND_QUIETLY)
      MESSAGE(STATUS "Found WEBP: ${WEBP_LIBRARIES}")
   ENDIF (NOT WEBP_FIND_QUIETLY)
ELSE (WEBP_FOUND)
   IF (WEBP_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find WEBP library")
   ENDIF (WEBP_FIND_REQUIRED)
ENDIF (WEBP_FOUND)

