#ifndef BPTREE_H
#define BPTREE_H

#include <memory>

#include "iterator.h"
#include "page_manager.h"

template<typename T, int TREE_ORDER = 3>
class BPTree {
  public:
	using NodeType = Node<T, TREE_ORDER>;
	using IteratorType = Iterator<T, TREE_ORDER>;

	struct Metadata {
	  long rootId{ 1 };
	  long count{ 0 };
	} header;

	enum state {
	  BPTREE_OVERFLOW,
	  BPTREE_UNDERFLOW,
	  NORMAL
	};

	BPTree(std::shared_ptr<PageManager> pm) : m_pageManager{ pm } {
	  if (m_pageManager->isEmpty()) {
		NodeType root{ header.rootId };
		m_pageManager->save(root.pageId, root);

		header.count++;
		m_pageManager->save(0, header);
	  } else {
		m_pageManager->recover(0, header);
	  }
	}

	NodeType newNode() {
	  header.count++;
	  NodeType ret{ header.count };
	  m_pageManager->save(0, header);
	  return ret;
	}

	NodeType readNode(long pageid) {
	  NodeType n{ -1 };
	  m_pageManager->recover(pageid, n);
	  return n;
	}

	void writeNode(long pageid, NodeType n) {
	  m_pageManager->save(pageid, n);
	}

	void insert(const T& value) {
	  auto root = readNode(header.rootId);
	  int state = insert(root, value);

	  if (state == BPTREE_OVERFLOW) {
		splitRoot();
	  }
	}

	int insert(NodeType& ptr, const T& value) {
	  int pos = 0;
	  while (pos < ptr.count && ptr.data[pos] < value) {
		pos++;
	  }

	  if (ptr.children[pos] != 0) {
		long pageid = ptr.children[pos];
		auto child = readNode(pageid);
		auto state = insert(child, value);
		if (state == BPTREE_OVERFLOW) {
		  split(ptr, pos);
		}
	  } else {
		ptr.insertInNode(pos, value);
		writeNode(ptr.pageId, ptr);
	  }

	  return ptr.isOverflow() ? BPTREE_OVERFLOW : NORMAL;
	}

	void split(NodeType& parent, int pos) {
	  auto nodeInOverflow = readNode(parent.children[pos]);
	  auto child1 = nodeInOverflow;
	  child1.count = 0;
	  auto child2 = newNode();

	  int iter = 0;
	  int i ;
	  for (i = 0; iter < std::ceil(TREE_ORDER / 2.0); ++i) {
		child1.children[i] = nodeInOverflow.children[iter];
		child1.data[i] = nodeInOverflow.data[iter];
		child1.count++;
		iter++;
	  }

	  child1.children[i] = nodeInOverflow.children[iter];
	  parent.insertInNode(pos, nodeInOverflow.data[iter]);

	  if (nodeInOverflow.children[0] != 0) {
		iter++;
	  } else {
		child2.right = child1.right;
		child1.right = child2.pageId;
		parent.children[pos + 1] = child2.pageId;
	  }

	  for (i = 0; iter < TREE_ORDER + 1; ++i) {
		child2.children[i] = nodeInOverflow.children[iter];
		child2.data[i] = nodeInOverflow.data[iter];
		child2.count++;
		iter++;
	  }

	  child2.children[i] = nodeInOverflow.children[iter];
	  parent.children[pos] = child1.pageId;
	  parent.children[pos + 1] = child2.pageId;

	  writeNode(parent.pageId, parent);
	  writeNode(child1.pageId, child1);
	  writeNode(child2.pageId, child2);
	}

	void splitRoot() {
	  auto nodeInOverflow = readNode(header.rootId);
	  auto child1 = newNode();
	  auto child2 = newNode();

	  int pos = 0;
	  int iter = 0;
	  int i ;
	  for (i = 0; iter < std::ceil(TREE_ORDER  / 2.0); ++i) {
		child1.children[i] = nodeInOverflow.children[iter];
		child1.data[i] = nodeInOverflow.data[iter];
		child1.count++;
		iter++;
	  }

	  child1.children[i] = nodeInOverflow.children[iter];
	  nodeInOverflow.data[0] = nodeInOverflow.data[iter];
	  child1.right = child2.pageId;
	  if (nodeInOverflow.children[0] != 0) {
		iter++;
	  }

	  for (i = 0; iter < TREE_ORDER + 1; ++i) {
		child2.children[i] = nodeInOverflow.children[iter];
		child2.data[i] = nodeInOverflow.data[iter];
		child2.count++;
		iter++;
	  }

	  child2.children[i] = nodeInOverflow.children[iter];

	  nodeInOverflow.children[0] = child1.pageId;
	  nodeInOverflow.children[1] = child2.pageId;
	  nodeInOverflow.count = 1;

	  writeNode(nodeInOverflow.pageId, nodeInOverflow);
	  writeNode(child1.pageId, child1);
	  writeNode(child2.pageId, child2);
	}

	void remove(const T& value) {
	  auto root = readNode(header.rootId);
	  int state = remove(root, value);
	  if (state == BPTREE_UNDERFLOW && root.count == 0) {
		header.rootId = root.children[0];
		writeNode(root.pageId, root);
		m_pageManager->save(0, header);
	  }
	}

	int remove(NodeType& ptr, const T& value) {
	  int pos = 0;
	  while (pos < ptr.count && ptr.data[pos] < value) {
		pos++;
	  }

	  if (ptr.children[pos] != 0) {
		if (ptr.data[pos] == value && pos != ptr.count) {
		  auto next = readNode(ptr.children[pos + 1]);
		  ptr.data[pos] = succesor(next);
		  writeNode(ptr.pageId, ptr);
		  pos++;
		}
		auto child = readNode(ptr.children[pos]);
		int state = remove(child, value);
		if (state == BPTREE_UNDERFLOW) {
		  auto nodeInUnderflow = child;
		  bool canSteal = stealSibling(nodeInUnderflow, ptr, pos);
		  if (!canSteal) {
			if (nodeInUnderflow.children[0] == 0) {
			  mergeLeaf(ptr, nodeInUnderflow, pos);
			} else {
			  auto canMerge = mergeWithParent(ptr, nodeInUnderflow, pos);
			  if (!canMerge) {
				decreaseHeight(ptr, nodeInUnderflow, pos);
			  }
			}
		  }
		}
	  } else if (ptr.data[pos] == value) {
		ptr.deleteInNode(pos);
		writeNode(ptr.pageId, ptr);
	  }

	  return ptr.isUnderflow() ? BPTREE_UNDERFLOW : NORMAL;
	}

	void decreaseHeight(NodeType& ptr, NodeType& nodeInUnderflow, int pos) {
	  if (pos != ptr.count) {
		auto child = readNode(ptr.children[pos]);
		if (child.count < std::floor(TREE_ORDER / 2.0)) {
		  auto sibling = readNode(ptr.children[pos + 1]);
		  sibling.insertInNode(0, ptr.data[pos]);
		  int last = nodeInUnderflow.count;
		  sibling.children[0] = nodeInUnderflow.children[last];

		  for (int i = last - 1; i >= 0; --i) {
			sibling.insertInNode(0, nodeInUnderflow.data[i]);
			sibling.children[0] = nodeInUnderflow.children[i];
		  }
		  ptr.deleteInNode(pos);
		  ptr.children[pos] = sibling.pageId;
		  writeNode(sibling.pageId, sibling);
		  writeNode(ptr.pageId, ptr);
		  return;
		}
	  }
	  auto sibling = readNode(ptr.children[pos - 1]);
	  int last = sibling.count;
	  sibling.insertInNode(last, ptr.data[pos - 1]);
	  sibling.children[last + 1] = nodeInUnderflow.children[0];
	  for (int i = 0; i < nodeInUnderflow.count; ++i) {
		last = sibling.count;
		sibling.insertInNode(last, nodeInUnderflow.data[i]);
		sibling.children[last + 1] = nodeInUnderflow.children[i + 1];
	  }
	  ptr.deleteInNode(pos -1);
	  ptr.children[pos - 1] = sibling.pageId;

	  writeNode(sibling.pageId, sibling);
	  writeNode(ptr.pageId, ptr);
	}

	bool mergeWithParent(NodeType& ptr, NodeType& nodeInUnderflow, int pos) {
	  if (pos != 0) {
		auto sibling  = readNode(ptr.children[pos - 1]);
		if (sibling.count -1 >= std::floor(TREE_ORDER / 2.0)) {
		  auto tmp = ptr.data[pos];
		  nodeInUnderflow.insertInNode(0, tmp);
		  ptr.data[pos] = sibling.data[0];
		  nodeInUnderflow.children[1] = sibling.children[0];
		  sibling.children[0] = sibling.children[1];
		  sibling.deleteInNode(0);

		  writeNode(sibling.pageId, sibling);
		  writeNode(ptr.pageId, ptr);
		  writeNode(nodeInUnderflow.pageId, nodeInUnderflow);
		  return true;
		}
	  } else if (pos != TREE_ORDER) {
		auto sibling = readNode(ptr.children[pos + 1]);
		if (sibling.count - 1 >= std::floor(TREE_ORDER / 2.0)) {
		  auto tmp = ptr.data[pos];
		  nodeInUnderflow.insertInNode(0, tmp);
		  ptr.data[pos] = sibling.data[0];
		  nodeInUnderflow.children[1] = sibling.children[0];
		  sibling.children[0] = sibling.children[1];
		  sibling.deleteInNode(0);

		  writeNode(sibling.pageId, sibling);
		  writeNode(ptr.pageId, ptr);
		  writeNode(nodeInUnderflow.pageId, nodeInUnderflow);
		  return true;
		}
	  }
	}

	void mergeLeaf(NodeType& ptr, NodeType& nodeInUnderflow, int pos) {
	  if (pos - 1 >= 0) {
		auto sibling = readNode(ptr.children[pos - 1]);
		for (int i = 0; i < nodeInUnderflow.count; ++i) {
		  auto posIn = sibling.count;
		  sibling.insertInNode(posIn, nodeInUnderflow.data[i]);
		}
		sibling.right = nodeInUnderflow.right;
		ptr.deleteInNode(pos - 1);
		writeNode(sibling.pageId, sibling);
		writeNode(ptr.pageId, ptr);
	  } else {
		auto sibling = readNode(ptr.children[1]);
		for (int i = 0; i < sibling.count; ++i) {
		  auto posIn = nodeInUnderflow.count;
		  nodeInUnderflow.insertInNode(pos, sibling.data[i]);
		}
		nodeInUnderflow.right = sibling.right;
		ptr.deleteInNode(0);
		writeNode(nodeInUnderflow.pageId, nodeInUnderflow);
		writeNode(ptr.pageId, ptr);
	  }
	}

	bool stealSibling(NodeType& nodeInUnderflow, NodeType& ptr, int pos) {
	  if (nodeInUnderflow.children[0] == 0) {
		if (pos != ptr.count) {
		  auto sibling = readNode(ptr.children[pos + 1]);
		  if (sibling.count - 1 >= std::floor(TREE_ORDER / 2.0)) {
			auto tmp = sibling.data[0];
			auto tmp2 = sibling.data[1];
			sibling.deleteInNode(0);
			nodeInUnderflow.insertInNode(sibling.count - 1, tmp);
			ptr.data[pos] = tmp2;
			writeNode(sibling.pageId, sibling);
			writeNode(nodeInUnderflow.pageId, nodeInUnderflow);
			writeNode(ptr.pageId, ptr);
			return true;
		  }
		}
		if (pos > 0) {
		  auto sibling = readNode(ptr.children[pos -1]);
		  if (sibling.count - 1 >= std::floor(TREE_ORDER / 2.0)) {
			auto tmp = sibling.data[sibling.count - 1];
			sibling.deleteInNode(sibling.count - 1);
			nodeInUnderflow.insertInNode(0, tmp);
			ptr.data[pos - 1] = tmp;
			writeNode(sibling.pageId, sibling);
			writeNode(nodeInUnderflow.pageId, nodeInUnderflow);
			writeNode(ptr.pageId, ptr);
			return true;
		  }
		}
	  }
	  return false;
	}

	T succesor(NodeType& ptr) {
	  while (ptr.children[0] != 0) {
		ptr = readNode(ptr.children[0]);
	  }
	  if (ptr.count == 1) {
		if (ptr.right == -1)
		  return NULL;
		ptr = readNode(ptr.right);
		return ptr.data[0];
	  } else {
		return ptr.data[1];
	  }
	}

	IteratorType begin() {
	  auto aux = readNode(header.rootId);
	  while(aux.children[0] != 0) {
		auto pageid = aux.children[0];
		aux = readNode(pageid);
	  }

	  IteratorType it{ m_pageManager };
	  it.currentPosition = aux;
	  return it;
	}

	IteratorType end() {
	  NodeType aux{ -1 };
	  IteratorType it{ m_pageManager };
	  it.currentPosition = aux;
	  return it;
	}

	IteratorType search(const T& object) {
	  auto root = readNode(header.rootId);
	  auto it = search(object, root);
	  if (*it == object) {
		return it;
	  } else {
		return end();
	  }
	}

	IteratorType search(const T& object, const NodeType& other) {
	  int pos = 0;
	  if (other.children[0] != 0) {
		while (pos < other.count && other.data[pos] <= object) {
		  pos++;
		}
		auto child = readNode(other.children[pos]);
		return search(object, child);
	  } else {
		while(pos < other.count && other.data[pos] < object) {
		  pos++;
		}
		IteratorType it{ m_pageManager };
		it.currentPosition = other;
		it.index = pos;
		return it;
	  }
	}

	int getReads() {
	  return m_pageManager->getReads();
	}

	int getWrites() {
	  return m_pageManager->getWrites();
	}

  private:
	std::shared_ptr<PageManager> m_pageManager;
};

#endif
