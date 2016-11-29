file(GLOB SuperAlchemist_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Super Alchemist
DECLARE_MAGEPROJECT(SuperAlchemist "${CMAKE_CURRENT_SOURCE_DIR}/include" "${SuperAlchemist_SRC}")