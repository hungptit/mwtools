PROJECT (TOOLS)
CMAKE_MINIMUM_REQUIRED (VERSION 3.4)

set(PROJECTS_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../")
set(SANDBOX_FOLDER "/sandbox/hungdang/")

set(EXTRA_COMPILE_OPTIONS "-Wno-deprecated-register -Wno-keyword-macro")

# set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_BUILD_TYPE Release)
# set(CMAKE_CXX_COMPILER clang++)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Read the git tags to determine the project version
include(GetGitVersion)
get_git_version(GIT_VERSION)

# Tell the user what versions we are using
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" VERSION ${GIT_VERSION})
message("-- Version: ${VERSION}")

# The version of the libraries
set(GENERIC_LIB_VERSION ${VERSION})
string(SUBSTRING ${VERSION} 0 1 GENERIC_LIB_SOVERSION)

include(CheckCXXCompilerFlag)
include(AddCXXCompilerFlag)
include(CXXFeatureCheck)

# set(CMAKE_BUILD_TYPE Debug)

set (CMAKE_BUILD_TYPE Release)
add_cxx_compiler_flag(-O4)

add_cxx_compiler_flag(-std=c++14)
add_cxx_compiler_flag(-Wall)
add_cxx_compiler_flag(-Wextra)
add_cxx_compiler_flag(-Wshadow)

add_cxx_compiler_flag(-Werror RELEASE)
add_cxx_compiler_flag(-Werror RELWITHDEBINFO)
add_cxx_compiler_flag(-Werror MINSIZEREL)

add_cxx_compiler_flag(-Wno-unused-local-typedef)

add_cxx_compiler_flag(-pedantic)
add_cxx_compiler_flag(-pedantic-errors)

add_cxx_compiler_flag(-Wshorten-64-to-32)
# add_cxx_compiler_flag(-Wfloat-equal)
add_cxx_compiler_flag(-Wint-to-void-pointer-cast)
add_cxx_compiler_flag(-fstrict-aliasing)
if (HAVE_CXX_FLAG_FSTRICT_ALIASING)
  add_cxx_compiler_flag(-Wstrict-aliasing)
endif()

add_cxx_compiler_flag(-Wthread-safety)
if (HAVE_WTHREAD_SAFETY)
  add_definitions(-DHAVE_WTHREAD_SAFETY)
  cxx_feature_check(THREAD_SAFETY_ATTRIBUTES)
endif()

# Use header only libraries
add_cxx_compiler_flag(-DFMT_HEADER_ONLY)
add_cxx_compiler_flag(-DCEREAL_RAPIDJSON_HAS_CXX11_RVALUE_REFS)

set(CMAKE_CXX_COMPILER ../../3p/llvm/bin/clang++)

ENABLE_TESTING()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include folder
INCLUDE_DIRECTORIES ("../../")
include_directories ("${PROJECTS_FOLDER}/3p/poco/include")
include_directories ("${PROJECTS_FOLDER}/3p/leveldb/include")
include_directories ("${PROJECTS_FOLDER}/3p/rapidjson/include")
include_directories ("${PROJECTS_FOLDER}/3p/gtest/include")
include_directories ("${PROJECTS_FOLDER}/3p/fmt/include")
include_directories ("${PROJECTS_FOLDER}/3p/cereal/include")
include_directories ("${PROJECTS_FOLDER}/3p/sparsehash/include/")
include_directories ("${PROJECTS_FOLDER}/3p/snappy/include/")

# Used libraries
set(LIB_GTEST "${PROJECTS_FOLDER}/3p/gtest/lib/libgtest.a")
set(LIB_GTEST_MAIN "${PROJECTS_FOLDER}/3p/gtest/lib/libgtest_main.a")
set(LIB_LEVELDB "${PROJECTS_FOLDER}/3p/leveldb/out-static/libleveldb.a")
set(LIB_POCO_FOUNDATION "${PROJECTS_FOLDER}/3p/poco/lib/libPocoFoundation.a")
set(LIB_POCO_UTIL "${PROJECTS_FOLDER}/3p/poco/lib/libPocoUtil.a")
set(LIB_POCO_DATA "${PROJECTS_FOLDER}/3p/poco/lib/libPocoData.a")
set(LIB_POCO_SQLITE "${PROJECTS_FOLDER}/3p/poco/lib/libPocoDataSQLite.a")
set(LIB_POCO_XML "${PROJECTS_FOLDER}/3p/poco/lib/libPocoXML.a")
set(LIB_POCO_NET "${PROJECTS_FOLDER}/3p/poco/lib/libPocoNet.a")
set(LIB_SNAPPY "${PROJECTS_FOLDER}/3p/snappy/lib/libsnappy.a")
set(LIB_FMT "${PROJECTS_FOLDER}/3p/fmt/lib/libfmt.a")

# Libraries used by rocksdb
set(LIB_ROCKSDB "${PROJECTS_FOLDER}/3p/rocksdb/librocksdb.a")
set(LIB_ZLIB "${PROJECTS_FOLDER}/3p/zlib/lib/libz.a")
set(LIB_LZ4 "${PROJECTS_FOLDER}/3p/lz4/lib/liblz4.a")
set(LIB_BZ2 "${PROJECTS_FOLDER}/3p/bzip2/lib/libbz2.a")
set(LIB_SNAPPY "${PROJECTS_FOLDER}/3p/snappy/lib/libsnappy.a")
set(LIB_JEMALLOC "${PROJECTS_FOLDER}/3p/jemalloc/lib/libjemalloc.a")

# set(LIB_TBB "${PROJECTS_FOLDER}/3p/tbb/lib/libtbb.so")
set(LIB_TBB "${SANDBOX_FOLDER}/3p/tbb/lib/libtbb.so")

include_directories ("${PROJECTS_FOLDER}/3p/rocksdb/include")
include_directories ("${PROJECTS_FOLDER}/3p/zlib/include")
include_directories ("${PROJECTS_FOLDER}/3p/lz4/include")
include_directories ("${PROJECTS_FOLDER}/3p/bzip2/include")
include_directories ("${PROJECTS_FOLDER}/3p/jemalloc/include")
include_directories ("${PROJECTS_FOLDER}/3p/tbb/include/")
include_directories ("${PROJECTS_FOLDER}/3p/spdlog/include/")

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
  set(COMMAND_SRC_FILES vncViewer gotoServer createPdfFile getGeckFiles getEdgCaseFiles
    regenerateGlobalDatabase runMatlab searchGeck parseTestLog
    geckoSearch openCluster openOutlook codeSearch mrdesktop
    runTests backupSandbox scanTestLog reviewBoard restoreSandbox pcodeFiles 
    web_browser createSBSSandbox deleteSBSSandbox cloneSandbox syncMasterSandbox)
  foreach (src_file ${COMMAND_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} 
      ${Boost_LIBRARIES}       
      ${LIB_FMT} ${LIB_SNAPPY}
      ${LIB_POCO_UTIL}
      ${LIB_POCO_NET}
      ${LIB_POCO_XML}
      ${LIB_POCO_SQLITE}
      ${LIB_POCO_FOUNDATION}
      ${LIB_POCO_DATA}
      ${LIB_POCO_SQLITE} ${LIB_SNAPPY} ${LIB_TBB} -ldl -lpthread) 
  endforeach (src_file)
  INSTALL_PROGRAMS(//$ENV{HOME}/Public/sbtools FILES ${COMMAND_SRC_FILES})

    set(COMMAND_SRC_FILES mrun_tests mget_test_results mrunlikebat)
  foreach (src_file ${COMMAND_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} 
      ${Boost_LIBRARIES}
      ${LIB_ROCKSDB} ${LIB_LZ4} ${LIB_ZLIB} ${LIB_SNAPPY} ${LIB_JEMALLOC} ${LIB_BZ2}
      ${LIB_FMT}
      ${LIB_POCO_UTIL} ${LIB_POCO_NET} ${LIB_POCO_XML} ${LIB_POCO_SQLITE} ${LIB_POCO_FOUNDATION} ${LIB_POCO_DATA} ${LIB_POCO_SQLITE}
      ${LIB_TBB} -ldl -lpthread) 
  endforeach (src_file)
  INSTALL_PROGRAMS(//$ENV{HOME}/Public/sbtools FILES ${COMMAND_SRC_FILES})

  set(COMMAND_SRC_FILES check_submit_file)
  foreach (src_file ${COMMAND_SRC_FILES})
    ADD_EXECUTABLE(${src_file} ${src_file}.cpp)
    TARGET_LINK_LIBRARIES(${src_file} 
      ${Boost_LIBRARIES} ${LIB_ROCKSDB} ${LIB_LZ4} ${LIB_ZLIB} ${LIB_SNAPPY} ${LIB_JEMALLOC}
      ${LIB_BZ2} ${LIB_FMT} ${LIB_TBB} -lpthread -lrt)
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
      ${LIB_FMT} ${LIB_GTEST} ${LIB_GTEST_MAIN}       
      ${LIB_POCO_UTIL} ${LIB_POCO_NET}
      ${LIB_POCO_XML} ${LIB_POCO_SQLITE}
      ${LIB_POCO_FOUNDATION} ${LIB_POCO_DATA}
      ${LIB_POCO_SQLITE} -ldl -lpthread)
    ADD_TEST(${src_file} ./${src_file})
  endforeach (src_file)
endif (Boost_FOUND)
