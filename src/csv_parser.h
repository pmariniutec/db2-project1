#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <fstream>
#include <iostream>
#include <vector>

class CSVRow {
  public:
	std::string_view operator[](std::size_t index) const {
	  return std::string_view(&m_line[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
	}

	std::size_t size() const {
	  return m_data.size() - 1;
	}

	friend std::ostream& operator<<(std::ostream& out, const CSVRow& row) {
	  out << row.m_line << '\n';
	  return out;
	}

	void readNextRow(std::istream& str) {
	  std::getline(str, m_line);

	  m_data.clear();
	  m_data.emplace_back(-1);
	  std::string::size_type pos = 0;
	  while ((pos = m_line.find(',', pos)) != std::string::npos) {
		m_data.emplace_back(pos);
		++pos;
	  }

	  pos = m_line.size();
	  m_data.emplace_back(pos);
	}

  private:
	std::string m_line;
	std::vector<int> m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data) {
  data.readNextRow(str);
  return str;
}   

class CSVIterator {
  public:
	using iterator_category = std::input_iterator_tag;
	using value_type = CSVRow;
	using difference_type = std::size_t;
	using pointer = CSVRow*;
	using reference = CSVRow&;

	CSVIterator(std::istream& str, bool skipHeader) : m_str(str.good() ? &str : NULL) {
	  ++(*this);

	  if (skipHeader) {
		++(*this);
	  }
	}
	CSVIterator() : m_str(NULL) {}

	CSVIterator& operator++() {
	  if (m_str) {
		if (!((*m_str) >> m_row)) { m_str = NULL; }
	  }
	  return *this;
	}

	CSVIterator operator++(int) {
	  CSVIterator tmp(*this);
	  ++(*this);
	  return tmp;
	}

	CSVRow const& operator*() const { return m_row; }
	CSVRow const* operator->() const { return &m_row; }

	bool operator==(CSVIterator const& rhs) { return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL))); }
	bool operator!=(CSVIterator const& rhs) { return !((*this) == rhs); }

  private:
	std::istream* m_str;
	CSVRow m_row;
};

#endif
