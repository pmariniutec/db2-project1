#ifndef RECORD_H
#define RECORD_H

class Record {
  public:
	virtual void setData() = 0;
	virtual void showData() = 0;
	virtual void setLazy(int i) = 0;
	virtual char* getKey() = 0;
};

#endif
