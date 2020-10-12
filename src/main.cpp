#include <iostream>
#include <iterator>
#include "mock_record.h"
#include "dynamic_hashing.h"
#include "indexed_sequential_access.h"
#include "csv_parser.h"

int main() {
  //ExtendibleHashing<MockRecord> extendibleHashing("extendible", 8);
	IndexedSequentialAccess<MockRecord> indexed("alalau");

  std::ifstream file("data/mock_data.csv");

  MockRecord test;

  for (CSVIterator rowIter{ file, true }; rowIter != CSVIterator{}; ++rowIter) {
		//extendibleHashing.insert({ *rowIter });
		indexed.insert({ *rowIter });
		test = MockRecord(*rowIter);
  }

  //extendibleHashing.search(test);
	auto record = indexed.search(test.getKey());
	record.showData();
}
