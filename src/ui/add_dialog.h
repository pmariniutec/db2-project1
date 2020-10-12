#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTextEdit;
class QLineEdit;
QT_END_NAMESPACE

class AddDialog : public QDialog {
  Q_OBJECT

  public:
	AddDialog(QWidget* parent = nullptr);

	QString id() const;
	QString firstName() const;
	QString lastName() const;
	QString email() const;
	QString gender() const;
	QString ipAddress() const;
	void editAddress(const QString& name, const QString& address);

  private:
	QLineEdit* idText;
	QLineEdit* firstNameText;
	QLineEdit* lastNameText;
	QLineEdit* emailText;
	QLineEdit* genderText;
	QLineEdit* ipAddressText;
};

#endif
