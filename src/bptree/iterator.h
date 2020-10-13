#ifndef BPTREE_ITERATOR_H
#define BPTREE_ITERATOR_H

#include <memory>

#include "node.h"
#include "page_manager.h"

template<typename T, int B>
class Iterator {
  public:
	using NodeType = Node<T, B>;
	int index = 0;
	NodeType currentPosition;

	Iterator() = default;
	Iterator(std::shared_ptr<PageManager> pm) : m_pageManager{ pm } {}

	T operator*() {
	  return currentPosition.data[index];
	}

	Iterator& operator++() {
	  if (index < currentPosition.count - 1) {
		index++;
	  } else {
		index = 0;
		NodeType aux{ -1 };
		if (currentPosition.right == 0) {
		  currentPosition = aux;
		} else {
		  m_pageManager->recover(currentPosition.right, currentPosition);
		}
	  }
	  return *this;
	}

	Iterator& operator=(const Iterator& rhs) {
	  currentPosition = rhs.currentPosition;
	  index = rhs.index;
	  m_pageManager = rhs.m_pageManager;
	  return *this;
	}

	Iterator(const Iterator& rhs) {
	  currentPosition = rhs.currentPosition;
	  index = rhs.index;
	  m_pageManager = rhs.m_pageManager;
	}

	bool operator!=(const Iterator& rhs) const {
	  if (currentPosition.pageId == rhs.currentPosition.pageId) {
		return !(currentPosition.data[index] == rhs.currentPosition.data[index]);
	  }
	  return true;
	}

  private:
	std::shared_ptr<PageManager> m_pageManager;
};

#endif
