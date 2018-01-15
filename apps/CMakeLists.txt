file(GLOB HEADER_LIST CONFIGURE_DEPENDS console/*.hpp)
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS console/*.cpp)

add_executable(console ${SOURCE_LIST} ${HEADER_LIST})

target_compile_features(console PRIVATE cxx_std_17)
set_target_properties(console PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(console PRIVATE iOpt)

if( ${CMAKE_BUILD_TYPE} MATCHES  "Debug" )
	set_target_properties(console PROPERTIES LINK_FLAGS /NODEFAULTLIB:LIBCMTD.LIB)
else()
	set_target_properties(console PROPERTIES LINK_FLAGS /NODEFAULTLIB:LIBCMT.LIB)
endif()