set (PROGRAM_NAME LIM2Stink)

set (SOURCES
    src/main.cpp
    src/LIM2Stink.cpp
    inc/LIM2Stink.h
    inc/visitors/Singleton.h
    src/visitors/Singleton.cpp
)

function (add_language_config LANG)
  add_executable(${PROGRAM_NAME}_${LANG} ${SOURCES})
  add_dependencies(${PROGRAM_NAME}_${LANG} ${COLUMBUS_GLOBAL_DEPENDENCY})
  target_link_libraries(${PROGRAM_NAME}_${LANG} limmetrics_${LANG} lim2graph  lim graph graphsupport rul strtable common csi io ${COMMON_EXTERNAL_LIBRARIES})
  set_schema_language_compiler_settings(${PROGRAM_NAME}_${LANG} ${LANG})
  add_copy_next_to_the_binary_dependency(${PROGRAM_NAME}_${LANG} Stink.rul)
  set_visual_studio_project_folder(${PROGRAM_NAME}_${LANG} FALSE)
endfunction()

add_language_config(java)
