#ifndef BPTREE_PM_H
#define BPTREE_PM_H

#include <fstream>
#include <string>
#include <iostream>

struct PageManager : protected std::fstream {
  public:
	PageManager(std::string fileName, bool trunc = false) : std::fstream{ fileName, std::ios::in | std::ios::out | std::ios::binary }, m_empty{ false }, m_fileName{ fileName } {
	  if (!good() || trunc) {
		m_empty = true;
		open(fileName, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
	  }
	}

	~PageManager() {
	  close();
	}

	inline bool isEmpty() { return m_empty; }

	template<typename Register>
	void save(const long& n, Register& reg) {
	  clear();
	  seekp(n * sizeof(Register), std::ios::beg);
	  write(reinterpret_cast<char*>(&reg), sizeof(reg));
	  writeCount++;
	}

	template<typename Register>
	bool recover(const long& n, Register& reg) {
	  clear();
	  seekg(n * sizeof(Register), std::ios::beg);
	  read(reinterpret_cast<char*>(&reg), sizeof(reg));
	  readCount++;
	  return gcount() > 0;
	}

	template<typename Register>
	void erase(const long& n) {
	  clear();
	  char mark = 'N';
	  seekg(n * sizeof(Register), std::ios::beg);
	  write(&mark, 1);
	  writeCount++;
	}

	int getReads() {
	  auto reads = readCount;
	  readCount = 0;
	  return reads;
	}

	int getWrites() {
	  auto writes = writeCount;
	  writeCount = 0;
	  return writes;
	}

  private:
	std::string m_fileName;
	int m_pageSize;
	bool m_empty;
	long m_pageIdCount;

	int writeCount{ 0 };
	int readCount{ 0 };
};

#endif
