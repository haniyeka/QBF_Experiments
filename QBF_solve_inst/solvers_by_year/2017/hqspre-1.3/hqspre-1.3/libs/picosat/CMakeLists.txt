# Create the PicoSat SAT solver library
add_library(picosat STATIC picosat.c version.c)
target_compile_definitions(picosat PUBLIC "-DTRACE")

install(TARGETS picosat
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/bin
        ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/bin
       )
