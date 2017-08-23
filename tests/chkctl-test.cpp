#include <iostream>
#include <catch.hpp>
#include "chk-ctl.h"

using namespace std;

TEST_CASE("should create object ctl", "[ChkCTL]") {
  ChkCTL *ctl = new ChkCTL();
  REQUIRE(ctl != NULL);
  delete ctl;
}

TEST_CASE("should return sorted items", "[ChkCTL]") {
  ChkCTL *ctl = new ChkCTL();

  auto units = ctl->getItems();
  REQUIRE(tolower(units[0]->name[0]) < tolower(units[units.size() - 1]->name[0]));

  units = ctl->getByTarget("service");
  REQUIRE(tolower(units[0]->name[0]) < tolower(units[units.size() - 1]->name[0]));

  delete ctl;
}

TEST_CASE("should get system units against saved items ", "[ChkCTL]") {
  ChkCTL *ctl = new ChkCTL();

  auto units = ctl->getItems();
  auto items = ctl->getItemsSorted();

//  REQUIRE(units.size() == items.size());

  delete ctl;
}

TEST_CASE("should fetch units and prepare", "[ChkCTL]") {
  ChkCTL *ctl = new ChkCTL();
  bool filtered = true;

  for (auto unit : ctl->getByTarget("service")) {
    if (unit->target.find("service") != 0) {
      filtered = false;
    }
  }

  for (auto unit : ctl->getByTarget("device")) {
    if (unit->target.find("device") != 0) {
      filtered = false;
    }
  }

  REQUIRE(filtered == true);

  delete ctl;
}

TEST_CASE("should fetch items sorted by target", "[ChkCTL]") {
  ChkCTL *ctl = new ChkCTL();

  vector<UnitItem *> units = ctl->getItemsSorted();
  string lastTarget;
  int targetChanged = 0;

  std::set<std::string> targets;

  for (const auto unit : units) {
    targets.insert((*unit).target);
  }

  for (auto unit : units) {
    if (unit->target.compare(lastTarget) != 0) {
      lastTarget = unit->target;
      targetChanged++;
    }
  }

  REQUIRE(targetChanged == targets.size());

  delete ctl;
}
