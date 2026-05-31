cmake_minimum_required(VERSION 3.16)

# 1. Wyszukiwanie pakietu Boost na systemie.
# Wymagamy komponentu 'unit_test_framework' (to jest właśnie Boost.Test)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# 2. Tworzymy program wykonywalny dla testów
add_executable(engine_tests
    testMain.cpp
    testPerft.cpp
)

# 3. Podpinamy rdzeń silnika ORAZ bibliotekę Boost.Test
target_link_libraries(engine_tests 
    PRIVATE 
        engine_core
        Boost::unit_test_framework
)

# 4. Automatyczna integracja Boost.Test z CTest (systemem testowym CMake)
# Dzięki temu każdy pojedynczyBOOST_AUTO_TEST_CASE będzie widoczny w CMake jako osobny test!
include(GoogleTest) # Moduł CMake 'GoogleTest' paradoksalnie obsługuje też Boost.Test przez funkcje odkrywania
# Alternatywnie, prosty wpis rejestrujący całość jako jeden test:
add_test(NAME AllEngineTests COMMAND engine_tests)