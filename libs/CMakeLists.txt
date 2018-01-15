option(ENABLE_SHARED "" FALSE)
option(WITH_JAVA "" FALSE)

add_subdirectory(libjpeg-turbo)

add_library(jpeg_turbo INTERFACE)

set(jpeg_turbo_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo)

target_include_directories(jpeg_turbo INTERFACE ${jpeg_turbo_INCLUDE_DIR})
target_link_libraries(jpeg_turbo INTERFACE turbojpeg-static legacy_stdio_definitions.lib)

set_target_properties(dist 
					  installer
					  cjpeg-static
					  djpeg-static
					  jpeg-static 
					  jpegtran-static 
					  jsimdcfg 
					  md5cmp 
					  rdjpgcom 
					  simd 
					  simd-objs 
					  tarball 
					  testclean 
					  tjbench-static 
					  tjtest 
					  tjunittest-static 
					  turbojpeg-static 
					  uninstall 
					  wrjpgcom 
					  PROPERTIES FOLDER turbojpeg)
