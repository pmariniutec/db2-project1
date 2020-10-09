#include "file_organization.h"

#include <string>
#include <map>
#include <fstream>

struct IndexInfo {
  std::string m_fullName;
  std::string m_name;
  int m_localDepth;

  void setName() {
	m_name = "";
	auto len = m_fullName.length();
	for (int i = 0; i < m_localDepth; ++i) {
	  m_name = m_fullName[len - 1 - i] + m_name;
	}
  }

  void setFullName(int n, int d) {
	m_fullName = "0";
	for (int i = 0; i < d; ++i) {
	  m_fullName = m_fullName + "0";
	}
	for (int i = 0; n > 0; ++i) {
	  if (n % 2 == 0)
		m_fullName[d - 1 - i] = '0';
	  else
		m_fullName[d - 1 - i] = '1';

	  n /= 2;
	}
  }

};

template<typename RecordType>
class ExtendibleHashing : public FileOrganization<RecordType> {
  public:
	ExtendibleHashing(std::string name, int d);
	~ExtendibleHashing();

	void search(RecordType record) override;
	void insert(RecordType record) override;

	void readIndex() override;
	void writeIndex() override;
	void scan() override;

	int hashInt(char* key);
	void initIndex();
	void insertLinked(RecordType record);
	int getBinaryDepth();
	std::string hash(char* key);

  private:
	std::string m_fileName;
	std::string m_indexName;
	std::map<int, IndexInfo> m_index;
	int m_depth;
	int m_binaryDepth;
	int m_maxBucketSize = B;
	int m_currentTopFileIndex;
};

template<typename RecordType>
ExtendibleHashing<RecordType>::ExtendibleHashing(std::string name, int d) : m_fileName{ name }, m_depth{ d } {
  m_indexName = "dynamicHash/" + m_fileName + "_ind";
  m_binaryDepth = getBinaryDepth();
  m_maxBucketSize = B;
  initIndex();
  readIndex();
}

template<typename RecordType>
ExtendibleHashing<RecordType>::~ExtendibleHashing() {
  writeIndex();
}

template<typename RecordType>
int ExtendibleHashing<RecordType>::getBinaryDepth() {
  int counter = 0;
  for (int i = 1; i < m_depth; i = i * 2)
	counter++;
  return counter;
}

template<typename RecordType>
void ExtendibleHashing<RecordType>::initIndex() {
  for (int i = 0; i < m_depth; ++i) {
	IndexInfo info;
	info.setFullName(i, m_binaryDepth);
	info.m_localDepth = 1;
	info.setName();
	m_index[i] = info;
  }
}

template<typename RecordType>
void ExtendibleHashing<RecordType>::search(RecordType record) {
  auto key = record.getKey();
  auto current = hashInt(key);

  if (m_index[current].m_localDepth == m_binaryDepth) {
	Bucket bucket;
	auto indexFileName = m_indexName + m_index[current].m_fullName;
	std::ifstream file(indexFileName, std::ios::binary);
	file.read((char*)&bucket, sizeof(bucket));
	file.close();
	auto next = bucket.next;
	int n;

	while (current != next) {
	  n = bucket.size;
	  for (int i = 0; i < n; ++i) {
		if (bucket.key[i] == key) {
		  std::cout << "Pos: " << bucket.pos[i] << '\n';
		  return;
		}
	  }
	  indexFileName = m_indexName + std::to_string(next);
	  file.open(indexFileName, std::ios::binary);
	  file.read((char*)&bucket, sizeof(bucket));
	  current = next;
	  next = bucket.next;
	}

	n = bucket.size;
	for (int i = 0; i < n; ++i) {
	  if (bucket.key[i] == key) {
		std::cout << "Pos: " << bucket.pos[i] << '\n';
		return;
	  }
	}
  } else {
	Bucket bucket;
	auto indexFileName = m_indexName + m_index[current].name;
	std::ifstream file(indexFileName, std::ios::binary);
	file.read((char*)&bucket, sizeof(bucket));
	file.close();
	int n = bucket.size;
	for (int i = 0; i < n; ++i) {
	  if (bucket.key[i] == key) {
		std::cout << "Pos: " << bucket.pos[i] << '\n';
		return;
	  }
	}
  }

  std::cout << "[ERROR] Key not found\n";
}

template<typename RecordType>
void ExtendibleHashing<RecordType>::insert(RecordType record) {
  auto key = record.getKey();
  int current = hashInt(key);
  auto indexPtr = m_index[current];
  if (indexPtr.m_localDepth == m_binaryDepth) {
	insertLinked(record);
  } else {
	std::ofstream file(m_fileName, std::ios::binary | std::ios::app);
	file.write((char*)&record, sizeof(record));
	auto pos = file.tellp();
	file.close();
	auto currentName = m_index[current].m_name;
	auto currentFileBucket = m_indexName + currentName;
	std::cout << currentFileBucket << '\n';

	int next;
	std::string nextFileBucket;
	int bucketSize;

	Bucket bucket;
	bucket.next = hashInt(key);
	bucket.size = 0;

	std::ifstream inFile(currentFileBucket, std::ios::binary);
	inFile.read((char*)&bucket, sizeof(bucket));
	inFile.close();

	next = bucket.next;
	bucketSize = bucket.size;
	int counter = 0;

	std::cout << bucketSize << '\n';
	std::cout << m_maxBucketSize << '\n';

	if (bucketSize == m_maxBucketSize) {
	  for (auto& [ first, second ] : m_index) {
		if (second.m_name == currentName) {
		  auto newBucketIndex = first;
		  second.m_localDepth++;
		  second.setName();
		}
	  }
	  for (int i = 0; i < bucketSize; ++i) {
		auto reassignKey = bucket.key[i];
		auto reassignPos = bucket.pos[i];
		auto current = hashInt(reassignKey);

		auto reassign = m_index[current].name;
		auto reassignIndex = m_indexName + reassign;
		Bucket obj;
		obj.size = 0;
		obj.next = current;
		std::ifstream inFile(reassignIndex, std::ios::binary);
		inFile.read((char*)&obj, sizeof(obj));
		inFile.close();

		int size = ++obj.size;
		obj.key[size - 1] = reassignKey;
		obj.pos[size - 1] = reassignPos;

		std::ofstream outFile(reassignIndex, std::ios::binary | std::ios::trunc);
		outFile.write((char*)&obj, sizeof(obj));
		outFile.close();
		insert(record);
	  }
	} else {
	  std::ofstream outFile(currentFileBucket, std::ios::out | std::ios::trunc);
	  auto size = ++bucket.size;
	  bucket.key[size - 1] = key;
	  bucket.pos[size - 1] = pos;
	  if (size == m_maxBucketSize && m_index[hashInt(key)].m_localDepth == m_binaryDepth) {
		bucket.next = ++m_currentTopFileIndex;
	  }
	  outFile.write((char*)&bucket, sizeof(bucket));
	  outFile.close();
	}
  }
}

template<typename RecordType>
void ExtendibleHashing<RecordType>::insertLinked(RecordType record) {
  std::ofstream outFile(m_fileName, std::ios::binary | std::ios::app);
  outFile.write((char*)&record, sizeof(record));
  auto pos = outFile.tellp();
  outFile.close();

  auto key = record.getKey();

  auto current = hashInt(key);
  auto currentFileBucket = m_indexName + m_index[current].m_fullName;

  int next;
  std::string nextFileBucket;

  int bucketSize;

  Bucket bucket;
  bucket.next = hashInt(key);
  bucket.size = 0;

  std::ifstream inFile(currentFileBucket, std::ios::binary);
  inFile.read((char*)&bucket, sizeof(bucket));
  inFile.close();

  next = bucket.next;
  bucketSize = bucket.size;

  while(current != next) {
	current = next;
	nextFileBucket = m_indexName + std::to_string(next);
	inFile.open(nextFileBucket, std::ios::binary);
	inFile.read((char*)&bucket, sizeof(bucket));
	bucketSize = bucket.size;
	next = bucket.next;
	inFile.close();
  }

  currentFileBucket = m_indexName + std::to_string(current);

  if (bucketSize < m_maxBucketSize) {
	inFile.open(currentFileBucket, std::ios::binary);
	inFile.read((char*)&bucket, sizeof(bucket));
	inFile.close();

	bucket.key[bucketSize] = key;
	bucket.pos[bucketSize] = pos;
	bucket.size = bucketSize + 1;
	bucket.next = current;

	std::ofstream outIndexFile(currentFileBucket, std::ios::binary | std::ios::trunc);
	outIndexFile.write((char*)&bucket, sizeof(bucket));
	outIndexFile.close();
  } else {
	m_currentTopFileIndex++;
	inFile.open(currentFileBucket, std::ios::binary);
	inFile.read((char*)&bucket, sizeof(bucket));
	inFile.close();

	bucket.next = m_currentTopFileIndex;
	std::ofstream outIndexFile(currentFileBucket, std::ios::binary | std::ios::trunc);
	outIndexFile.write((char*)&bucket, sizeof(bucket));
	outIndexFile.close();

	auto newFileBucket = m_indexName + std::to_string(m_currentTopFileIndex);
	std::ofstream newIndexFile(newFileBucket, std::ios::binary);
	Bucket newBucket;

	newBucket.next = m_currentTopFileIndex;
	newBucket.size = 0;

	newIndexFile.write((char*)&newBucket, sizeof(newBucket));
	newIndexFile.close();
  }

}

template<typename RecordType>
void ExtendibleHashing<RecordType>::writeIndex() {
  std::ofstream file(m_indexName, std::ios::binary);
  for (auto& [ first, second ] : m_index) {
	file.write((char*)&first, sizeof(first));
	file.write((char*)&second, sizeof(second));
  }
  std::ifstream metadata("dynamicHash/hashMetadata", std::ios::binary | std::ios::trunc);
  metadata.read((char*)&m_currentTopFileIndex, sizeof(m_currentTopFileIndex));
}

template<typename RecordType>
void ExtendibleHashing<RecordType>::readIndex() {
  std::ifstream file(m_indexName, std::ios::binary);
  int key;
  IndexInfo value;
  while(file.read((char*)&key, sizeof(key))) {
	file.read((char*)&value, sizeof(value));
	m_index[key] = value;
  }
  m_currentTopFileIndex = m_depth - 1;
  std::ifstream metadata("dynamicHash/hashMetadata", std::ios::binary);
  metadata.read((char*)&m_currentTopFileIndex, sizeof(m_currentTopFileIndex));
}

template<typename RecordType>
void ExtendibleHashing<RecordType>::scan() {
  std::ifstream file(m_fileName, std::ios::binary);
  RecordType record;
  while (file.read((char*)&record, sizeof(record))) {
	record.showData();
  }
}

template<typename RecordType>
std::string ExtendibleHashing<RecordType>::hash(char* key) {
  std::string s(key);
  int k = std::stoi(s);
  return { k % m_depth };
}

template<typename RecordType>
int ExtendibleHashing<RecordType>::hashInt(char* key) {
  std::string s(key);
  int k = std::stoi(s);
  return k % m_depth;
}
