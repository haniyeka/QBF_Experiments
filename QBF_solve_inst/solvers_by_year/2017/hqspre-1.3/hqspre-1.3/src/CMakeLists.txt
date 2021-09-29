# Create the HQSpre library

add_library(hqspre STATIC
              aux.hpp
              bool_vector.hpp
              bool_vector.ipp
              bool_vector.cpp
              clause.hpp
              clause.ipp
              clause.cpp
              exceptions.hpp
              formula.hpp
              formula.ipp
              formula.cpp
              formula_blocked_et_al.cpp
              formula_blocked_et_al.ipp
              formula_debug.cpp
              formula_dep_elim.cpp
              formula_dep_schemes.cpp
              formula_equiv_contra.cpp
              formula_experimental.cpp
              formula_gate_interface.cpp
              formula_gates.cpp
              formula_inout.cpp
              formula_resolution.cpp
              formula_sat.cpp
              formula_substitute.cpp
              formula_subsumption.cpp
              formula_unit_pure.cpp
              formula_univ_expand.cpp
              formula_univ_red.cpp
              gate.hpp
              gate.cpp
              literal.hpp
              prefix.hpp
              prefix.ipp
              prefix.cpp
              process_limits.hpp
              sat_solver.hpp
              settings.hpp
              settings.cpp
              skolem.hpp
              timer.hpp
              timer.cpp
              varheap.hpp
           )
target_link_libraries(hqspre antom)
target_link_libraries(hqspre picosat)
target_compile_options(hqspre PRIVATE -Wno-unused-variable)

# Create the HQSpre binary
add_executable(preprocess main.cpp)
set_target_properties(preprocess PROPERTIES OUTPUT_NAME "hqspre")
target_link_libraries(preprocess hqspre)
target_link_libraries(preprocess antom)
target_link_libraries(preprocess ${Boost_LIBRARIES} rt ${ZLIB_LIBRARIES})
target_link_libraries(preprocess picosat)
target_link_libraries(preprocess easylogging)
target_compile_options(preprocess PRIVATE "-Wno-unused-variable")

# Create the HQSpre binary
add_executable(hqspre_solver main_solver.cpp)
target_link_libraries(hqspre_solver hqspre)
target_link_libraries(hqspre_solver antom)
target_link_libraries(hqspre_solver ${Boost_LIBRARIES} rt ${ZLIB_LIBRARIES})
target_link_libraries(hqspre_solver picosat)
target_link_libraries(hqspre_solver easylogging)
target_compile_options(hqspre_solver PRIVATE "-Wno-unused-variable")

install(TARGETS preprocess hqspre hqspre_solver
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/bin
        ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/bin
       )
