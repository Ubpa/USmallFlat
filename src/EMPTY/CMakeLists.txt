Ubpa_AddTarget(
  TEST
  MODE EXE
  SOURCE
    "${PROJECT_SOURCE_DIR}/include/small_vector.h"
    "${PROJECT_SOURCE_DIR}/include/fixed_vector.h"
  LIB
    Ubpa::USmallVector_core
)
