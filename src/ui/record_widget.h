#ifndef RECORDWIDGET_H
#define RECORDWIDGET_H

#include <QItemSelection>
#include <QTabWidget>

#include "file_organization.h"
#include "new_record_tab.h"
#include "table_model.h"

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QItemSelectionModel;
QT_END_NAMESPACE

class RecordWidget : public QTabWidget {
  Q_OBJECT

  public:
	RecordWidget(QWidget* parent = nullptr);
	void readFromFile(FileOrganization<MockRecord>* file);
	void writeToFile(FileOrganization<MockRecord>* file);

  public slots:
	void showAddEntryDialog();
	void addEntry(const QString& id, const QString& firstName, const QString& lastName, const QString& email, const QString& gender, const QString& ipAddress);
	void editEntry();
	void removeEntry();

  signals:
	void selectionChanged(const QItemSelection& selected);

  private:
	void setupTabs();

	TableModel* table;
	NewRecordTab* newRecordTab;
};

#endif
