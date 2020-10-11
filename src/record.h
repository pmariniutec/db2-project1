#ifndef RECORD_H
#define RECORD_H

class Record {
  public:
	virtual void showData() = 0;
	virtual char* getKey() = 0;
};

#endif
