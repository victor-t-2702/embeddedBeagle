#ReactionTimer/app/CMakeLists.txt

# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(ReactionTimer ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(ReactionTimer LINK_PRIVATE hal)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET ReactionTimer POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:ReactionTimer>"
     "~/embeddedBeagle/public/myApps/ReactionTimer" 
  COMMENT "Copying ARM executable to public NFS directory")
