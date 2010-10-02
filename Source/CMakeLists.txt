#
#  Create a library with WebPImageIO
#
SET(ITKIO_SRCS
  itkWebPImageIO.cxx
  itkWebPImageIOFactory.cxx
)

ADD_LIBRARY(WebPImageIO ${ITKIO_SRCS})

TARGET_LINK_LIBRARIES(WebPImageIO ITKIO ${WEBP_LIBRARIES})
