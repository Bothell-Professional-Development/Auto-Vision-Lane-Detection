cmake_minimum_required (VERSION 2.6)
project (EBLaneDetectorTrainer)

# Create generated folder to hold SVM output
set(SVM_OUTPUT_LEFT		${SVM_OUTPUT_FOLDER}/SVM_HOG_LeftLane.xml CACHE STRING "")
set(SVM_OUTPUT_RIGHT	${SVM_OUTPUT_FOLDER}/SVM_HOG_RightLane.xml CACHE STRING "")

set(SVM_INPUT			${PROJECT_BINARY_DIR}/_SVMTrainingData)
file(MAKE_DIRECTORY		${SVM_INPUT})

set(SVM_INPUT_LEFT_POS	${SVM_INPUT}/positive/left/ CACHE STRING "")
set(SVM_INPUT_LEFT_NEG  ${SVM_INPUT}/negative/left/ CACHE STRING "")
set(SVM_INPUT_RIGHT_POS ${SVM_INPUT}/positive/right/ CACHE STRING "")
set(SVM_INPUT_RIGHT_NEG ${SVM_INPUT}/negative/right/ CACHE STRING "")

set(SVM_DETECTOR_CONFIG ${CONFIG_FOLDER}/SVM_Trainer.cfg CACHE STRING "")
set(TRAINER_INPUT ${INPUT_FOLDER}/2.MP4 CACHE STRING "")

file(MAKE_DIRECTORY		${SVM_INPUT_LEFT_POS})
file(MAKE_DIRECTORY		${SVM_INPUT_LEFT_NEG})
file(MAKE_DIRECTORY		${SVM_INPUT_RIGHT_POS})
file(MAKE_DIRECTORY		${SVM_INPUT_RIGHT_NEG})

create_generated_header(${PROJECT_SOURCE_DIR}/interface/generated.h
	"#define CFG_FILE_PATH \"${SVM_DETECTOR_CONFIG}\""
)

create_generated_config(${SVM_DETECTOR_CONFIG}
	"SVM_LEFT_MODEL:${SVM_OUTPUT_LEFT}"
	"SVM_RIGHT_MODEL:${SVM_OUTPUT_RIGHT}"

	"POSITIVE_LEFT_FILE_PATH:${SVM_INPUT_LEFT_POS}"
	"NEGATIVE_LEFT_FILE_PATH:${SVM_INPUT_LEFT_NEG}"
	"POSITIVE_RIGHT_FILE_PATH:${SVM_INPUT_RIGHT_POS}"
	"NEGATIVE_RIGHT_FILE_PATH:${SVM_INPUT_RIGHT_NEG}"

	"LEFT_HISTOGRAM:${SVM_OUTPUT_FOLDER}/HistogramDistanceMatrix_HOG_Left.jpg"
	"RIGHT_HISTOGRAM:${SVM_OUTPUT_FOLDER}/HistogramDistanceMatrix_HOG_Right.jpg"
)


set(TARGET_OS "win32")
set(LIB_PREFIX "")
set(LIB_POSTFIX "330")
set(LIB_POSTFIX_D "330d")
set(LIB_EXT "dll")

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE
                  ${SVM_DETECTOR_CONFIG}
                  ${PROJECT_SOURCE_DIR}/interface/*.h
                  ${PROJECT_SOURCE_DIR}/source/*.cpp)
				  
#These directories contain the header files needed
include_directories(
	${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/include
	${PROJECT_SOURCE_DIR}/interface
	${CMAKE_SOURCE_DIR}/common_lib/inc)

#These directories contain the .lib files needed
#link_directories(${PROJECT_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs)

add_definitions(-DUNICODE)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})
target_link_libraries(${PROJECT_NAME} common_lib)

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

