#ifndef NEWRECORDTAB_H
#define NEWRECORDTAB_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QVBoxLayout;
QT_END_NAMESPACE

class NewRecordTab : public QWidget {
  Q_OBJECT

  public:
	NewRecordTab(QWidget* parent = nullptr);

  public slots:
	void addEntry();

  signals:
	void sendDetails(const QString& id, const QString& firstName, const QString& lastName, const QString& email, const QString& gender, const QString& ipAddress);
};

#endif
