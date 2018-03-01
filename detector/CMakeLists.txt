cmake_minimum_required (VERSION 2.6)
project (EBLaneDetector)

set(LIBBuild FALSE CACHE BOOL "LIBBuild")

# Create generated folder to hold SVM output
set(SVM_DETECTOR_CONFIG             ${CONFIG_FOLDER}/SVM_Detector.cfg CACHE STRING "")
set(DETECTOR_INPUT                  ${INPUT_FOLDER}/2.MP4 CACHE STRING "")
set(SVM_MODEL_LEFT                  ${SVM_OUTPUT_FOLDER}/SVM_HOG_LeftLaneVehicle.xml CACHE STRING "")
set(SVM_MODEL_RIGHT                 ${SVM_OUTPUT_FOLDER}/SVM_HOG_RightLaneVehicle.xml CACHE STRING "")

set(SIMULATOR_SVM_DETECTOR_CONFIG   ${CONFIG_FOLDER}/SVM_Detector_Simulator.cfg CACHE STRING "")
set(SIMULATOR_DETECTOR_INPUT        ${INPUT_FOLDER}/sim_training.avi CACHE STRING "")
set(SIMULATOR_SVM_MODEL_LEFT        ${SVM_OUTPUT_FOLDER}/SVM_HOG_LeftLaneSimulation.xml CACHE STRING "")
set(SIMULATOR_SVM_MODEL_RIGHT       ${SVM_OUTPUT_FOLDER}/SVM_HOG_RightLaneSimulation.xml CACHE STRING "")

copy_if_missing(${CMAKE_SOURCE_DIR}/tools/templates ${SVM_OUTPUT_FOLDER} SVM_HOG_LeftLaneSimulation.xml)
copy_if_missing(${CMAKE_SOURCE_DIR}/tools/templates ${SVM_OUTPUT_FOLDER} SVM_HOG_RightLaneSimulation.xml)
copy_if_missing(${CMAKE_SOURCE_DIR}/tools/templates ${SVM_OUTPUT_FOLDER} SVM_HOG_LeftLaneVehicle.xml)
copy_if_missing(${CMAKE_SOURCE_DIR}/tools/templates ${SVM_OUTPUT_FOLDER} SVM_HOG_RightLaneVehicle.xml)

copy_if_missing(//USBTH1MS002/USSilo/Automotive/Public/Bothell_Professional_Development/Auto-Vision-Lane-Detection    ${INPUT_FOLDER} 2.MP4)
copy_if_missing(//USBTH1MS002/USSilo/Automotive/Public/Bothell_Professional_Development/Auto-Vision-Lane-Detection    ${INPUT_FOLDER} sim_training.avi)

create_generated_header(${PROJECT_SOURCE_DIR}/interface/generated.h
	"#define CFG_FILE_PATH \"${SVM_DETECTOR_CONFIG}\""
)

create_generated_config(${SVM_DETECTOR_CONFIG}
	"SVM_LEFT_MODEL:${SVM_MODEL_LEFT}"
	"SVM_RIGHT_MODEL:${SVM_MODEL_RIGHT}"
	"DETECTOR_INPUT:${DETECTOR_INPUT}"

	"HORIZONTAL_RESOLUTION:960"
	"VERTICAL_RESOLUTION:540"
	"BOX_WIDTH:15"
	"BOX_HEIGHT:15"
	"HORIZONTAL_CENTER:550"
	"AVERAGING_WINDOW_SIZE:20"
	"VERTICAL_REGION_UPPER:320"
	"VERTICAL_REGION_LOWER:400"
	"HORIZONTAL_REGION_LEFT_OFFSET:200"
	"HORIZONTAL_REGION_RIGHT_OFFSET:200"
	"IDEAL_LEFT_LANE_MARKER_START_OFFSET:130"
	"IDEAL_LEFT_LANE_MARKER_END_OFFSET:40"
	"IDEAL_RIGHT_LANE_MARKER_START_OFFSET:170"
	"IDEAL_RIGHT_LANE_MARKER_END_OFFSET:35"
	"ADJACENT_LEFT_LANE_MARKER_START_OFFSET:50"
	"ADJACENT_LEFT_LANE_MARKER_END_OFFSET:130"
	"ADJACENT_RIGHT_LANE_MARKER_START_OFFSET:50"
	"ADJACENT_RIGHT_LANE_MARKER_END_OFFSET:130"
	"AVG_DISTANCE_FROM_LINE_THRESHOLD_FACTOR:2"
	"LEFT_LANE_MIN_ANGLE:-90"
	"LEFT_LANE_MAX_ANGLE:-34"
	"RIGHT_LANE_MIN_ANGLE:-155"
	"RIGHT_LANE_MAX_ANGLE:-90"
)

create_generated_config(${SIMULATOR_SVM_DETECTOR_CONFIG}
	"SVM_LEFT_MODEL:${SIMULATOR_SVM_MODEL_LEFT}"
	"SVM_RIGHT_MODEL:${SIMULATOR_SVM_MODEL_RIGHT}"
	"DETECTOR_INPUT:${SIMULATOR_DETECTOR_INPUT}"

	"HORIZONTAL_RESOLUTION:800"
	"VERTICAL_RESOLUTION:450"
	"BOX_WIDTH:25"
	"BOX_HEIGHT:25"
	"HORIZONTAL_CENTER:400"
	"AVERAGING_WINDOW_SIZE:5"
	"VERTICAL_REGION_UPPER:250"
	"VERTICAL_REGION_LOWER:420"
	"HORIZONTAL_REGION_LEFT_OFFSET:330"
	"HORIZONTAL_REGION_RIGHT_OFFSET:330"
	"IDEAL_LEFT_LANE_MARKER_START_OFFSET:210"
	"IDEAL_LEFT_LANE_MARKER_END_OFFSET:70"
	"IDEAL_RIGHT_LANE_MARKER_START_OFFSET:200"
	"IDEAL_RIGHT_LANE_MARKER_END_OFFSET:70"
	"ADJACENT_LEFT_LANE_MARKER_START_OFFSET:100"
	"ADJACENT_LEFT_LANE_MARKER_END_OFFSET:200"
	"ADJACENT_RIGHT_LANE_MARKER_START_OFFSET:100"
	"ADJACENT_RIGHT_LANE_MARKER_END_OFFSET:200"
	"AVG_DISTANCE_FROM_LINE_THRESHOLD_FACTOR:2.5"
	"LEFT_LANE_MIN_ANGLE:-120"
	"LEFT_LANE_MAX_ANGLE:-30"
	"RIGHT_LANE_MIN_ANGLE:-155"
	"RIGHT_LANE_MAX_ANGLE:-70"
)

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

if(LIBBuild)
    add_definitions(-DLIBBuild -DUNICODE -D_AFXDLL)
    add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCE})
else()
	add_definitions(-DUNICODE)
	add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} common_lib)
else()
    target_link_libraries(${PROJECT_NAME} common_lib pthread)
endif()

#Link the necessary libraries to our executable
set(LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/)

if(WIN32)
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
else()
    find_library(OCV_CORE_LIBRARY opencv_core hints ${LIBRARIES_DIR})
    find_library(OCV_CORE_LIBRARY_D opencv_core hints ${LIBRARIES_DIR})

    find_library(OCV_HIGHGUI_LIBRARY opencv_highgui hints ${LIBRARIES_DIR})
    find_library(OCV_HIGHGUI_LIBRARY_D opencv_highgui hints ${LIBRARIES_DIR})

    find_library(OCV_IMGCODECS_LIBRARY opencv_imgcodecs hints ${LIBRARIES_DIR})
    find_library(OCV_IMGCODECS_LIBRARY_D opencv_imgcodecs hints ${LIBRARIES_DIR})

    find_library(OCV_IMGPROC_LIBRARY opencv_imgproc hints ${LIBRARIES_DIR})
    find_library(OCV_IMGPROC_LIBRARY_D opencv_imgproc hints ${LIBRARIES_DIR})

    find_library(OCV_ML_LIBRARY opencv_ml hints ${LIBRARIES_DIR})
    find_library(OCV_ML_LIBRARY_D opencv_ml hints ${LIBRARIES_DIR})

    find_library(OCV_VIDEOIO_LIBRARY opencv_videoio hints ${LIBRARIES_DIR})
    find_library(OCV_VIDEOIO_LIBRARY_D opencv_videoio hints ${LIBRARIES_DIR})

    # find_library(OCV_FFMPEG_LIBRARY opencv_ffmpeg hints ${LIBRARIES_DIR})
endif()

target_link_libraries(${PROJECT_NAME} optimized ${OCV_CORE_LIBRARY} "-static")
target_link_libraries(${PROJECT_NAME} debug ${OCV_CORE_LIBRARY_D} "-static")

#target_link_libraries(${PROJECT_NAME} ${OCV_FFMPEG_LIBRARY})

target_link_libraries(${PROJECT_NAME} optimized ${OCV_HIGHGUI_LIBRARY} "-static")
target_link_libraries(${PROJECT_NAME} debug ${OCV_HIGHGUI_LIBRARY_D} "-static")

target_link_libraries(${PROJECT_NAME} optimized ${OCV_IMGCODECS_LIBRARY} "-static")
target_link_libraries(${PROJECT_NAME} debug ${OCV_IMGCODECS_LIBRARY_D} "-static")

target_link_libraries(${PROJECT_NAME} optimized ${OCV_IMGPROC_LIBRARY} "-static")
target_link_libraries(${PROJECT_NAME} debug ${OCV_IMGPROC_LIBRARY_D} "-static")

target_link_libraries(${PROJECT_NAME} optimized ${OCV_ML_LIBRARY} "-static")
target_link_libraries(${PROJECT_NAME} debug ${OCV_ML_LIBRARY_D} "-static")

target_link_libraries(${PROJECT_NAME} optimized ${OCV_VIDEOIO_LIBRARY} "-static")
target_link_libraries(${PROJECT_NAME} debug ${OCV_VIDEOIO_LIBRARY_D} "-static")

#target_link_libraries(${PROJECT_NAME} optimized ${OCV_FFMPEG_LIBRARY})
#target_link_libraries(${PROJECT_NAME} debug ${OCV_FFMPEG_LIBRARY_D})

#Copy the pre-built dlls to the output executable folder
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_core${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_highgui${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ml${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_videoio${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
if(WIN32)
    file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Release)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_core${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_highgui${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgcodecs${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_imgproc${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ml${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_videoio${LIB_POSTFIX_D}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
if(WIN32)
    file(COPY ${CMAKE_SOURCE_DIR}/opencv3.3/${TARGET_OS}/libs/${LIB_PREFIX}opencv_ffmpeg${LIB_POSTFIX}.${LIB_EXT} DESTINATION ${PROJECT_BINARY_DIR}/Debug)
endif()


