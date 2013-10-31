include_directories (${SRC_DIR}/include)
link_directories (${SRC_DIR}/src)

add_library (${PROJECT_NAME}
  looper.cpp
)
target_link_libraries (${PROJECT_NAME})

