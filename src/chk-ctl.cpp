#include <algorithm>

#include "chk-ctl.h"
#include "chk-systemd.h"

ChkCTL::ChkCTL() {
  bus = new ChkBus();
}

ChkCTL::~ChkCTL() {
  delete bus;
  items.clear();
}

std::vector<UnitItem *> ChkCTL::getItems() {
  if (items.size() < 1) {
    fetch();
  }
  sortByName(&items);
  return items;
}

std::vector<UnitItem *> ChkCTL::getByTarget(const char *target) {
  if (items.size() < 1) {
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
