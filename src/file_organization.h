#ifndef FILEORGANIZATION_H
#define FILEORGANIZATION_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

constexpr std::size_t BUCKET_SIZE = 10;

struct Bucket {
  int next;
  int size;
  char* key[BUCKET_SIZE];
  long pos[BUCKET_SIZE];
};

template<typename RecordType>
class FileOrganization {
  public:
	virtual void readIndex() = 0;
	virtual void writeIndex() = 0;
	virtual std::vector<RecordType> scan() = 0;
	virtual void insert(RecordType record) = 0;
	virtual RecordType search(char* key) = 0;
	virtual bool remove(char* key) = 0;
};

#endif
