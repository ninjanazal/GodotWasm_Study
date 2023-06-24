set(SOURCE_THEBOY)
set(HEADER_THEBOY)

set ( VENDOR_INC_DIRECTORIES
    ${VENDOR_INC_DIRECTORIES}
	${CMAKE_CURRENT_SOURCE_DIR}/Common/
	${CMAKE_CURRENT_SOURCE_DIR}/Components/
	${CMAKE_CURRENT_SOURCE_DIR}/EmulatorController/

    PARENT_SCOPE
)


# - - - - - - - - - -
# Add sub folders
# - - - - - - - - - -
add_subdirectory(Common)
add_subdirectory(Components)
add_subdirectory(EmulatorController)


set( VENDOR_SOURCE
    ${VENDOR_SOURCE}
    ${SOURCE_THEBOY}

    PARENT_SCOPE
)

set( VENDOR_HEADERS
    ${VENDOR_HEADERS}
    ${HEADER_THEBOY}

    PARENT_SCOPE
)