cmake_minimum_required(VERSION 3.12...3.31)

project( Triangulation_on_hyperbolic_surface_2_Examples )

# CGAL and its components
find_package( CGAL REQUIRED  )

# create a target per cppfile
file(
  GLOB cppfiles
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(cppfile ${cppfiles})
  create_single_source_cgal_program("${cppfile}")
endforeach()
