file(GLOB WelcomeExample_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Super Alchemist
DECLARE_MAGEPROJECT(WelcomeExample "${CMAKE_CURRENT_SOURCE_DIR}/include" "${WelcomeExample_SRC}")