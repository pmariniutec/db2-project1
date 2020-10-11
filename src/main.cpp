#include <iostream>
#include <iterator>
#include "mock_record.h"
#include "dynamic_hashing.h"
#include "csv_parser.h"

int main() {
  ExtendibleHashing<MockRecord> extendibleHashing("extendible", 8);

  std::ifstream file("data/mock_data.csv");

  MockRecord test;

  for (CSVIterator rowIter{ file, true }; rowIter != CSVIterator{}; ++rowIter) {
	extendibleHashing.insert({ *rowIter });
	test = MockRecord(*rowIter);
  }

  extendibleHashing.search(test);
}
