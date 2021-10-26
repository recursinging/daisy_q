cmake_policy(SET CMP0048 NEW)

project(daisy_q VERSION 0.0.1)

cmake_minimum_required(VERSION 3.20)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET daisy_q)

set(LIBDAISY_DIR lib/libDaisy)
set(Q_DIR lib/q)

# DaisySP is a CMake project as well, and can be included here...
# set(DAISYSP_DIR lib/DaisySP)


option(Q_BUILD_EXAMPLES "" OFF)
option(Q_BUILD_TEST "" OFF)
option(Q_BUILD_IO "" OFF)

# open-ocd -  might need to be adapted for your environment...
set(CHIPSET stm32h7x)
set(OCD openocd)
set(OCD_DIR /usr/local/share/openocd/scripts)
set(PGM_DEVICE interface/stlink.cfg)
set(OCDFLAGS -f ${PGM_DEVICE} -f target/${CHIPSET}.cfg)
set(OCD_PROGRAM ${OCD} -s ${OCD_DIR} ${OCDFLAGS} -c "program ${TARGET}.elf verify reset exit")

add_subdirectory(${LIBDAISY_DIR})
add_subdirectory(${Q_DIR})
#add_subdirectory(${DAISYSP_DIR})

include_directories(src)

add_executable(${TARGET} src/main.cpp)

target_link_libraries(${TARGET} PUBLIC 
    libq 
    daisy 
#   DaisySP 
)

get_filename_component(LIBDAISY_DIR_ABS "${LIBDAISY_DIR}"
REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")

set(LINKER_SCRIPT ${LIBDAISY_DIR_ABS}/core/STM32H750IB_flash.lds)

set_target_properties(${TARGET} PROPERTIES
CXX_STANDARD 17
CXX_STANDARD_REQUIRED YES
SUFFIX ".elf"
LINK_DEPENDS ${LINKER_SCRIPT}
)

target_link_options(${TARGET}  PUBLIC
-T ${LINKER_SCRIPT}
-Wl,-Map=${TARGET}.map,--cref
-Wl,--check-sections
-Wl,--unresolved-symbols=report-all
-Wl,--warn-common
-Wl,--warn-section-align
-Wl,--print-memory-usage
)

target_compile_options(${TARGET}
  PUBLIC
    -Os
    -Wall
    -Wno-attributes
    -Wno-strict-aliasing
    -Wno-maybe-uninitialized
    -Wno-missing-attributes
    -Wno-stringop-overflow
    -Wno-error=reorder
    -Wno-error=sign-compare
    -fexceptions
    -DQ_DONT_USE_THREADS=1
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>
)


add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
    ARGS -O ihex
    -S ${TARGET}.elf
    ${TARGET}.hex
    BYPRODUCTS
    ${TARGET}.hex
    COMMENT "Generating HEX image"
VERBATIM)

add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
    ARGS -O binary
    -S ${TARGET}.elf
    ${TARGET}.bin
    BYPRODUCTS
    ${TARGET}.bin
    COMMENT "Generating binary image"
VERBATIM)

add_custom_target(program
  COMMAND ${OCD_PROGRAM}
)