#ReactionTimer/app/CMakeLists.txt

set(PROJECT_NAME "BeatBox")

# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(${PROJECT_NAME} LINK_PRIVATE hal)

#ALSA Support
find_package(ALSA REQUIRED)
target_link_libraries(${PROJECT_NAME} LINK_PRIVATE asound)
target_link_libraries(${PROJECT_NAME} PRIVATE hal ${ALSA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE hal asound)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
  COMMAND "$(CMAKE_COMMAND)" -E copy_directory
      "${CMAKE_SOURCE_DIR}/beatbox-wave-files"
     "$(HOME)/ensc351/public/myApps/beatbox-wave-files" 
  COMMENT "Copying WAVE files to public NFS directory")

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:${PROJECT_NAME}>"
     "~/embeddedBeagle/public/myApps/${PROJECT_NAME}" 
  COMMENT "Copying ARM executable to public NFS directory")
