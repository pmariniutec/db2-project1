#include <iostream>
#include "dynamic_hashing.h"
#include "record.h"

// TODO: move to own file.
struct CustomRecord : public Record {

};

int main() {
  ExtendibleHashing<CustomRecord> dynamicFile("dynamic", 8);
}
