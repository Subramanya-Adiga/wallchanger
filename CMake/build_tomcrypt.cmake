macro(build_tomcrypt TOM_PATH)
  add_library(tomcrypt STATIC ${SOURCES})
  add_library(tomcrypt::tomcrypt_static ALIAS tomcrypt)

  target_sources(
    tomcrypt
    PRIVATE ${TOM_PATH}/src/ciphers/aes/aes_tab.c
            ${TOM_PATH}/src/ciphers/aes/aes.c
            ${TOM_PATH}/src/ciphers/twofish/twofish_tab.c
            ${TOM_PATH}/src/ciphers/twofish/twofish.c
            ${TOM_PATH}/src/ciphers/blowfish.c
            ${TOM_PATH}/src/ciphers/rc6.c
            ${TOM_PATH}/src/ciphers/serpent.c
            ${TOM_PATH}/src/hashes/md5.c
            ${TOM_PATH}/src/hashes/sha1.c
            ${TOM_PATH}/src/hashes/tiger.c
            ${TOM_PATH}/src/hashes/whirl/whirl.c
            ${TOM_PATH}/src/hashes/whirl/whirltab.c
            ${TOM_PATH}/src/hashes/rmd160.c
            ${TOM_PATH}/src/hashes/helper/hash_file.c
            ${TOM_PATH}/src/hashes/helper/hash_filehandle.c
            ${TOM_PATH}/src/hashes/helper/hash_memory.c
            ${TOM_PATH}/src/hashes/helper/hash_memory_multi.c
            ${TOM_PATH}/src/hashes/sha2/sha256.c
            ${TOM_PATH}/src/modes/ctr/ctr_encrypt.c
            ${TOM_PATH}/src/modes/ctr/ctr_decrypt.c
            ${TOM_PATH}/src/modes/ctr/ctr_done.c
            ${TOM_PATH}/src/modes/ctr/ctr_getiv.c
            ${TOM_PATH}/src/modes/ctr/ctr_setiv.c
            ${TOM_PATH}/src/modes/ctr/ctr_start.c
            ${TOM_PATH}/src/modes/ctr/ctr_test.c
            ${TOM_PATH}/src/modes/ofb/ofb_decrypt.c
            ${TOM_PATH}/src/modes/ofb/ofb_done.c
            ${TOM_PATH}/src/modes/ofb/ofb_encrypt.c
            ${TOM_PATH}/src/modes/ofb/ofb_getiv.c
            ${TOM_PATH}/src/modes/ofb/ofb_setiv.c
            ${TOM_PATH}/src/modes/ofb/ofb_start.c
            ${TOM_PATH}/src/prngs/chacha20.c
            ${TOM_PATH}/src/prngs/fortuna.c
            ${TOM_PATH}/src/prngs/rc4.c
            ${TOM_PATH}/src/prngs/yarrow.c
            ${TOM_PATH}/src/prngs/sprng.c
            ${TOM_PATH}/src/prngs/sober128.c
            ${TOM_PATH}/src/prngs/rng_get_bytes.c
            ${TOM_PATH}/src/prngs/rng_make_prng.c
            ${TOM_PATH}/src/mac/hmac/hmac_done.c
            ${TOM_PATH}/src/mac/hmac/hmac_file.c
            ${TOM_PATH}/src/mac/hmac/hmac_init.c
            ${TOM_PATH}/src/mac/hmac/hmac_memory.c
            ${TOM_PATH}/src/mac/hmac/hmac_memory_multi.c
            ${TOM_PATH}/src/mac/hmac/hmac_process.c
            ${TOM_PATH}/src/mac/hmac/hmac_test.c
            ${TOM_PATH}/src/stream/chacha/chacha_crypt.c
            ${TOM_PATH}/src/stream/chacha/chacha_done.c
            ${TOM_PATH}/src/stream/chacha/chacha_ivctr32.c
            ${TOM_PATH}/src/stream/chacha/chacha_ivctr64.c
            ${TOM_PATH}/src/stream/chacha/chacha_keystream.c
            ${TOM_PATH}/src/stream/chacha/chacha_setup.c
            ${TOM_PATH}/src/stream/rc4/rc4_stream.c
            ${TOM_PATH}/src/stream/rc4/rc4_stream_memory.c
            ${TOM_PATH}/src/stream/sober128/sober128_stream.c
            ${TOM_PATH}/src/stream/sober128/sober128tab.c
            ${TOM_PATH}/src/stream/sober128/sober128_stream_memory.c
            ${TOM_PATH}/src/misc/crc32.c
            ${TOM_PATH}/src/misc/adler32.c
            ${TOM_PATH}/src/misc/base64/base64_encode.c
            ${TOM_PATH}/src/misc/base64/base64_decode.c
            ${TOM_PATH}/src/misc/compare_testvector.c
            ${TOM_PATH}/src/misc/error_to_string.c
            ${TOM_PATH}/src/misc/zeromem.c
            ${TOM_PATH}/src/misc/copy_or_zeromem.c
            ${TOM_PATH}/src/misc/crypt/crypt.c
            ${TOM_PATH}/src/misc/crypt/crypt_sizes.c
            ${TOM_PATH}/src/misc/crypt/crypt_argchk.c
            ${TOM_PATH}/src/misc/crypt/crypt_constants.c
            ${TOM_PATH}/src/misc/crypt/crypt_inits.c
            ${TOM_PATH}/src/misc/crypt/crypt_ltc_mp_descriptor.c
            ${TOM_PATH}/src/misc/crypt/crypt_cipher_descriptor.c
            ${TOM_PATH}/src/misc/crypt/crypt_cipher_is_valid.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_cipher.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_cipher_any.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_cipher_id.c
            ${TOM_PATH}/src/misc/crypt/crypt_register_cipher.c
            ${TOM_PATH}/src/misc/crypt/crypt_unregister_cipher.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_hash.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_hash_any.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_hash_id.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_hash_oid.c
            ${TOM_PATH}/src/misc/crypt/crypt_register_hash.c
            ${TOM_PATH}/src/misc/crypt/crypt_unregister_hash.c
            ${TOM_PATH}/src/misc/crypt/crypt_hash_descriptor.c
            ${TOM_PATH}/src/misc/crypt/crypt_hash_is_valid.c
            ${TOM_PATH}/src/misc/crypt/crypt_find_prng.c
            ${TOM_PATH}/src/misc/crypt/crypt_prng_descriptor.c
            ${TOM_PATH}/src/misc/crypt/crypt_prng_is_valid.c
            ${TOM_PATH}/src/misc/crypt/crypt_register_prng.c
            ${TOM_PATH}/src/misc/crypt/crypt_unregister_prng.c)

  target_include_directories(
    tomcrypt PUBLIC $<BUILD_INTERFACE:${TOM_PATH}/src/headers>
                    $<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>)

  target_compile_definitions(
    tomcrypt PRIVATE -DLTC_SOURCE -DGIT_VERSION=\"git-v1.18.2-423-g01c455c3\"
                     -DLTC_NO_ROLC)

  if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(tomcrypt PRIVATE LTC_NO_PROTOTYPES
                                                _CRT_SECURE_NO_WARNINGS)
  endif()

  set_target_properties(
    tomcrypt
    PROPERTIES POSITION_INDEPENDENT_CODE 1
               ARCHIVE_OUTPUT_NAME "tomcrypt"
               DEBUG_POSTFIX "_sd"
               RELEASE_POSTFIX "_s")

  if(NOT MSVC)
    target_compile_options(
      tomcrypt
      PRIVATE -Wall
              -Wsign-compare
              -Wcast-align
              -Wshadow
              -Wextra
              -Wsystem-headers
              -Wbad-function-cast
              -Wcast-align
              -Wstrict-prototypes
              -Wpointer-arith
              -Wdeclaration-after-statement
              -Wwrite-strings
              -Wno-type-limits
              -O3
              -funroll-loops
              -fomit-frame-pointer)
  endif()

  install(
    TARGETS tomcrypt
    EXPORT tomcryptTargets
    ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT lib
    RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
            COMPONENT lib
            COMPONENT dev)

  install(
    EXPORT tomcryptTargets
    NAMESPACE "tomcrypt::"
    DESTINATION ${INSTALL_CMAKEDIR}
    COMPONENT dev)

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/tomcryptConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/CMake/tomcryptConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/tomcryptConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CMAKEDIR})

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tomcryptConfig.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/tomcryptConfigVersion.cmake
          DESTINATION ${INSTALL_CMAKEDIR})
endmacro()
