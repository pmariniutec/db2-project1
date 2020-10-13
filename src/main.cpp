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

  auto start = std::chrono::high_resolution_clock::now();

  auto fileOrganization = new ExtendibleHashing<MockRecord>("extendible_hashing", 8);
  std::ifstream file("data/mock_data.csv");
  for (CSVIterator rowIter{ file, true }; rowIter != CSVIterator{}; ++rowIter) {
	fileOrganization->insert({ *rowIter });
  }

  auto end = std::chrono::high_resolution_clock::now();

  auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

  std::cout << "Time: " << time << '\n';

  /*
  QApplication app(argc, argv);
  MainWindow mw;
  mw.show();
  return app.exec();
  */
}
