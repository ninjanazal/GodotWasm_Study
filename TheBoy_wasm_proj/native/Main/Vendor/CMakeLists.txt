set(VENDOR_SOURCE)
set(VENDOR_HEADERS)
set(VENDOR_INC_DIRECTORIES)

add_subdirectory(TheBoy)


set( SOURCES
	${SOURCES}
	${VENDOR_SOURCE}
    
    PARENT_SCOPE
)

set ( HEADERS 
	${HEADERS}
    ${VENDOR_HEADERS}
    
    PARENT_SCOPE
)

set ( INC_DIRECTORIES
    ${INC_DIRECTORIES}
    ${VENDOR_INC_DIRECTORIES}
    
    PARENT_SCOPE
)

