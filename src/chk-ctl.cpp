#include <algorithm>
#include <set>

#include "chk-ctl.h"
#include "chk-systemd.h"

ChkCTL::ChkCTL() {
  bus = new ChkBus();
  items.clear();
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
  std::vector<UnitInfo> sysUnits = bus->getAllUnits();

  items.clear();

  for (const auto unit : sysUnits) {
    if (unit.id) {
      pushItem(unit);
    }
  }

  sysUnits.clear();
}

void ChkCTL::sortByName(std::vector<UnitItem *> *sortable) {
  std::sort(sortable->begin(), sortable->end(), [](const UnitItem *a, const UnitItem *b) {
    const char* s1 = a->id.c_str();
    const char* s2 = b->id.c_str();
    while(true) {
       if ( std::toupper(*s1) < std::toupper(*s2) ) return true;
       if ( std::toupper(*s1) > std::toupper(*s2) ) return false;
       if ( *s1 == 0 && *s2 == 0 ) return false;
       if ( *s1 > *s2) return false;
       if ( *s1 < *s2) return true;
       ++s1; ++s2;
    }
  });
}

void ChkCTL::pushItem(UnitInfo unit) {
  UnitItem *item = new UnitItem();

  std::string id(strdup(unit.id));

  item->id = id;
  item->target = id.substr(id.find_last_of('.') + 1, id.length());
  item->description = std::string((unit.description == NULL ?
      strdup(unit.unitPath) : strdup(unit.description)));

  if (unit.state != NULL) {
    std::string state(unit.state);
    std::string sub(unit.subState == NULL ? "" : unit.subState);

    if (state.find("enabled") == 0) {
      item->state = UNIT_STATE_ENABLED;
    } else if (state.find("disabled") == 0) {
      item->state = UNIT_STATE_DISABLED;
    } else if (state.find("static") == 0) {
      item->state = UNIT_STATE_STATIC;
    } else if (state.find("bad") == 0) {
      item->state = UNIT_STATE_BAD;
    } else {
      item->state = UNIT_STATE_MASKED;
    }

    if (!sub.empty()) {
      if (sub.find("running") == 0) {
        item->sub = UNIT_SUBSTATE_RUNNING;
      } else {
        item->state = UNIT_SUBSTATE_CONNECTED;
      }
    } else {
        item->sub = UNIT_SUBSTATE_INVALID;
    }

  } else {
    item->state = UNIT_STATE_MASKED;
  }

  free((void *) unit.state);

  items.push_back(item);
};

std::vector<UnitItem *> ChkCTL::getItemsSorted() {
  if (items.empty()) {
    fetch();
  }

  std::set<std::string> targets;
  std::vector<std::string> orderedTargets;
  std::vector<UnitItem *> sunits;

  for (const auto unit : items) {
    if (unit->target.compare("service") != 0 &&
        unit->target.compare("timer") != 0 &&
        unit->target.compare("socket") != 0) {

      targets.insert(unit->target);
    }
  }

  orderedTargets.push_back("service");
  orderedTargets.push_back("timer");
  orderedTargets.push_back("socket");

  for (const auto target : targets) {
    orderedTargets.push_back(target);
  }

  bool isFirst = false;

  for (std::string target : orderedTargets) {
    auto targetedUnits = getByTarget(target.c_str());
    sortByName(&targetedUnits);

    if (isFirst) {
      UnitItem *separator = new UnitItem();
      separator->target = std::string(target);

      sunits.push_back(separator);
    }
    sunits.insert(sunits.end(), targetedUnits.begin(), targetedUnits.end());
    isFirst = true;
  }

  return sunits;
}
