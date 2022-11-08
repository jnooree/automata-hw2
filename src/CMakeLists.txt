set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

function(add_project_executable name)
  add_executable(${name} "main_${name}.cpp")
  install(TARGETS ${name} RUNTIME)
endfunction()

add_project_executable(run)
