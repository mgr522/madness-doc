# src/madness

# Add source subdirectories
add_subdirectory(external)

if(ENABLE_ELEMENTAL)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elemental/include
      ${CMAKE_CURRENT_BINARY_DIR}/elemental/include)
endif()

add_subdirectory(world)
add_subdirectory(misc)
add_subdirectory(tensor)
add_subdirectory(mra)

# Create the super library that contains all MADNESS libraries
add_library(madness
    $<TARGET_OBJECTS:MADworld-obj> $<TARGET_OBJECTS:MADmisc-obj>
    $<TARGET_OBJECTS:MADtensor-obj> ${MADTENSOR_MTXMQ_OBJ}
    $<TARGET_OBJECTS:MADlinalg-obj> $<TARGET_OBJECTS:MADmuparser-obj>
    $<TARGET_OBJECTS:MADtinyxml-obj> $<TARGET_OBJECTS:MADmra-obj>)

# Set the dependencies for the MADworld library
if(CXX11_COMPILE_FLAG)
  target_compile_options(madness INTERFACE $<INSTALL_INTERFACE:${CXX11_COMPILE_FLAG}>)
endif()
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  target_link_libraries(madness PUBLIC "-Wl,-no_pie")
endif()
if(ENABLE_ELEMENTAL)
  target_link_libraries(madness PUBLIC elemental)
endif()
if(PAPI_FOUND)
  target_include_directories(madness PUBLIC ${PAPI_INCLUDE_DIRS})
  target_link_libraries(madness PUBLIC ${PAPI_LIBRARIES})
endif()
if(TBB_FOUND)
  target_include_directories(madness PUBLIC ${TBB_INCLUDE_DIRS})
  if(TBB_USE_DEBUG AND TBB_LIBRARIES_DEBUG)
    target_compile_definitions(madness PUBLIC "${TBB_COMPILE_FLAGS_DEBUG}")
    target_link_libraries(madness PUBLIC ${TBB_LIBRARIES_DEBUG})
  else()
    target_link_libraries(madness PUBLIC ${TBB_LIBRARIES_RELEASE})
  endif()
endif()
if(MPI_FOUND)
  target_include_directories(madness PUBLIC ${MPI_INCLUDE_PATH})
  target_compile_definitions(madness PUBLIC -DMPICH_SKIP_MPICXX=1 -DOMPI_SKIP_MPICXX=1)
  target_compile_options(madness PUBLIC "${MPI_COMPILE_FLAGS}")
  target_link_libraries(madness PUBLIC ${MPI_LINK_FLAGS})
  target_link_libraries(madness PUBLIC ${MPI_LIBRARIES})
endif()
if(HAVE_IBMBGQ OR HAVE_IBMBGP)
  target_link_libraries(MADtensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/new_mtxmq/bests/libMADMTXM.a)
endif()
target_link_libraries(madness PUBLIC ${LAPACK_LIBRARIES})
if(GPERFTOOLS_FOUND)
  target_include_directories(madness PUBLIC ${GPERFTOOLS_INCLUDE_DIRS})
  target_link_libraries(madness PUBLIC ${GPERFTOOLS_LIBRARIES})
endif()
if(LIBUNWIND_FOUND)
  target_include_directories(madness PUBLIC ${LIBUNWIND_INCLUDE_DIRS})
  target_link_libraries(madness PUBLIC ${LIBUNWIND_LIBRARIES})
endif()
target_link_libraries(madness PUBLIC ${CMAKE_THREAD_LIBS_INIT})

# Add common header files and madness library to the install list
install(TARGETS madness EXPORT madness
    COMPONENT madness
    LIBRARY DESTINATION "${MADNESS_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${MADNESS_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${MADNESS_INSTALL_INCLUDEDIR}")
install(FILES constants.h fortran_ctypes.h madness_config.h ${CMAKE_CURRENT_BINARY_DIR}/config.h
    DESTINATION "${MADNESS_INSTALL_INCLUDEDIR}/madness/"
    COMPONENT common)
add_custom_target(install-common
     COMMAND ${CMAKE_COMMAND} -DCOMPONENT=common -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
     COMMENT "Installing common library components")

