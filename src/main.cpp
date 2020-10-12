#include <iostream>
#include <iterator>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>

#include <QApplication>

#include "mock_record.h"
#include "extendible_hashing.h"
#include "indexed_sequential_access.h"
#include "csv_parser.h"
#include "ui/main_window.h"

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(1, 2000);

std::mutex fileLock;

template<typename FileOrganization>
void transactionExecute(FileOrganization* file) {
  MockRecord tmp{ "", "", "", "", "", "" };
  std::lock_guard<std::mutex> guard(fileLock);
  file->insert(tmp);
}

int main(int argc, char* argv[]) {
  /*
  std::thread t1{ &transactionExecute<ExtendibleHashing<MockRecord>>, &fo };
  std::thread t2{ &transactionExecute<ExtendibleHashing<MockRecord>>, &fo };
  t1.join();
  t2.join();
  */

  QApplication app(argc, argv);
  MainWindow mw;
  mw.show();
  return app.exec();
}
