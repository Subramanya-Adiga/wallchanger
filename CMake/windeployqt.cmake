get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
function(windeployqt target)

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(windeploy_lib_type "--release")
  else()
    set(windeploy_lin_type "--debug")
  endif()

  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND
      "${_qt_bin_dir}/windeployqt.exe" --verbose 1 "${windeploy_lib_type}"
      --no-svg --no-opengl --no-opengl-sw --no-compiler-runtime
      --no-system-d3d-compiler \"$<TARGET_FILE:${target}>\"
    COMMENT
      "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
  )

endfunction()
