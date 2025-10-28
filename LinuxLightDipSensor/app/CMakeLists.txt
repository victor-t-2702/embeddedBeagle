#LinuxLightDipSensor/app/CMakeLists.txt

# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(LinuxLightDipSensor ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(LinuxLightDipSensor LINK_PRIVATE hal)

# Make use of the GPIO library
target_link_libraries(LinuxLightDipSensor LINK_PRIVATE gpiod)


# Copy executable to final location (change `hello_world` to project name as needed)
#add_custom_command(TARGET LinuxLightDipSensor POST_BUILD 
#  COMMAND "${CMAKE_COMMAND}" -E copy 
#     "$<TARGET_FILE:LinuxLightDipSensor>"
#     "~/embeddedBeagle/public/myApps/LinuxLightDipSensor" 
#  COMMENT "Copying ARM executable to public NFS directory")
