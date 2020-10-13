#ifndef BPTREE_NODE_H
#define BPPREE_NODE_H

#include <cmath>

template<typename T, int TREE_ORDER>
struct Node {

  Node() = default;
  long pageId{ -1 };
  long count{ 0 };
  long right{ 0 };

  T data[TREE_ORDER + 1];
  long children[TREE_ORDER + 2];

  Node (long pageid) : pageId{ pageid } {
	for (int  i = 0; i < TREE_ORDER + 2; ++i) {
	  children[i] = 0;
	}
  }

  void insertInNode(int pos, const T& value) {
	int j = count;

	while (j > pos) {
	  data[j] = data[j - 1];
	  children[j + 1] = children[j];
	  j--;
	}
	data[j] = value;
	children[j + 1] = children[j];

	count++;
  }

  void deleteInNode(int pos) {
	for (int i = pos; i < count; ++i) {
	  data[i] = data[i + 1];
	  children[i + 1] = children[i + 2];
	}
	count--;
  }

  bool isOverflow() { return count > TREE_ORDER; }
  bool isUnderflow() { return count < std::floor(TREE_ORDER / 2.0); }

};

#endif
