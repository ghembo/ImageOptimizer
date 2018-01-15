
# Testing library
FetchContent_Declare(
catch2
GIT_REPOSITORY https://github.com/catchorg/Catch2.git
GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(catch2)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(iOptTest i_opt_test.cpp)

# I'm using C++17 in the test
target_compile_features(iOptTest PRIVATE cxx_std_17)
set_target_properties(iOptTest PROPERTIES CXX_EXTENSIONS OFF)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(iOptTest PRIVATE iOpt Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME iOptTest COMMAND iOptTest) # Command can be a target

if( ${CMAKE_BUILD_TYPE} MATCHES  "Debug" )
	set_target_properties(iOptTest PROPERTIES LINK_FLAGS /NODEFAULTLIB:LIBCMTD.LIB)
else()
	set_target_properties(iOptTest PROPERTIES LINK_FLAGS /NODEFAULTLIB:LIBCMT.LIB)
endif()