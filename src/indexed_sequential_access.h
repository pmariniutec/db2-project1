#ifndef ISAM_H
#define ISAM_H

#include "file_organization.h"

#include <string>
#include <filesystem>
#include <map>
#include <fstream>

static int REPEATED = 0;

template<typename RecordType>
class IndexedSequentialAccess : public FileOrganization<RecordType> {
  public:
	IndexedSequentialAccess(std::string file, std::string index) : m_fileName{ file }, m_indexName{ index } {}
	IndexedSequentialAccess(std::string fileName);
	~IndexedSequentialAccess();

	void insert(RecordType record) override;
	RecordType search(char* key) override;
	bool remove(char* key) override;

	void readIndex() override;
	void writeIndex() override;
	void compressFile();
	std::vector<RecordType> scan() override;

  private:
	std::string m_fileName;
	std::string m_indexName;
	std::map<int, int> m_index;
};

template<typename RecordType>
IndexedSequentialAccess<RecordType>::~IndexedSequentialAccess() {
  writeIndex();
}

template<typename RecordType>
IndexedSequentialAccess<RecordType>::IndexedSequentialAccess(std::string fileName) : m_fileName{ fileName } {
  m_indexName = "indices/indexed_sequential_access/" + m_fileName;
  std::fstream(m_indexName, std::ios::binary);
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::readIndex() {
  std::ifstream file(m_indexName, std::ios::binary);
  int key;
  int value;

  while (file.read((char*)&key, sizeof(key))) {
	file.read((char*)&value, sizeof(value));
	m_index[key] = value;
  }
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::compressFile() {
  std::string tmpFile = m_fileName + "_tmp";

  std::ifstream infile(m_fileName, std::ios::binary);
  std::ofstream outfile(tmpFile, std::ios::binary | std::ios::app);

  RecordType record;
  int newValue = 0;

  for (auto& [key, value] : m_index) {
	if (value != -1) {
	  //get record from data file.
	  infile.seekg(value * sizeof(record));
	  infile.read((char*)&record, sizeof(record));

	  //insert record to new data file.
	  outfile.write((char*)&record, sizeof(record));

	  //change index value to new value.
	  value = newValue;
	  newValue++;
	}
  }

  std::filesystem::rename(tmpFile, m_fileName);
  REPEATED = 0;
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::writeIndex() {
  std::ofstream file(m_indexName, std::ios::binary);
  for (auto& [key, value] : m_index) {
	if (value != -1) {
	  file.write((char*)&key, sizeof(key));
	  file.write((char*)&value, sizeof(value));
	}
  }
}

template<typename RecordType>
std::vector<RecordType> IndexedSequentialAccess<RecordType>::scan() {
  std::vector<RecordType> records;

  std::ifstream file(m_fileName, std::ios::binary);
  RecordType record;
  while (file.read((char*)&record, sizeof(record))) {
	records.push_back(record);
  }

  return records;
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::insert(RecordType record) {
  std::ofstream file(m_fileName, std::ios::binary | std::ios::app);
  file.write((char*)&record, sizeof(record));

  auto key = record.getKey();

  auto index_record = m_index.find(std::stoi(key));
  if (index_record != m_index.end()) {
	index_record->second = m_index.size() + REPEATED;
	REPEATED++;
  } else {
	m_index.insert(std::pair<int, int>(std::stoi(key), m_index.size() + REPEATED));
  }
}

template<typename RecordType>
RecordType IndexedSequentialAccess<RecordType>::search(char* key) {
  auto index_record = m_index.find(std::stoi(key));
  RecordType file_record;

  if (index_record != m_index.end()) {
	std::ifstream file(m_fileName, std::ios::binary);
	file.seekg(index_record->second * sizeof(RecordType));
	file.read((char*)&file_record, sizeof(file_record));
	return file_record;
  } else {
	throw std::invalid_argument("Key value is not found");
  }
}

template<typename RecordType>
bool IndexedSequentialAccess<RecordType>::remove(char* key) {
  auto index_record = m_index.find(std::stoi(key));
  if (index_record != m_index.end()) {
	index_record->second = -1;
	return true;
  } else {
	return false;
  }
}

#endif
