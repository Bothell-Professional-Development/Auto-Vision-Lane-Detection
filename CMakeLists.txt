cmake_minimum_required (VERSION 2.6)
project (EBAutoVisionLaneDetection)

ADD_SUBDIRECTORY(detector)

if(WIN32)
    ADD_SUBDIRECTORY(trainer)
endif()

#Create gitignore for build dir
file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "/*")