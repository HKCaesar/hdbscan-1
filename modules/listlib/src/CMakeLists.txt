project (listlib)

file(GLOB_RECURSE LISTLIB_SRC_FILES ${LISTLIB_SRC_DIR}/*.c)
add_library(listlib ${LISTLIB_SRC_FILES})
include_directories(${GLIB2_INCLUDE_DIRS})
