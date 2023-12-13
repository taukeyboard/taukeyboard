execute_process(
  COMMAND ${WHERE_CMD} ${TOOLCHAIN_PREFIX}gcc
  OUTPUT_VARIABLE TOOLCHAIN_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "TOOLCHAIN_PATH:${TOOLCHAIN_PATH}")

# specify cross compilers and tools
SET(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}ld${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "")
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}objdump CACHE INTERNAL "")
set(SIZE ${TOOLCHAIN_PREFIX}size CACHE INTERNAL "")
