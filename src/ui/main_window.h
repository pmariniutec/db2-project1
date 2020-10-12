#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "record_widget.h"
#include "file_organization.h"
#include "indexed_sequential_access.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
	MainWindow();

  private slots:
	void updateActions(const QItemSelection& selection);
	void openFile();
	void saveFile();

  private:
	void createMenus();

	FileOrganization<MockRecord>* fileOrganization;
	RecordWidget* recordWidget;
	QAction* editAct;
	QAction* removeAct;
};

#endif
