Ubpa_DownloadTestZip(
  http://home.ustc.edu.cn/~ustczt/resources/USmallVector/doctest_0.zip
  dectest.zip
  SHA256 F2A3973D6F9F61107F370EFAA624C3275DB849574CFC5CBC88A90983C31096DC
)

Ubpa_AddTarget(
  TEST
  MODE EXE
  LIB
    Ubpa::USmallVector_core
  DEFINE
    _HAS_DEPRECATED_UNCAUGHT_EXCEPTION # C++17
)
