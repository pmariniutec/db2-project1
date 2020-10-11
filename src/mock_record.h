#ifndef CUSTOMRECORD_H
#define CUSTOMRECORD_H

#include <iostream>
#include <cstring>
#include "record.h"
#include "csv_parser.h"

struct MockRecord : public Record {

  char id[10];
  char firstName[100];
  char lastName[100];
  char email[200];
  char gender[10];
  char ipAddress[30];

  MockRecord() = default;

  MockRecord(CSVRow row) {
	std::strncpy(id, row[0].data(), row[0].size());
	id[row[0].size()] = 0;

	std::strncpy(firstName, row[1].data(), row[1].size());
	firstName[row[1].size()] = 0;

	std::strncpy(lastName, row[2].data(), row[2].size());
	lastName[row[2].size()] = 0;

	std::strncpy(email, row[3].data(), row[3].size());
	email[row[3].size()] = 0;

	std::strncpy(gender, row[4].data(), row[4].size());
	gender[row[4].size()] = 0;

	std::strncpy(ipAddress, row[5].data(), row[5].size());
	ipAddress[row[5].size()] = 0;
  }

  void showData() {
	std::cout << id << ' ' << firstName << ' ' << lastName << ' ' << email << ' ' << gender << ' ' << ipAddress << '\n';
  }

  char* getKey() {
	return id;
  }

};

#endif
