#define CATCH_CONFIG_MAIN

#include <iostream>
#include <catch.hpp>

TEST_CASE( "should initialize tests", "[Catch]" ) {
  REQUIRE( 1 == 1 );
}

