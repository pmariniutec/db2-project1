#include <iostream>
#include <iterator>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>

#include <QApplication>

#include "ui/main_window.h"

#include "test.h"

int main(int argc, char* argv[]) {
  testISAM("data/small_data.csv");
  testISAM("data/medium_data.csv");
  testISAM("data/large_data.csv");

  testBPTree("data/small_data.csv");
  testBPTree("data/medium_data.csv");
  testBPTree("data/large_data.csv");

  /*
  QApplication app(argc, argv);
  MainWindow mw;
  mw.show();
  return app.exec();
  */
}
