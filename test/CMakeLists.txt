include_directories (${SRC_DIR}/include)
link_directories (${SRC_DIR}/test)

find_library(GMOCK_LIBRARY NAMES gmock
            PATHS   ${FOO_PREFIX}/lib
                    /usr/lib
                    /usr/lib64
                    /usr/local/lib
                    /usr/local/lib64)

add_executable (${PROJECT_NAME}-tests
  tests.cpp
  queue_test.cpp
  looper_test.cpp
)
target_link_libraries (${PROJECT_NAME}-tests ${GMOCK_LIBRARY} pthread)

