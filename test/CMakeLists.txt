add_executable(
  runUnitTests 
    "src/tab.cpp"
    "src/palette.cpp")
target_link_libraries(runUnitTests gtest gmock_main reader)