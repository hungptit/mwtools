PROJECT (TOOLS)
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_BUILD_TYPE Release)
# set(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Wall -Wextra -pedantic -O3")

ENABLE_TESTING()

# Include folder
INCLUDE_DIRECTORIES ("../../")
INCLUDE_DIRECTORIES ("../../3p/gtest/include")
INCLUDE_DIRECTORIES ("../../3p/poco/include/")

# Used libraries
set(PROJECT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../")
set(LIB_GTEST "${PROJECT_FOLDER}/3p/gtest/lib/libgtest.a")
set(LIB_GTEST_MAIN "${PROJECT_FOLDER}/3p/gtest/lib/libgtest_main.a")
set(LIB_POCO "${PROJECT_FOLDER}/3p/gtest/lib/libgtest_main.a")
set(LIB_POCO_FOUNDATION  "${PROJECT_FOLDER}/3p/poco/lib/libPocoFoundation.a")
set(LIB_POCO_UTIL "${PROJECT_FOLDER}/3p/poco/lib/libPocoUtil.a")
set(LIB_POCO_XML  "${PROJECT_FOLDER}/3p/poco/lib/libPocoXML.a")
set(LIB_POCO_SQLITE  "${PROJECT_FOLDER}/3p/poco/lib/libPocoDataSQLited.so")

# Find Boost libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# This option make sure that we use the local boost version.
set(Boost_NO_SYSTEM_PATHS TRUE) 
if (Boost_NO_SYSTEM_PATHS)
  set(BOOST_ROOT "${PROJECT_FOLDER}/3p/boost")
  set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
  set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
endif (Boost_NO_SYSTEM_PATHS)

find_package(Boost REQUIRED regex date_time system filesystem thread graph program_options)
include_directories(${BOOST_INCLUDE_DIRS})

# Display debug info
message(STATUS "${Boost_ROOT}")
message(STATUS "${Boost_INCLUDE_DIRS}")
message(STATUS "${Boost_LIBRARY_DIRS}")

set(COMMAND_SRC_FILES gotoServer createPdfFile getGeckFiles getEdgCaseFiles
  regenerateGlobalDatabase cleanupTmpFolder runMatlab checkSubmitFile searchGeck
  geckoSearch openCluster openOutlook codeSearch
  runTests backupSandbox scanTestLog reviewBoard restoreSandbox pcodeFiles)

# set(COMMAND_SRC_FILES gotoServer createPdfFile getGeckFiles getEdgCaseFiles
#   runTests backupSandbox scanTestLog reviewBoard restoreSandbox pcodeFiles
#   regenerateGlobalDatabase cleanupTmpFolder runMatlab checkSubmitFile searchGeck
#   geckoSearch openCluster openOutlook codeSearch)

message(STATUS "${Boost_LIBRARIES}")

if (Boost_FOUND)
  # Compile all binaries
  foreach (src_file ${COMMAND_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} ${Boost_LIBRARIES})
  endforeach (src_file)
  INSTALL_PROGRAMS(//$ENV{HOME}/Public/sbtools FILES ${COMMAND_SRC_FILES})

  # Compile all unittests
  # set(UNITTEST_SRC_FILES tUnitTests tTestSuite tTestDatabase tLogFolder tLogParser tRemoveWhiteSpace tBasic tFindEditedFiles 
  #   tSandbox tFileFinder tFileDatabase main)

  # set(UNITTEST_SRC_FILES tUnitTests tTestDatabase tLogFolder tLogParser tRemoveWhiteSpace tSandbox tFileFinder)
  # foreach (src_file ${UNITTEST_SRC_FILES})
  #   ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
  #   TARGET_LINK_LIBRARIES(${src_file} 
  #     ${LIB_BOOST_PROGRAM_OPTIONS} ${LIB_BOOST_REGEX} ${LIB_BOOST_SYSTEM} ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_DATETIME} 
  #     ${LIB_GTEST} ${LIB_GTEST_MAIN} ${LIB_POCO_FOUNDATION} ${LIB_POCO_UTIL} ${LIB_POCO_SQLITE} ${LIB_POCO_XML} -lpthread)
  #   ADD_TEST(${src_file} ./${src_file})
  # endforeach (src_file)
endif (Boost_FOUND)
