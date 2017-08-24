#include "chk-ui.h"
#include "chk.h"

void startCurses() {
  initscr();
  start_color();
  noecho();
  cbreak();
  curs_set(false);

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
}

void stopCurses() {
  endwin();
}

void printInMiddle(WINDOW *win, int starty, int startx, int width,
    char *string, chtype color, char *sp) {

  int length, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if(startx != 0)
    x = startx;
  if(starty != 0)
    y = starty;
  if(width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  wattron(win, color);

  if (sp) {
    for (int i = startx; i < width; i++) {
      mvwprintw(win, y, i, "%c", sp);
    }
  }

  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
}

void aboutWindow(RECTANGLE *parent) {
  const int winH = 22;
  const int winW = 60;

  WINDOW *aboutwin = newwin(winH, winW,
      (parent->h / 2) - (winH / 2),
      (parent->w / 2) - (winW / 2)
      );

  wattron(aboutwin, COLOR_PAIR(2));
  mvwprintw(aboutwin, 0, 0, ABOUT_INFO, VERSION);
  wattroff(aboutwin, COLOR_PAIR(2));
  refresh();
  wrefresh(aboutwin);
  getch();
  delwin(aboutwin);
  clear();
  refresh();
}
