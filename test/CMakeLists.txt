# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(DBMSTests LANGUAGES CXX)

# ---- Dependencies ----

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(DBMS_test source/DBMS_test.cpp)
target_link_libraries(
    DBMS_test PRIVATE
    DBMS_lib
    Catch2::Catch2WithMain
)
target_compile_features(DBMS_test PRIVATE cxx_std_20)

catch_discover_tests(DBMS_test)

# ---- End-of-file commands ----

add_folders(Test)
