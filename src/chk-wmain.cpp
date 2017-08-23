#include "chk-ui.h"
#include <iostream>
#include <csignal>
#include <unistd.h>

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
        break;
      case 'j':
      case KEY_DOWN:
        moveDown();
        break;
      case 'f':
      case KEY_NPAGE:
        movePageDown();
        break;
      case 'b':
      case KEY_PPAGE:
        movePageUp();
        break;
      default:
        break;
    }
  }
}

void MainWindow::resize() {
  screenSize->x = 0;
  screenSize->y = 0;
  getmaxyx(stdscr, screenSize->h, screenSize->w);
}

void MainWindow::moveUp() {
  if (selected > 0) {
    selected--;
  }
}

void MainWindow::moveDown() {
  unsigned int offset = start + selected;
  unsigned int ps = winSize->h;

  if ((offset + winSize->h / 2) <= units.size() && selected < (winSize->h / 2)) {
    selected++;
  } else if ((selected + ps / 2) < units.size()) {
    start++;
  }
}

void MainWindow::movePageUp() {

}

void MainWindow::movePageDown() {
//  int offset = start + selected;
  unsigned int ps = winSize->h - 4;

    start = units.size() - 10;
//  if ((start + ps / 2) < units.size()) {
//    start += ps;
//  } else {
//  }
}

void MainWindow::drawUnits() {
  if (units.empty()) {
    units = ctl->getItems();
  }

  wclear(win);
  box(win, 0, 0);
  getmaxyx(win, winSize->h, winSize->w);

  for (int i = 0; i < (winSize->h - 2); i++) {
    if (i == selected) {
      wattron(win, A_REVERSE);
    }

    mvwprintw(win, i + 1, 1, "%d %s", units.size(), units[start + i]->id.c_str());
    wattroff(win, A_REVERSE);
  }

  refresh();
  wrefresh(win);
}
