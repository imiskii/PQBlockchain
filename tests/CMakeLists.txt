
enable_testing()

# Create executeble for tests
#add_executable(Scitanie sucet.cpp)
#add_test(Test Scitanie 100 100)
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