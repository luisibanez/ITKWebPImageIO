#
#
#
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(itkWebPImageIOFactoryTest01 itkWebPImageIOFactoryTest01.cxx)
TARGET_LINK_LIBRARIES(itkWebPImageIOFactoryTest01 WebPImageIO)

ADD_EXECUTABLE(itkWebPImageIOTest00 itkWebPImageIOTest00.cxx)
TARGET_LINK_LIBRARIES(itkWebPImageIOTest00 WebPImageIO)

ADD_EXECUTABLE(itkWebPImageIOTest01 itkWebPImageIOTest01.cxx)
TARGET_LINK_LIBRARIES(itkWebPImageIOTest01 WebPImageIO)

ADD_TEST(WebPTest00  ${CXX_TEST_PATH}/itkWebPImageIOFactoryTest01 )
ADD_TEST(WebPTest01  ${CXX_TEST_PATH}/itkWebPImageIOTest00 )

ADD_TEST(WebPTest02  ${CXX_TEST_PATH}/itkWebPImageIOTest01
 ${WebP_DATA_ROOT}/1_original.webp
 ${TEMP}/1_original.png
 )

ADD_TEST(WebPTest03  ${CXX_TEST_PATH}/itkWebPImageIOTest01
 ${WebP_DATA_ROOT}/3_original.webp
 ${TEMP}/3_original.png
 )

ADD_TEST(WebPTest04  ${CXX_TEST_PATH}/itkWebPImageIOTest01
 ${WebP_DATA_ROOT}/4_original.webp
 ${TEMP}/4_original.png
 )

ADD_TEST(WebPTest05  ${CXX_TEST_PATH}/itkWebPImageIOTest01
 ${WebP_DATA_ROOT}/6_original.webp
 ${TEMP}/6_original.png
 )

ADD_TEST(WebPTest06  ${CXX_TEST_PATH}/itkWebPImageIOTest01
 ${WebP_DATA_ROOT}/10_original.webp
 ${TEMP}/10_original.png
 )
