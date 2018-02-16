add_library(
    reader
        "src/reader.cpp"
		"src/palette/palette.cpp"
)

target_include_directories(reader INTERFACE "src/")

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(reader ${Boost_LIBRARIES})	
endif()


if(FREEIMAGE_FOUND)
	include_directories(${FREEIMAGE_INCLUDE_DIRS})
	MESSAGE(STATUS "freeimage library ${FREEIMAGE_LIBRARIES}")	
	target_link_libraries(reader ${FREEIMAGE_LIBRARIES})
endif()

