PROJECT (TOOLS)
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_BUILD_TYPE Release)
# set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER ../../3p/llvm/bin/clang++)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic -O4 -Wno-deprecated-register -Wno-keyword-macro -Wno-unused-local-typedef")

ENABLE_TESTING()

set(PROJECTS_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../")

# Include folder
INCLUDE_DIRECTORIES ("../../")
include_directories ("${PROJECTS_FOLDER}/3p/poco/include")
include_directories ("${PROJECTS_FOLDER}/3p/leveldb/include")
include_directories ("${PROJECTS_FOLDER}/3p/rapidjson/include")
include_directories ("${PROJECTS_FOLDER}/3p/gtest/include")
include_directories ("${PROJECTS_FOLDER}/3p/cppformat/include")
include_directories ("${PROJECTS_FOLDER}/3p/cereal/include")
include_directories ("${PROJECTS_FOLDER}/3p/sparsehash/include/")

# Used libraries
set(LIB_GTEST "${PROJECTS_FOLDER}/3p/gtest/lib/libgtest.a")
set(LIB_GTEST_MAIN "${PROJECTS_FOLDER}/3p/gtest/lib/libgtest_main.a")
set(LIB_LEVELDB "${PROJECTS_FOLDER}/3p/leveldb/libleveldb.a")
set(LIB_POCO_FOUNDATION "${PROJECTS_FOLDER}/3p/poco/lib/libPocoFoundation.a")
set(LIB_POCO_UTIL "${PROJECTS_FOLDER}/3p/poco/lib/libPocoUtil.a")
set(LIB_POCO_DATA "${PROJECTS_FOLDER}/3p/poco/lib/libPocoData.a")
set(LIB_POCO_SQLITE "${PROJECTS_FOLDER}/3p/poco/lib/libPocoDataSQLite.a")
set(LIB_POCO_XML "${PROJECTS_FOLDER}/3p/poco/lib/libPocoXML.a")
set(LIB_POCO_NET "${PROJECTS_FOLDER}/3p/poco/lib/libPocoNet.a")
set(LIB_CPPFORMAT "${PROJECTS_FOLDER}/3p/cppformat/lib/libcppformat.a")

# Find Boost libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# This option make sure that we use the local boost version.
set(Boost_NO_SYSTEM_PATHS TRUE) 
if (Boost_NO_SYSTEM_PATHS)
  set(BOOST_ROOT "${PROJECTS_FOLDER}/3p/boost")
  set(BOOST_INCLUDE_DIRS "${BOOST_ROOT}/include")
  set(BOOST_LIBRARY_DIRS "${BOOST_ROOT}/lib")
endif (Boost_NO_SYSTEM_PATHS)

find_package(Boost REQUIRED regex date_time system filesystem thread graph program_options iostreams)
include_directories(${BOOST_INCLUDE_DIRS})

# Display debug info
message(STATUS "${Boost_ROOT}")
message(STATUS "${Boost_INCLUDE_DIRS}")
message(STATUS "${Boost_LIBRARY_DIRS}")

if (Boost_FOUND)
  # Compile all binaries
  set(COMMAND_SRC_FILES gotoServer createPdfFile getGeckFiles getEdgCaseFiles
    regenerateGlobalDatabase runMatlab searchGeck
    geckoSearch openCluster openOutlook codeSearch
    runTests backupSandbox scanTestLog reviewBoard restoreSandbox pcodeFiles)
  foreach (src_file ${COMMAND_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} 
      ${Boost_LIBRARIES}       
      ${LIB_POCO_UTIL}
      ${LIB_POCO_NET}
      ${LIB_POCO_XML}
      ${LIB_POCO_SQLITE}
      ${LIB_POCO_FOUNDATION}
      ${LIB_POCO_DATA}
      ${LIB_POCO_SQLITE} -ldl -lpthread) 
  endforeach (src_file)
  INSTALL_PROGRAMS(//$ENV{HOME}/Public/sbtools FILES ${COMMAND_SRC_FILES})

  set(COMMAND_SRC_FILES findEditedFiles buildEditedFileDatabase deleteFiles checkSubmitFile copyFiles)
  foreach (src_file ${COMMAND_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} 
      ${Boost_LIBRARIES} ${LIB_LEVELDB} ${LIB_CPPFORMAT} -lpthread -lrt)
    ADD_TEST(${src_file} ./${src_file})
  endforeach (src_file)
  INSTALL_PROGRAMS(//$ENV{HOME}/Public/sbtools FILES ${COMMAND_SRC_FILES})

  # Compile all unittests
  # set(UNITTEST_SRC_FILES tUnitTests tTestSuite tTestDatabase tLogFolder tLogParser tRemoveWhiteSpace tBasic tFindEditedFiles 
  #   tSandbox tFileDatabase mai)

  # set(UNITTEST_SRC_FILES tUnitTests tTestSuite tTestDatabase tLogFolder tLogParser tRemoveWhiteSpace tBasic tFindEditedFiles 
  #   tSandbox tFileFinder tFileDatabase main)

  set(UNITTEST_SRC_FILES tUnitTests tSandbox)
  foreach (src_file ${UNITTEST_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} 
      ${Boost_LIBRARIES} 
      ${LIB_GTEST} 
      ${LIB_GTEST_MAIN}       
      ${LIB_POCO_UTIL}
      ${LIB_POCO_NET}
      ${LIB_POCO_XML}
      ${LIB_POCO_SQLITE}
      ${LIB_POCO_FOUNDATION}
      ${LIB_POCO_DATA}
      ${LIB_POCO_SQLITE} -ldl -lpthread)
    ADD_TEST(${src_file} ./${src_file})
  endforeach (src_file)
endif (Boost_FOUND)
