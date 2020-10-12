#include <QtWidgets>

#include "new_record_tab.h"
#include "add_dialog.h"

NewRecordTab::NewRecordTab(QWidget* parent) : QWidget(parent) {
  auto descriptionLabel = new QLabel(tr(
		"There are currently no records."
		"\nClick Add to add new records."));

  auto addButton = new QPushButton(tr("Add"));

  connect(addButton, &QAbstractButton::clicked, this, &NewRecordTab::addEntry);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(descriptionLabel);
  mainLayout->addWidget(addButton, 0, Qt::AlignCenter);

  setLayout(mainLayout);
}

void NewRecordTab::addEntry() {
  AddDialog aDialog;

  if (aDialog.exec())
	emit sendDetails(aDialog.id(), aDialog.firstName(), aDialog.lastName(), aDialog.email(), aDialog.gender(), aDialog.ipAddress());
}
