cmake_minimum_required (VERSION 2.6)
project (EBAutoVisionLaneDetection)

# Create gitignore for build dir
file(WRITE ${PROJECT_BINARY_DIR}/.gitignore "/*")

# Common Definitions
set(INIT_GENERATED_FILES	FALSE CACHE BOOL "")

set(SVM_OUTPUT_FOLDER		${PROJECT_BINARY_DIR}/_SVMModel)
file(MAKE_DIRECTORY			${SVM_OUTPUT_FOLDER})

set(CONFIG_FOLDER			${PROJECT_BINARY_DIR}/_Config)
file(MAKE_DIRECTORY			${CONFIG_FOLDER})

set(INPUT_FOLDER			${PROJECT_BINARY_DIR}/_Input)
file(MAKE_DIRECTORY			${INPUT_FOLDER})


# Utility function
	function(create_generated_header path)
		set(CONTENT "")
		foreach(val ${ARGN})
			set(CONTENT "${CONTENT}\n${val}")
		endforeach()

		if(NOT EXISTS ${path} OR INIT_GENERATED_FILES)
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

		if(NOT EXISTS ${path} OR INIT_GENERATED_FILES)
			file(WRITE
				${path}
				"NOTE: This is a generated file and will be overwritten upon CMake config.\n"
				${CONTENT})
		endif()
	endfunction(create_generated_config)

	function(copy_if_missing src_dir dst_dir file_name)
		set(src ${src_dir}/${file_name})
		set(dst ${dst_dir}/${file_name})

		if(NOT EXISTS ${src})
			message(SEND_ERROR "${src}" " doesn't exist")
			return()
		endif()

		if (EXISTS ${dst})
			message(STATUS ${dst} " already exists")
			return()
		endif()

		if(EXISTS "${src}" AND (NOT EXISTS "${dst}" OR INIT_GENERATED_FILES))
			file(COPY ${src} DESTINATION ${dst_dir})
		endif()
	endfunction(copy_if_missing)

ADD_SUBDIRECTORY(common_lib)
ADD_SUBDIRECTORY(detector)
ADD_SUBDIRECTORY(detector_lib)
if(WIN32)
    ADD_SUBDIRECTORY(trainer)
endif()
