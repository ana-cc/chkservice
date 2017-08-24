#include "chk-ui.h"
#include <iostream>
#include <csignal>
#include <cstring>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow() {
  resize();
  std::signal(SIGWINCH, MainWindow::sigwinch);
}

MainWindow::~MainWindow() {

}

void MainWindow::sigwinch(int c) {
  stopCurses();
  exit(0);
}

void MainWindow::createMenu() {
  win = newwin(screenSize->h, screenSize->w, 0, 0);
  keypad(win, true);
  keypad(stdscr, true);

  while(1) {
    drawUnits();

    int key = wgetch(win);

    switch(key) {
      case 'k':
      case KEY_UP:
        moveUp();
        if (units[start + selected]->id.size() == 0)
          moveUp();
        break;
      case 'j':
      case KEY_DOWN:
        moveDown();
        if (units[start + selected]->id.size() == 0)
          moveDown();
        break;
      case 'f':
      case KEY_NPAGE:
        movePageDown();
        if (units[start + selected]->id.size() == 0)
          moveUp();
        break;
      case 'b':
      case KEY_PPAGE:
        movePageUp();
        if (units[start + selected]->id.size() == 0)
          moveDown();
        break;
      case 'q':
        stopCurses();
        exit(0);
        break;
      default:
        break;
    }
  }
}

void MainWindow::resize() {
  screenSize->x = 2;
  screenSize->y = 0;
  getmaxyx(stdscr, screenSize->h, screenSize->w);
}

void MainWindow::moveUp() {
  int ps = winSize->h - 3;

  if (start > 0 && selected < ps / 2) {
    start--;
  } else if (selected > 0) {
    selected--;
  }
}

void MainWindow::moveDown() {
  int offset = start + selected;
  int ps = winSize->h - 3;
  int max = units.size() - 1;

  if ((start + ps) < max) {
    if (selected < ps / 2) {
      selected++;
    } else {
      start++;
    }
  } else if (offset < max) {
    selected++;
  }

  if (offset >= max) {
    selected = ps;
  }
}

void MainWindow::movePageUp() {
  int ps = winSize->h - 3;

  if (start > 0) {
    start -= ps;
  }

  if (start < 0) {
    start = 0;
    selected = 0;
  }
}

void MainWindow::movePageDown() {
  int ps = winSize->h - 3;
  int max = units.size() - 1;

  if ((start + ps / 2) < max) {
    start += ps;
  }

  if ((start + ps) > max) {
    start = max - ps;
    selected = ps;
  }
}

void MainWindow::drawUnits() {
  if (units.empty()) {
    units = ctl->getItemsSorted();
  }

  getmaxyx(win, winSize->h, winSize->w);
  winSize->h -= 2;

  for (int i = 0; i < (winSize->h - 2); i++) {
    if ((i + start) > (int)units.size() - 1) {
      break;
    }

    UnitItem *unit = units[start + i];

    if (i == selected) {
      drawInfo(unit);
      wattron(win, A_REVERSE);
    }

    drawItem(unit, i + screenSize->x);
    wattroff(win, A_REVERSE);
  }

  refresh();
  wrefresh(win);
}

void MainWindow::drawItem(UnitItem *unit, int y) {
  if (unit->id.size() == 0) {
    std::string title(unit->target);
    title += "s";

    printInMiddle(win, y, 2, winSize->w - 2, (char *)title.c_str(), COLOR_PAIR(3), (char *)'~');
    return;
  }

  if (unit->state == UNIT_STATE_ENABLED) {
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, y, 1, " [x] ");
    wattroff(win, COLOR_PAIR(2));
  } else if (unit->state == UNIT_STATE_DISABLED) {
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, y, 1, " [ ] ");
    wattroff(win, COLOR_PAIR(5));
  } else if (unit->state == UNIT_STATE_STATIC) {
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, y, 1, " [s] ");
    wattroff(win, COLOR_PAIR(5));
  } else if (unit->state == UNIT_STATE_BAD) {
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, y, 1, " -b- ");
    wattroff(win, COLOR_PAIR(1));
  } else if (unit->state == UNIT_STATE_MASKED) {
    wattron(win, COLOR_PAIR(3));
    mvwprintw(win, y, 1, " -m- ");
    wattroff(win, COLOR_PAIR(3));
  }

  if (unit->sub == UNIT_SUBSTATE_RUNNING) {
    wattron(win, COLOR_PAIR(3));
    mvwprintw(win, y, 6, " >  ");
    wattroff(win, COLOR_PAIR(3));
  } else if (unit->state == UNIT_SUBSTATE_CONNECTED) {
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, y, 6, " =  ");
    wattroff(win, COLOR_PAIR(5));
  } else {
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, y, 6, "    ");
    wattroff(win, COLOR_PAIR(5));
  }

  if (unit->id.size() > ((unsigned int)winSize->w - 12)) {
    unit->id.resize(winSize->w - 12);
  }

  std::stringstream sline;

  unit->description.resize(winSize->w / 2, ' ');
  sline << std::string(unit->id.size(), ' ') << " " << std::setw(winSize->w - 12 - unit->id.size())
    << unit->description;

  std::string cline(sline.str());
  std::string name(unit->id);

  name.resize(cline.find_first_of(unit->description[0]), ' ');

  if (cline.size() > ((unsigned int)winSize->w - 12)) {
    cline.resize(winSize->w - 12);
  }

  wattron(win, COLOR_PAIR(5));
  mvwprintw(win, y, 10, "%s", cline.c_str());
  wattroff(win, COLOR_PAIR(5));
  mvwprintw(win, y, 10, "%s", name.c_str());
}

void MainWindow::drawInfo(UnitItem *unit) {
//  mvwprintw(win, 0, 4, (char *)desc.c_str());
//  printInMiddle(win, 0, 0, winSize->w, (char *)desc.c_str(), COLOR_PAIR(2), NULL);
}
