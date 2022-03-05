Ubpa_DownloadTestZip(
  https://udata-1308066321.cos.ap-guangzhou.myqcloud.com/doctest_2_4_8.zip
  doctest_2_4_8.zip
  SHA256 ffca1494c00da645fbf0e86cff89b7222828ffd1927aca3a484aa962139be255
)

Ubpa_AddTarget(
  TEST
  MODE EXE
  LIB
    Ubpa::USmallFlat_core
  DEFINE
    _HAS_DEPRECATED_UNCAUGHT_EXCEPTION # C++17
)
