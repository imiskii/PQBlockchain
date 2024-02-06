
# Create executeble for tests
#add_executable(Scitanie sucet.cpp)
#add_test(Test Scitanie 100 100) / add_test(NAME Test COMMAND Scitanie)
#add_tests_properties(Test PROPERTIES PASS_REGULAR_EXPRESSION "200")

# add_test(NAME <název> COMMAND <přı́kaz> [args])
# • Vytvořı́ cı́l s názvem <název> pro testovánı́
# • Pokud je <přı́kaz> názvem spustitelného cı́le je jı́m nahrazen
# • Test lze spustit pomocı́ ctest -R <název>“

# set_tests_properties(<název> PROPERTIES <param>)
# • Umožňuje nastavit parametry testu
# • PASS REGULAR EXPRESSION – výstup musı́ odpovı́dat R. V.
# • TIMEOUT – časový limit na provedenı́ testu
# • COST – specifikuje pořadı́ testů



## 
# Macro for adding new tests
# 
# Examples:
# + package_add_test(Example example.cpp ArgParser "${PROJECT_DIR}")
# + package_add_test(Example example.cpp [] "${PROJECT_DIR}")                   // no libraries
# + package_add_test(Example example1.cpp example2.cpp [] "${PROJECT_DIR}")     // multiple sources
macro(package_add_test TESTNAME FILES LIBRARIES TEST_WORKING_DIRECTORY)
    add_executable(${TESTNAME} ${FILES})
    if(LIBRARIES)
        target_link_libraries(${TESTNAME} gtest gmock gtest_main ${LIBRARIES})
    else()
        target_link_libraries(${TESTNAME} gtest gmock gtest_main)
    endif()
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${TEST_WORKING_DIRECTORY}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()


if(BUILD_TESTING)
    enable_testing()
    include(GoogleTest)
    package_add_test(Example example.cpp [] "${PROJECT_DIR}")

endif()

