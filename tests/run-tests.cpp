#define CATCH_CONFIG_MAIN

#include <catch.hpp>
#include <cccpp.h>

TEST_CASE("Should add 2 + 2", "Add" ) {
  REQUIRE(add(2, 2) == 4);
}

TEST_CASE("Should add 3 + 2", "Add" ) {
  REQUIRE(add(3, 2) == 5);
}

