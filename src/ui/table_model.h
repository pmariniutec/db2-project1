#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>

#include "mock_record.h"

inline QDataStream& operator<<(QDataStream& stream, const MockRecord& record) {
  return stream << QString{ record.id } << QString{ record.firstName } << QString{ record.lastName } << QString{ record.email } << QString{ record.gender } << QString{ record.ipAddress };
}

inline QDataStream& operator>>(QDataStream& stream, MockRecord& record) {
  QString id, firstName, lastName, email, gender, ipAddress;
  stream >> id >> firstName >> lastName >> email >> gender >> ipAddress;
  std::strncpy(record.id, id.toStdString().c_str(), id.size());
  std::strncpy(record.firstName, firstName.toStdString().c_str(), firstName.size());
  std::strncpy(record.lastName, lastName.toStdString().c_str(), lastName.size());
  std::strncpy(record.email, email.toStdString().c_str(), email.size());
  std::strncpy(record.gender, gender.toStdString().c_str(), gender.size());
  std::strncpy(record.ipAddress, ipAddress.toStdString().c_str(), ipAddress.size());
  return stream;
}

class TableModel : public QAbstractTableModel {
  Q_OBJECT

  public:
	TableModel(QObject* parent = nullptr);
	TableModel(const QVector<MockRecord>& records, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
	const QVector<MockRecord>& getRecords() const;

  private:
	QVector<MockRecord> records;
};

#endif
