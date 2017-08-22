#include <algorithm>
#include <set>

#include "chk-ctl.h"
#include "chk-systemd.h"

ChkCTL::ChkCTL() {
  bus = new ChkBus();
}

ChkCTL::~ChkCTL() {
  delete bus;
  items.clear();
}

void ChkCTL::updateItems() {
  items.clear();
  fetch();
}

std::vector<UnitItem *> ChkCTL::getItems() {
  if (items.empty()) {
    fetch();
  }

  sortByName(&items);
  return items;
}

std::vector<UnitItem *> ChkCTL::getByTarget(const char *target) {
  if (items.empty()) {
    fetch();
  }

  std::vector<UnitItem *> found;
  std::string pattern = target == NULL ? "" : target;

  for (UnitItem *item : items) {
    if (item->target.find(pattern) == 0) {
      found.push_back(item);
    }
  }

  sortByName(&found);
  return found;
}

void ChkCTL::fetch() {
  std::vector<UnitInfo> sysUnits = bus->getUnits();

  for (const auto unit : sysUnits) {
    pushItem(unit);
  }

  sortByName(&items);
}

void ChkCTL::sortByName(std::vector<UnitItem *> *sortable) {
  std::sort(sortable->begin(), sortable->end(), [](const UnitItem *a, const UnitItem *b) {
    return std::tolower(a->name.c_str()[0]) < std::tolower(b->name.c_str()[0]);
  });
}

void ChkCTL::pushItem(UnitInfo unit) {
  UnitItem *item = new UnitItem;

  std::string id = unit.id;

  item->name = id.substr(0, id.find_last_of('.'));
  item->target = id.substr(id.find_last_of('.') + 1, id.length());
  item->sysUnit = unit;

  items.push_back(item);
};

std::vector<UnitItem *> ChkCTL::getItemsSorted() {
  std::set<std::string> targets;
  std::vector<UnitItem *> units;

  if (items.empty()) {
    fetch();
  }

  for (const auto unit : items) {
    targets.insert((*unit).target);
  }

  for (std::string target : targets) {
    auto targetedUnits = getByTarget(target.c_str());
    units.insert(units.end(), targetedUnits.begin(), targetedUnits.end());
  }

  return units;
}
