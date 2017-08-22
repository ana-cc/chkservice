#include <iostream>
#include <catch.hpp>

#include "chk-systemd.h"

using namespace std;

TEST_CASE("should create object bus", "[ChkBus]") {
  ChkBus *bus = new ChkBus();

  REQUIRE(bus != NULL);
  REQUIRE(bus->isConnected() == false);

  delete bus;
}

TEST_CASE("should connect to systemd bus", "[ChkBus]") {
  ChkBus *bus = new ChkBus();

  REQUIRE_NOTHROW(bus->connect());
  REQUIRE(bus->isConnected() == true);
  bus->disconnect();
  REQUIRE(bus->isConnected() == false);

  delete bus;
}

TEST_CASE("should get list of units", "[ChkBus]") {
  ChkBus *bus = new ChkBus();
  bool sshServiceFound = false;

  REQUIRE_NOTHROW(bus->connect());

  vector<UnitInfo> units;

  REQUIRE_NOTHROW((units = bus->getUnits()));
  REQUIRE(typeid(units[0]) == typeid(UnitInfo));
  REQUIRE(typeid(units[0].id) == typeid(const char *));
  REQUIRE(string(units[0].id).length() > 0);

  for (vector<UnitInfo>::iterator iter = units.begin(); iter != units.end(); iter++) {
    if (string((*iter).id).find("ssh.service") == 0) {
      sshServiceFound = true;
    }
  }

  REQUIRE(sshServiceFound == true);

  delete bus;
}

#include <unistd.h>

TEST_CASE("should be able disable/enable unit", "[ChkBus]") {
  ChkBus *bus = new ChkBus();

  REQUIRE_NOTHROW(bus->disableUnit("ssh.service"));
  REQUIRE_NOTHROW(bus->enableUnit("ssh.service"));
  REQUIRE_NOTHROW(bus->reloadDaemon());

  delete bus;
}

