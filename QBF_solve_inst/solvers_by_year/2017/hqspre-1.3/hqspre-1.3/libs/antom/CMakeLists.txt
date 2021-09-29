# Set the include paths
include_directories("$(PROJECT_SOURCE_DIR)/Core")
include_directories("$(PROJECT_SOURCE_DIR)/prepro")
include_directories("$(PROJECT_SOURCE_DIR)/solver")

# Create the antom library
add_library(antom
  Core/allocator.h
  Core/clause.h
  Core/control.h
  Core/core.h
  Core/helper.h
  Core/reason.h
  Core/solverstate.cpp
  Core/statistics.h
  Core/varheap.h
  Core/watcher.h
  prepro/bce.h
  prepro/bce.cpp
  prepro/bva.h
  prepro/bva.cpp
  prepro/hte.h
  prepro/hte.cpp
  prepro/modelrebuilder.h
  prepro/modelrebuilder.cpp
  prepro/preprocessor.h
  prepro/preprocessor.cpp
  prepro/upla.h
  prepro/upla.cpp
  prepro/varcandidate.h
  solver/antombase.h
  solver/antombase.cpp
  solver/antom.h
  solver/antom.cpp
  solver/debug.cpp
  solver/parser.h
  solver/parser.cpp
  solver/softclause.h
  solver/solverproxy.h
  solver/solverproxy.cpp
  solver/sorter.h
  solver/sorter.cpp
  )

# Create the Antom binary
#add_executable(antom solver/main.cpp)
#target_link_libraries(antom libantom)
