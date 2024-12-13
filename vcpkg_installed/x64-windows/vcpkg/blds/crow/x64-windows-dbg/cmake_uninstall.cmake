if(NOT EXISTS "C:/Users/marci/Desktop/PO/PO/server/vcpkg_installed/x64-windows/vcpkg/blds/crow/x64-windows-dbg/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: C:/Users/marci/Desktop/PO/PO/server/vcpkg_installed/x64-windows/vcpkg/blds/crow/x64-windows-dbg/install_manifest.txt")
endif()

file(READ "C:/Users/marci/Desktop/PO/PO/server/vcpkg_installed/x64-windows/vcpkg/blds/crow/x64-windows-dbg/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "C:/Users/marci/AppData/Local/vcpkg/downloads/tools/cmake-3.29.2-windows/cmake-3.29.2-windows-i386/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif()
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach()

