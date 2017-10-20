cmake_minimum_required (VERSION 2.6)
project (EBLaneDetector)

if(WIN32)
    set(TARGET_OS "win32")
    set(LIB_PREFIX "")
    set(LIB_POSTFIX "330")
    set(LIB_POSTFIX_D "330d")
    set(LIB_EXT "dll")
else()
    set(TARGET_OS "raspbianStretch")
    set(LIB_PREFIX "lib")
    set(LIB_POSTFIX "")
    set(LIB_POSTFIX_D "")
    set(LIB_EXT "so")
endif()

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE
                  ${PROJECT_SOURCE_DIR}/interface/*.h
                  ${PROJECT_SOURCE_DIR}/source/*.cpp)
				  
#These directories contain the header files needed
include_directories(${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/include)
include_directories(${PROJECT_SOURCE_DIR}/interface)

#These directories contain the .lib files needed
# link_directories(${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})

#Link the necessary libraries to our executable
set(LIBRARIES_DIR ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/)

if(WIN32)
    find_library(OCV_CORE_LIBRARY ${LIB_PREFIX}opencv_core${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})
    find_library(OCV_HIGHGUI_LIBRARY ${LIB_PREFIX}opencv_highgui${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})
    find_library(OCV_IMGPROC_LIBRARY ${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})
    find_library(OCV_VIDEOIO_LIBRARY ${LIB_PREFIX}opencv_videoio${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})
else()
    find_library(OCV_CORE_LIBRARY opencv_core hints ${LIBRARIES_DIR})
    find_library(OCV_HIGHGUI_LIBRARY opencv_highgui hints ${LIBRARIES_DIR})
    find_library(OCV_IMGPROC_LIBRARY opencv_imgproc hints ${LIBRARIES_DIR})
    find_library(OCV_VIDEOIO_LIBRARY opencv_videoio hints ${LIBRARIES_DIR})
endif()

target_link_libraries(${PROJECT_NAME} ${OCV_CORE_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${OCV_HIGHGUI_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${OCV_IMGPROC_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${OCV_VIDEOIO_LIBRARY})

#Copy the pre-built dlls to the output executable folder
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_core${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_highgui${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_videoio${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
if(WIN32)
    file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
endif()

#Create gitignore for build dir
file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "/*")
