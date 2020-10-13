#include <QAction>
#include <QFileDialog>
#include <QMenuBar>

#include "main_window.h"

MainWindow::MainWindow() : QMainWindow(), recordWidget(new RecordWidget) {
  setCentralWidget(recordWidget);
  createMenus();
  setWindowTitle(tr("DBMS"));

  fileOrganization = new IndexedSequentialAccess<MockRecord>("isam");
  std::ifstream file("data/large_data.csv");
  for (CSVIterator rowIter{ file, true }; rowIter != CSVIterator{}; ++rowIter) {
	fileOrganization->insert({ *rowIter });
  }
}

void MainWindow::createMenus() {
  QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

  QAction* openAct = new QAction(tr("&Open..."), this);
  fileMenu->addAction(openAct);
  connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

  QAction* saveAct = new QAction(tr("&Save As..."), this);
  fileMenu->addAction(saveAct);
  connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

  fileMenu->addSeparator();

  QAction* exitAct = new QAction(tr("&Exit"), this);
  fileMenu->addAction(exitAct);
  connect(exitAct, &QAction::triggered, this, &QWidget::close);

  QMenu* toolMenu = menuBar()->addMenu(tr("&Tools"));

  QAction* addAct = new QAction(tr("&Add Entry..."), this);
  toolMenu->addAction(addAct);
  connect(addAct, &QAction::triggered, recordWidget, &RecordWidget::showAddEntryDialog);

  editAct = new QAction(tr("&Edit Entry..."), this);
  editAct->setEnabled(false);
  toolMenu->addAction(editAct);
  connect(editAct, &QAction::triggered, recordWidget, &RecordWidget::editEntry);

  toolMenu->addSeparator();

  removeAct = new QAction(tr("&Remove Entry"), this);
  removeAct->setEnabled(false);
  toolMenu->addAction(removeAct);
  connect(removeAct, &QAction::triggered, recordWidget, &RecordWidget::removeEntry);

  connect(recordWidget, &RecordWidget::selectionChanged, this, &MainWindow::updateActions);
}

void MainWindow::openFile() {
  recordWidget->readFromFile(fileOrganization);
}

void MainWindow::saveFile() {
  recordWidget->writeToFile(fileOrganization);
}

void MainWindow::updateActions(const QItemSelection& selection) {
  QModelIndexList indexes = selection.indexes();

  if (!indexes.isEmpty()) {
	removeAct->setEnabled(true);
	editAct->setEnabled(true);
  } else {
	removeAct->setEnabled(false);
	editAct->setEnabled(false);
  }
}
