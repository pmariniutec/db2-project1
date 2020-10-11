#include "file_organization.h"

#include <string>
#include <map>
#include <fstream>

int REPEATED = 0;

template<typename RecordType>
class IndexedSequentialAccess : public FileOrganization<RecordType> {
	public:
		IndexedSequentialAccess(std::string file, std::string index) : m_fileName{file}, m_indexName{index} {}
		IndexedSequentialAccess(std::string fileName);
		~IndexedSequentialAccess();
		
		void insert(RecordType record) override;
		RecordType search(char* key) override;
		bool remove(char* key) override;

		void readIndex() override;
		void writeIndex() override;
		void compressFile();
		void scan() override;

	private:
		std::string m_fileName;
		std::string m_indexName;
		std::map<int, int> m_index;
};

template<typename RecordType>
IndexedSequentialAccess<RecordType>::~IndexedSequentialAccess() {
	compressFile(); //not necesary
	writeIndex();
}

template<typename RecordType>
IndexedSequentialAccess<RecordType>::IndexedSequentialAccess(std::string fileName) : m_fileName{fileName}{

}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::readIndex() {
	std::ifstream file(m_indexName, std::ios::binary);
	int key;
	int value;

	while(file.read((char*)&key, sizeof(key))) {
		file.read((char*)&value, sizeof(value));
		m_index[key] = value;
  }
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::compressFile() {
	std::string new_file = m_fileName;
	new_file += "_new";

	std::ifstream infile(m_fileName, std::ios::binary);
	std::ofstream outfile(new_file, std::ios::binary | std::ios::app);
	
	RecordType record;
	int new_value = 0;

	for (auto& [key, value] : m_index) {
		if (value != -1) {
			//get record from data file.
			infile.seekg(value * sizeof(record));
			infile.read((char*) &record, sizeof(record));
			
			//insert record to new data file.
			outfile.write((char*) &record, sizeof(record));

			//change index value to new value.
			value = new_value;
			new_value++;
		}
	}
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::writeIndex() {
	std::ofstream file(m_indexName, std::ios::binary);
	for (auto& [key, value] : m_index) {
		if (value != -1) {
			file.write((char*) &key, sizeof(key));
			file.write((char*) &value, sizeof(value));
		}
	}
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::scan() {
	std::ifstream file(m_fileName, std::ios::binary);
	RecordType record;
  while (file.read((char*)&record, sizeof(record))) {
		record.showData();
  }
}

template<typename RecordType>
void IndexedSequentialAccess<RecordType>::insert(RecordType record) {
	std::ofstream file(m_fileName, std::ios::binary | std::ios::app);
	file.write((char*) &record, sizeof(record));
	
	auto key = record.getKey();

	auto item = m_index.find(key);
	if (item != m_index.end()) {
		item -> second = m_index.size() + REPEATED;
		REPEATED++;
	}else {
		m_index.insert(std::pair<int, int>(key,m_index.size() + REPEATED));
	}
}

template<typename RecordType>
RecordType IndexedSequentialAccess<RecordType>::search(char* key) {
	auto index_record = m_index.find(std::stoi(key));
	RecordType file_record;
	if (index_record != m_index.end()) {
		std::ifstream file(m_fileName, std::ios::binary);
		file.seekg(index_record->second * sizeof(RecordType));
		file.read((char*) &file_record, sizeof(file_record));
		return file_record;
	}else {
		return;
	}
}

template<typename RecordType>
bool IndexedSequentialAccess<RecordType>::remove(char* key) {
	auto index_record = m_index.find(std::stoi(key));
	if (index_record != m_index.end()) {
		index_record->second = -1;
		return true;
	}else {
		return false;
}

