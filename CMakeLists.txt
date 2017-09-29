cmake_minimum_required (VERSION 2.6)
project (EBLaneDetector)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE
                  ${PROJECT_SOURCE_DIR}/interface/*.h
                  ${PROJECT_SOURCE_DIR}/source/*.cpp)
				  
#These directories contain the header files needed
include_directories(${PROJECT_SOURCE_DIR}/opencv3.3/include)
include_directories(${PROJECT_SOURCE_DIR}/interface)

#These directories contain the .lib files needed
link_directories(${PROJECT_SOURCE_DIR}/opencv3.3/libs)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE})

#Link the necessary libraries to our executable
target_link_libraries(${PROJECT_NAME} opencv_core330d)
target_link_libraries(${PROJECT_NAME} opencv_highgui330d)
target_link_libraries(${PROJECT_NAME} opencv_imgproc330d)
target_link_libraries(${PROJECT_NAME} opencv_videoio330d)

#Copy the pre-built dlls to the output executable folder
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/resources/opencv_core330d.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/resources/opencv_highgui330d.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/resources/opencv_imgproc330d.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/resources/opencv_videoio330d.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/resources/opencv_imgcodecs330d.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)
file(COPY ${PROJECT_SOURCE_DIR}/opencv3.3/resources/opencv_ffmpeg330.dll DESTINATION ${PROJECT_BINARY_DIR}/Debug)