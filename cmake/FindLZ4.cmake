# FindLZ4.cmake
# - Try to find lz4 and create an imported target 'LZ4'

find_path(LZ4_INCLUDE_DIR lz4.h)

#  Bail out with proper error message if LZ4_INCLUDE_DIR wasn't found
if(NOT LZ4_INCLUDE_DIR)
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(LZ4 REQUIRED_VARS LZ4_INCLUDE_DIR)
endif()

if(NOT TARGET LZ4)
	add_library(LZ4 UNKNOWN IMPORTED)

	# Find the library
	find_library(LZ4_LIBRARY_RELEASE "lz4" HINTS ${LZ4_INCLUDE_DIR}/../lib NO_DEFAULT_PATH NO_CMAKE_PATH)
	if(NOT EXISTS ${LZ4_LIBRARY_RELEASE})
		find_library(LZ4_LIBRARY_RELEASE "lz4")
	endif()
	
	find_library(LZ4_LIBRARY_DEBUG "lz4" HINTS ${LZ4_INCLUDE_DIR}/../debug/lib NO_DEFAULT_PATH NO_CMAKE_PATH)
	if(NOT EXISTS ${LZ4_LIBRARY_DEBUG})
		find_library(LZ4_LIBRARY_DEBUG "lz4d")
	endif()
	
	mark_as_advanced(LZ4_LIBRARY_DEBUG LZ4_LIBRARY_RELEASE)
	
	# Set the LZ4_LIBRARY variable based on what was found, use that
	if(LZ4_LIBRARY_DEBUG AND LZ4_LIBRARY_RELEASE)
		set(LZ4_LIBRARY ${LZ4_LIBRARY_RELEASE})
	elseif(LZ4_LIBRARY_DEBUG)
		set(LZ4_LIBRARY ${LZ4_LIBRARY_DEBUG})
	elseif(LZ4_LIBRARY_RELEASE)
		set(LZ4_LIBRARY ${LZ4_LIBRARY_RELEASE})
	endif()
	
	if(LZ4_LIBRARY_RELEASE)
		set_property(TARGET LZ4 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
		set_property(TARGET LZ4 PROPERTY IMPORTED_LOCATION_RELEASE ${LZ4_LIBRARY_RELEASE})
	endif()

	if(LZ4_LIBRARY_DEBUG)
		set_property(TARGET LZ4 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
		set_property(TARGET LZ4 PROPERTY IMPORTED_LOCATION_DEBUG ${LZ4_LIBRARY_DEBUG})
	endif()

	# Include directories
	set_property(TARGET LZ4 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${LZ4_INCLUDE_DIR})
else()
	set(LZ4_LIBRARY LZ4)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZ4 REQUIRED_VARS LZ4_LIBRARY LZ4_INCLUDE_DIR)
