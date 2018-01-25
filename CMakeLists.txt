cmake_minimum_required (VERSION 2.6)
project (EBAutoVisionLaneDetection)

# Create gitignore for build dir
file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "/*")



# Common Definitions
set(INIT_GENERATED      FALSE CACHE BOOL "")

set(SVM_OUTPUT_FOLDER	${PROJECT_BINARY_DIR}/_SVMModel)
file(MAKE_DIRECTORY		${SVM_OUTPUT_FOLDER})

set(CONFIG_FOLDER		${PROJECT_BINARY_DIR}/_Config)
file(MAKE_DIRECTORY		${CONFIG_FOLDER})

set(INPUT_FOLDER		${PROJECT_BINARY_DIR}/_Input)
file(MAKE_DIRECTORY		${INPUT_FOLDER})

set(SVM_OUTPUT_LEFT		${SVM_OUTPUT_FOLDER}/SVM_HOG_LeftLane.xml)
set(SVM_OUTPUT_RIGHT	${SVM_OUTPUT_FOLDER}/SVM_HOG_RightLane.xml)
set(SVM_INPUT			${PROJECT_BINARY_DIR}/_SVMInputData)



# Utility function
	function(create_generated_header path)
		set(CONTENT "")
		foreach(val ${ARGN})
			set(CONTENT "${CONTENT}\n${val}")
		endforeach()

		if(NOT EXISTS ${path} OR INIT_GENERATED)
			file(WRITE
				${path}
				"#pragma once\n"
		 		"\n"
				"/**\n"
				" * NOTE: This is a generated file and will be overwritten upon CMake config.\n"
				" */\n"
				${CONTENT}
				"\n")
		endif()
	endfunction(create_generated_header)

	function(create_generated_config path)
		set(CONTENT "")
		foreach(val ${ARGN})
			set(CONTENT "${CONTENT}\n${val}")
		endforeach()

		if(NOT EXISTS ${path} OR INIT_GENERATED)
			file(WRITE
				${path}
				"NOTE: This is a generated file and will be overwritten upon CMake config.\n"
				${CONTENT})
		endif()
	endfunction(create_generated_config)

ADD_SUBDIRECTORY(common_lib)
ADD_SUBDIRECTORY(detector)
if(WIN32)
    ADD_SUBDIRECTORY(trainer)
    # ADD_SUBDIRECTORY(tools)
endif()
