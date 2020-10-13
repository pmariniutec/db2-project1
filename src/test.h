#ifndef TEST_H
#define TEST_H

#include "mock_record.h"
#include "indexed_sequential_access.h"

#include "bptree/bptree.h"
#include "bptree/page_manager.h"


struct Pair {
  long id;
  long pageId;

  bool operator<(const Pair& p) const {
	return id < p.id;
  }

  bool operator<=(const Pair& p) const {
	return id <= p.id;
  }

  bool operator==(const Pair& p) const {
	return id <= p.id;
  }
};

std::ostream& operator<<(std::ostream& out, Pair& p) {
  out << p.id;
  return out;
}

constexpr int PAGE_SIZE = 1024;
constexpr int BTREE_ORDER = ((PAGE_SIZE - (6 * sizeof(long) + sizeof(Pair))) / (sizeof(Pair) + sizeof(long)));


void testBPTree(std::string dataFile) {
  std::cout << "Running tests for B+ Tree for file " << dataFile << '\n';

  auto pm = std::make_shared<PageManager>("bptree.index", true);
  BPTree<Pair, BTREE_ORDER> bt(pm);
  PageManager recordManager("records.bin", true);
  long pageId;
  int i = 1;

  auto start = std::chrono::high_resolution_clock::now();

  std::fstream file(dataFile);
  for (CSVIterator rowIter{ file, true }; rowIter != CSVIterator{}; ++rowIter, ++i) {
	MockRecord rec{ *rowIter };
	pageId = i;
	recordManager.save(pageId, rec);
	bt.insert(Pair{std::stoi(rec.id), pageId});
  }

  auto finish = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::seconds>(finish - start).count();

  std::cout << "Insertion Time: " << time << "s" << '\n';
  std::cout << "Disk Reads: " << (bt.getReads() + recordManager.getReads()) << '\n';
  std::cout << "Disk Writes: " << (bt.getWrites() + recordManager.getWrites()) << '\n';

  start = std::chrono::high_resolution_clock::now();

  auto iter = bt.search(Pair{ 30000, -1 });
  auto end = bt.search(Pair{ 73000, -1 });

  for (; iter != end; ++iter) {
	auto pair = *iter;
	MockRecord rec;
	recordManager.recover(pair.pageId, rec);
  }

  finish = std::chrono::high_resolution_clock::now();

  time = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

  std::cout << "Range-Search Time: " << time << "ms" << '\n';
  std::cout << "Disk Reads: " << (bt.getReads() + recordManager.getReads()) << '\n';
  std::cout << "Disk Writes: " << (bt.getWrites() + recordManager.getWrites()) << '\n';
}

void testISAM(std::string dataFile) {
  std::cout << "Running tests for ISAM for file " << dataFile << '\n';

  auto start = std::chrono::high_resolution_clock::now();

  auto fileOrganization = new IndexedSequentialAccess<MockRecord>("isam");
  std::ifstream file(dataFile);
  for (CSVIterator rowIter{ file, true }; rowIter != CSVIterator{}; ++rowIter) {
	fileOrganization->insert({ *rowIter });
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
  std::cout << "Insertion time: " << time << "s" << '\n';
  std::cout << "Disk Reads: " << fileOrganization->getReads() << '\n';
  std::cout << "Disk Writes: " << fileOrganization->getWrites() << '\n';

  start = std::chrono::high_resolution_clock::now();

  auto tmp = fileOrganization->search("200");

  end = std::chrono::high_resolution_clock::now();

  time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Search time: " << time << "ms" << '\n';
  std::cout << "Disk Reads: " << fileOrganization->getReads() << '\n';
  std::cout << "Disk Writes: " << fileOrganization->getWrites() << '\n';
}

#endif
