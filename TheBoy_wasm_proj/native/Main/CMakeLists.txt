set( SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/gdlib.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set ( HEADERS 
	${CMAKE_CURRENT_SOURCE_DIR}/main.h
)
message(${SOURCES})
message("-----")


set ( INC_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/
)

add_subdirectory(Vendor)


message(${SOURCES})
message("-----")
message(${HEADERS})
message("-----")
message(${INC_DIRECTORIES})



if(EMSCRIPTEN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s SIDE_MODULE=1 -s WASM_OBJECT_FILES=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s SIDE_MODULE=1 -s WASM_OBJECT_FILES=1")
	
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC godot-cpp)
include_directories(${INC_DIRECTORIES})
