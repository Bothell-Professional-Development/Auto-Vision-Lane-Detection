cmake_minimum_required (VERSION 2.6)
project (EBLaneDetectorTrainer)


set(TARGET_OS "win32")
set(LIB_PREFIX "")
set(LIB_POSTFIX "330")
set(LIB_POSTFIX_D "330d")
set(LIB_EXT "dll")


file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE
                  ${PROJECT_SOURCE_DIR}/interface/*.h
                  ${PROJECT_SOURCE_DIR}/source/*.cpp)
				  
#These directories contain the header files needed
include_directories(${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/include)
include_directories(${CMAKE_SOURCE_DIR}/interface)

#These directories contain the .lib files needed
#link_directories(${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs)

add_definitions(-DUNICODE)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})

#Link the necessary libraries to our executable
set(LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/)



find_library(OCV_CORE_LIBRARY ${LIB_PREFIX}opencv_core${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_CORE_LIBRARY_D ${LIB_PREFIX}opencv_core${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})

find_library(OCV_HIGHGUI_LIBRARY ${LIB_PREFIX}opencv_highgui${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_HIGHGUI_LIBRARY_D ${LIB_PREFIX}opencv_highgui${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})

find_library(OCV_IMGCODECS_LIBRARY ${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_IMGCODECS_LIBRARY_D ${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})

find_library(OCV_IMGPROC_LIBRARY ${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_IMGPROC_LIBRARY_D ${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})

find_library(OCV_ML_LIBRARY ${LIB_PREFIX}opencv_ml${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_ML_LIBRARY_D ${LIB_PREFIX}opencv_ml${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})

find_library(OCV_VIDEOIO_LIBRARY ${LIB_PREFIX}opencv_videoio${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_VIDEOIO_LIBRARY_D ${LIB_PREFIX}opencv_videoio${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})

find_library(OCV_FFMPEG_LIBRARY ${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX} hints ${LIBRARIES_DIR})
find_library(OCV_FFMPEG_LIBRARY_D ${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX_D} hints ${LIBRARIES_DIR})



target_link_libraries(${PROJECT_NAME} optimized ${OCV_CORE_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${OCV_CORE_LIBRARY_D})

#target_link_libraries(${PROJECT_NAME} ${OCV_FFMPEG_LIBRARY})

target_link_libraries(${PROJECT_NAME} optimized ${OCV_HIGHGUI_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${OCV_HIGHGUI_LIBRARY_D})

target_link_libraries(${PROJECT_NAME} optimized ${OCV_IMGCODECS_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${OCV_IMGCODECS_LIBRARY_D})

target_link_libraries(${PROJECT_NAME} optimized ${OCV_IMGPROC_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${OCV_IMGPROC_LIBRARY_D})

target_link_libraries(${PROJECT_NAME} optimized ${OCV_ML_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${OCV_ML_LIBRARY_D})

target_link_libraries(${PROJECT_NAME} optimized ${OCV_VIDEOIO_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${OCV_VIDEOIO_LIBRARY_D})

#target_link_libraries(${PROJECT_NAME} optimized ${OCV_FFMPEG_LIBRARY})
#target_link_libraries(${PROJECT_NAME} debug ${OCV_FFMPEG_LIBRARY_D})

#Copy the pre-built dlls to the output executable folder
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_core${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_highgui${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ml${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_videoio${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)

file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_core${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_highgui${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ml${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_videoio${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)

