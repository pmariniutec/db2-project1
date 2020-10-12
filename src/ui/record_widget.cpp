#include <QtWidgets>

#include "record_widget.h"

#include "add_dialog.h"
#include "file_organization.h"

RecordWidget::RecordWidget(QWidget* parent) : QTabWidget(parent), table(new TableModel(this)),
												newRecordTab(new NewRecordTab(this)) {
  connect(newRecordTab, &NewRecordTab::sendDetails, this, &RecordWidget::addEntry);
  addTab(newRecordTab, tr("Address Book"));
  setupTabs();
}

void RecordWidget::showAddEntryDialog() {
  AddDialog aDialog;

  if (aDialog.exec())
	addEntry(aDialog.id(), aDialog.firstName(), aDialog.lastName(), aDialog.email(), aDialog.gender(), aDialog.ipAddress());
}

void RecordWidget::addEntry(const QString& id, const QString& firstName, const QString& lastName, const QString& email, const QString& gender, const QString& ipAddress) {
  if (!table->getRecords().contains({ id.toStdString(), firstName.toStdString(), lastName.toStdString(), email.toStdString(), gender.toStdString(), ipAddress.toStdString() })) {
	table->insertRows(0, 1, QModelIndex());

	QModelIndex index = table->index(0, 0, QModelIndex());
	table->setData(index, id, Qt::EditRole);

	index = table->index(0, 1, QModelIndex());
	table->setData(index, firstName, Qt::EditRole);

	index = table->index(0, 2, QModelIndex());
	table->setData(index, lastName, Qt::EditRole);

	index = table->index(0, 3, QModelIndex());
	table->setData(index, email, Qt::EditRole);

	index = table->index(0, 4, QModelIndex());
	table->setData(index, gender, Qt::EditRole);

	index = table->index(0, 5, QModelIndex());
	table->setData(index, ipAddress, Qt::EditRole);

	removeTab(indexOf(newRecordTab));
  } else {
	QMessageBox::information(this, tr("Duplicate ID"), tr("The id \"%1\" already exists.").arg(id));
  }
}

void RecordWidget::editEntry() {
  /* TODO
  QTableView* temp = static_cast<QTableView*>(currentWidget());
  QSortFilterProxyModel* proxy = static_cast<QSortFilterProxyModel*>(temp->model());
  QItemSelectionModel* selectionModel = temp->selectionModel();

  const QModelIndexList indexes = selectionModel->selectedRows();
  QString id, firstName, lastName, email, gender, ipAddress;
  int row = -1;

  for (const QModelIndex& index : indexes) {
	row = proxy->mapToSource(index).row();
	QModelIndex idIndex = table->index(row, 0, QModelIndex());
	QVariant varName = table->data(idIndex, Qt::DisplayRole);
	id = varName.toString();

	QModelIndex firstNameIndex = table->index(row, 1, QModelIndex());
	QVariant varAddr = table->data(firstNameIndex, Qt::DisplayRole);
	firstName = varAddr.toString();
  }

  AddDialog aDialog;
  aDialog.setWindowTitle(tr("Edit a Record"));
  aDialog.editAddress(name, address);

  if (aDialog.exec()) {
	const QString newAddress = aDialog.address();
	if (newAddress != address) {
	  const QModelIndex index = table->index(row, 1, QModelIndex());
	  table->setData(index, newAddress, Qt::EditRole);
	}
  }
  */
}

void RecordWidget::removeEntry() {
  QTableView* temp = static_cast<QTableView*>(currentWidget());
  QSortFilterProxyModel* proxy = static_cast<QSortFilterProxyModel*>(temp->model());
  QItemSelectionModel* selectionModel = temp->selectionModel();

  const QModelIndexList indexes = selectionModel->selectedRows();

  for (QModelIndex index : indexes) {
	int row = proxy->mapToSource(index).row();
	table->removeRows(row, 1, QModelIndex());
  }

  if (table->rowCount(QModelIndex()) == 0)
	insertTab(0, newRecordTab, tr("Records"));
}

void RecordWidget::setupTabs() {
  const auto groups = { "Records" };

  for (const QString& str : groups) {
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(table);
	proxyModel->setFilterKeyColumn(0);

	QTableView* tableView = new QTableView;
	tableView->setModel(proxyModel);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->verticalHeader()->hide();
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->setSortingEnabled(true);

	connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &RecordWidget::selectionChanged);

	connect(this, &QTabWidget::currentChanged, this, [this, tableView](int tabIndex) {
	  if (widget(tabIndex) == tableView)
		emit selectionChanged(tableView->selectionModel()->selection());
	});

	addTab(tableView, str);
  }
}

void RecordWidget::readFromFile(FileOrganization<MockRecord>* file) {
  auto records = file->scan();

  if (records.empty()) {
	QMessageBox::information(this, tr("No records in file"), tr("The file you are attempting to open contains no records."));
  } else {
	for (const auto& record : records)
	  addEntry(record.id, record.firstName, record.lastName, record.email, record.gender, record.ipAddress);
  }
}

void RecordWidget::writeToFile(FileOrganization<MockRecord>* file) {
  // TODO
}
