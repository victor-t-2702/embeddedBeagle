#LinuxLightDipSensor/hal/CMakeLists.txt

# CMakeList.txt for HAL
#   Build a library (`hal`) which exposes the header files as "hal/*.h"
#   Use header as: #include "hal/button.h"

include_directories(hal/include)
file(GLOB MY_SOURCES "src/*.c")

add_library(hal STATIC ${MY_SOURCES})

target_include_directories(hal PUBLIC include)



# # Glob all C source files
# file(GLOB HAL_SOURCES "src/*.c")

# # Build static library
# add_library(hal STATIC ${HAL_SOURCES})

# # Expose headers for other targets
# target_include_directories(hal PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

