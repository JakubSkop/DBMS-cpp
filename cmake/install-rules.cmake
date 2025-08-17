install(
    TARGETS DBMS_exe
    RUNTIME COMPONENT DBMS_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
