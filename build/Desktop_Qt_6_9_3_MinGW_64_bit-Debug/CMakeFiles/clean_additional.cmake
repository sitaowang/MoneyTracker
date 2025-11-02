# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MoneyTracker_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MoneyTracker_autogen.dir\\ParseCache.txt"
  "MoneyTracker_autogen"
  )
endif()
