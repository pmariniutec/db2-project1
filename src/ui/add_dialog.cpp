#include "add_dialog.h"

#include <QtWidgets>

AddDialog::AddDialog(QWidget* parent) : QDialog(parent), idText(new QLineEdit),
										firstNameText(new QLineEdit),
										lastNameText(new QLineEdit),
										emailText(new QLineEdit),
										genderText(new QLineEdit),
										ipAddressText(new QLineEdit) {

  auto idLabel = new QLabel(tr("ID"));
  auto firstNameLabel = new QLabel(tr("First Name"));
  auto lastNameLabel = new QLabel(tr("Last Name"));
  auto emailLabel = new QLabel(tr("Email"));
  auto genderLabel = new QLabel(tr("Gender"));
  auto ipAddressLabel = new QLabel(tr("Ip Address"));

  auto okButton = new QPushButton(tr("OK"));
  auto cancelButton = new QPushButton(tr("Cancel"));

  auto gLayout = new QGridLayout;
  gLayout->setColumnStretch(1, 2);
  gLayout->addWidget(idLabel, 0, 0);
  gLayout->addWidget(idText, 0, 1);

  gLayout->addWidget(firstNameLabel, 1, 0, Qt::AlignLeft);
  gLayout->addWidget(firstNameText, 1, 1, Qt::AlignLeft);

  gLayout->addWidget(lastNameLabel, 2, 0, Qt::AlignLeft);
  gLayout->addWidget(lastNameText, 2, 1, Qt::AlignLeft);

  gLayout->addWidget(emailLabel, 3, 0, Qt::AlignLeft);
  gLayout->addWidget(emailText, 3, 1, Qt::AlignLeft);

  gLayout->addWidget(genderLabel, 4, 0, Qt::AlignLeft);
  gLayout->addWidget(genderText, 4, 1, Qt::AlignLeft);

  gLayout->addWidget(ipAddressLabel, 5, 0, Qt::AlignLeft);
  gLayout->addWidget(ipAddressText, 5, 1, Qt::AlignLeft);

  auto buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);

  gLayout->addLayout(buttonLayout, 6, 1, Qt::AlignRight);

  auto mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gLayout);
  setLayout(mainLayout);

  connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);
  connect(cancelButton, &QAbstractButton::clicked, this, &QDialog::reject);

  setWindowTitle(tr("Add a Record"));
}

QString AddDialog::id() const {
  return idText->text();
}

QString AddDialog::firstName() const {
  return firstNameText->text();
}

QString AddDialog::lastName() const {
  return lastNameText->text();
}

QString AddDialog::email() const {
  return emailText->text();
}

QString AddDialog::gender() const {
  return genderText->text();
}

QString AddDialog::ipAddress() const {
  return ipAddressText->text();
}

void AddDialog::editAddress(const QString& name, const QString& address) {
  /* TODO
  nameText->setReadOnly(true);
  nameText->setText(name);
  addressText->setPlainText(address);
  */
}
