
# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB API_HEADER_LIST CONFIGURE_DEPENDS ../include/iopt/*.hpp)
file(GLOB HEADER_LIST CONFIGURE_DEPENDS ./*.hpp)
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS ./*.cpp)
#set(HEADER_LIST "${ModernCMakeExample_SOURCE_DIR}/include/modern/lib.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(iOpt ${SOURCE_LIST} ${HEADER_LIST} ${API_HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(iOpt PUBLIC ../include)

# This depends on (header only) boost
#target_link_libraries(modern_library PRIVATE Boost::boost)

# All users of this library will need at least C++11
target_compile_features(iOpt PUBLIC cxx_std_11)
set_target_properties(iOpt PROPERTIES CXX_EXTENSIONS OFF)

# IDEs should put the headers in a nice place
source_group("Header Files\\api" FILES ${API_HEADER_LIST})

target_link_libraries(iOpt PRIVATE jpeg_turbo)