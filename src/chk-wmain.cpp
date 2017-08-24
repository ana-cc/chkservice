#include "chk-ui.h"
#include <iostream>
#include <csignal>
#include <cstring>

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

//  box(win, 0, 0);
  getmaxyx(win, winSize->h, winSize->w);
  winSize->h -= 2;

  std::string title;
  title += "chkservice t/";
  title += units.size();

  printInMiddle(win, 0, 2, winSize->w - 4, (char *)title.c_str(), COLOR_PAIR(4), (char *)NULL);

  for (int i = 0; i < (winSize->h - 2); i++) {
    if ((i + start) > (int)units.size() - 1) {
      break;
    }

    if (i == selected) {
      wattron(win, A_REVERSE);
    }

    drawItem(units[start + i], i + screenSize->x);
    wattroff(win, A_REVERSE);
  }

  refresh();
  wrefresh(win);
}

void MainWindow::drawItem(UnitItem *unit, int y) {
  for (int i = 2; i < (winSize->w - 2); i++) {
    mvwprintw(win, y, i, " ");
  }

  if (unit->id.size() == 0) {
    std::string title(unit->target);
    title += "s";

    printInMiddle(win, y, 4, winSize->w - 4, (char *)title.c_str(), COLOR_PAIR(3), (char *)'~');
    return;
  }

  if (unit->id.size() > (unsigned int)(winSize->w - 20)) {
    unit->id.resize(winSize->w - 20);
  }

  if (unit->state != 100) {
    if (unit->state == UNIT_STATE_ENABLED) {
      wattron(win, COLOR_PAIR(2));
      mvwprintw(win, y, 2, "[x]");
      wattroff(win, COLOR_PAIR(2));
    } else if (unit->state == UNIT_STATE_DISABLED) {
      wattron(win, COLOR_PAIR(5));
      mvwprintw(win, y, 2, "[ ]");
      wattroff(win, COLOR_PAIR(5));
    } else if (unit->state == UNIT_STATE_STATIC) {
      wattron(win, COLOR_PAIR(5));
      mvwprintw(win, y, 2, "[s]");
      wattroff(win, COLOR_PAIR(5));
    } else if (unit->state == UNIT_STATE_BAD) {
      wattron(win, COLOR_PAIR(1));
      mvwprintw(win, y, 2, "-b-");
      wattroff(win, COLOR_PAIR(1));
    } else if (unit->state == UNIT_STATE_MASKED) {
      wattron(win, COLOR_PAIR(3));
      mvwprintw(win, y, 2, "-m-");
      wattroff(win, COLOR_PAIR(3));
    }
  } else {
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, y, 2, " -");
    wattroff(win, COLOR_PAIR(5));
  }

  mvwprintw(win, y, 10, "%s", unit->id.c_str());
}
