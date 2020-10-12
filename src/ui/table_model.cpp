#include "table_model.h"

TableModel::TableModel(QObject* parent) : QAbstractTableModel(parent) { }

TableModel::TableModel(const QVector<MockRecord>& records, QObject* parent)
  : QAbstractTableModel(parent), records(records) { }

int TableModel::rowCount(const QModelIndex& parent) const {
  return parent.isValid() ? 0 : records.size();
}

int TableModel::columnCount(const QModelIndex& parent) const {
  return parent.isValid() ? 0 : 6;
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
	return QVariant();

  if (index.row() >= records.size() || index.row() < 0)
	return QVariant();

  if (role == Qt::DisplayRole) {
	const auto& record = records.at(index.row());

	switch (index.column()) {
	  case 0:
		return record.id;
	  case 1:
		return record.firstName;
	  case 2:
		return record.lastName;
	  case 3:
		return record.email;
	  case 4:
		return record.gender;
	  case 5:
		return record.ipAddress;
	  default:
		break;
	}
  }
  return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
	return QVariant();

  if (orientation == Qt::Horizontal) {
	switch (section) {
	  case 0:
		return tr("ID");
	  case 1:
		return tr("First Name");
	  case 2:
		return tr("Last Name");
	  case 3:
		return tr("Email");
	  case 4:
		return tr("Gender");
	  case 5:
		return tr("Ip Address");
	  default:
		break;
	}
  }
  return QVariant();
}

bool TableModel::insertRows(int position, int rows, const QModelIndex& index) {
  Q_UNUSED(index);
  beginInsertRows(QModelIndex(), position, position + rows - 1);

  for (int row = 0; row < rows; ++row)
	records.insert(position, { "", "", "", "", "", "" });

  endInsertRows();
  return true;
}

bool TableModel::removeRows(int position, int rows, const QModelIndex& index) {
  Q_UNUSED(index);
  beginRemoveRows(QModelIndex(), position, position + rows - 1);

  for (int row = 0; row < rows; ++row)
	records.removeAt(position);

  endRemoveRows();
  return true;
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (index.isValid() && role == Qt::EditRole) {
	const int row = index.row();
	auto record = records.value(row);

	switch (index.column()) {
	  case 0:
		std::strncpy(record.id, value.toString().toStdString().c_str(), sizeof(record.id));
		break;
	  case 1:
		std::strncpy(record.firstName, value.toString().toStdString().c_str(), sizeof(record.firstName));
		break;
	  case 2:
		std::strncpy(record.lastName, value.toString().toStdString().c_str(), sizeof(record.lastName));
		break;
	  case 3:
		std::strncpy(record.email, value.toString().toStdString().c_str(), sizeof(record.email));
		break;
	  case 4:
		std::strncpy(record.gender, value.toString().toStdString().c_str(), sizeof(record.gender));
		break;
	  case 5:
		std::strncpy(record.ipAddress, value.toString().toStdString().c_str(), sizeof(record.ipAddress)); 
		break;
	  default:
		return false;
	}
	records.replace(row, record);
	emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });

	return true;
  }

  return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const {
  if (!index.isValid())
	return Qt::ItemIsEnabled;

  return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

const QVector<MockRecord>& TableModel::getRecords() const {
  return records;
}
