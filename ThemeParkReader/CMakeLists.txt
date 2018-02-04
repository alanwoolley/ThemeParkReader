add_executable(
    ThemeParkReader 
        "src/ThemeParkReader.cpp"
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(ThemeParkReader ${Boost_LIBRARIES})	
endif()

if(FreeImage_FOUND)
	include_directories(${FREEIMAGE_INCLUDE_DIRS})
	MESSAGE(STATUS "freeimage library ${FREEIMAGE_LIBRARIES}")
	target_link_libraries(ThemeParkReader ${FREEIMAGE_LIBRARIES})
endif()
