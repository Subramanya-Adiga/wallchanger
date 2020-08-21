macro(build_lua LUA_PATH)
  add_library(lua STATIC ${SOURCES})
  add_library(lua::lua_static ALIAS lua)

  target_sources(
    lua
    PRIVATE ${LUA_PATH}/lapi.c
            ${LUA_PATH}/lcode.c
            ${LUA_PATH}/lctype.c
            ${LUA_PATH}/ldebug.c
            ${LUA_PATH}/ldo.c
            ${LUA_PATH}/ldump.c
            ${LUA_PATH}/lfunc.c
            ${LUA_PATH}/lgc.c
            ${LUA_PATH}/llex.c
            ${LUA_PATH}/lmem.c
            ${LUA_PATH}/lobject.c
            ${LUA_PATH}/lopcodes.c
            ${LUA_PATH}/lparser.c
            ${LUA_PATH}/lstate.c
            ${LUA_PATH}/lstring.c
            ${LUA_PATH}/ltable.c
            ${LUA_PATH}/ltm.c
            ${LUA_PATH}/lundump.c
            ${LUA_PATH}/lvm.c
            ${LUA_PATH}/lzio.c
            ${LUA_PATH}/ltests.c
            ${LUA_PATH}/lauxlib.c
            ${LUA_PATH}/lbaselib.c
            ${LUA_PATH}/ldblib.c
            ${LUA_PATH}/liolib.c
            ${LUA_PATH}/lmathlib.c
            ${LUA_PATH}/loslib.c
            ${LUA_PATH}/ltablib.c
            ${LUA_PATH}/lstrlib.c
            ${LUA_PATH}/lutf8lib.c
            ${LUA_PATH}/loadlib.c
            ${LUA_PATH}/lcorolib.c
            ${LUA_PATH}/linit.c)

  target_include_directories(lua INTERFACE "${LUA_PATH}/")
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(
      lua PRIVATE -DLUA_USE_LINUX #[=[-DLUA_USE_READLINE]=])
    target_link_libraries(lua PRIVATE #[=[${READLINE_LIBRARY}]=] dl)
  endif()
  if(NOT MSVC)
    target_compile_options(
      lua
      PRIVATE -Wall
              -O2
              -fmax-errors=5
              -Wextra
              -Wshadow
              -Wsign-compare
              -Wundef
              -Wwrite-strings
              -Wredundant-decls
              -Wdisabled-optimization
              -Wdouble-promotion
              -Wlogical-op
              -Wno-aggressive-loop-optimizations
              -Wdeclaration-after-statement
              -Wmissing-prototypes
              -Wnested-externs
              -Wstrict-prototypes
              -Wc++-compat
              -Wold-style-definition
              -std=c99
              -fno-stack-protector
              -fno-common
              -march=native)
  endif()
  #[=[find_library(READLINE_LIBRARY NAMES readline PATH /usr/lib /usr/local/lib
                                      /opt/local/lib)]=]
endmacro()
