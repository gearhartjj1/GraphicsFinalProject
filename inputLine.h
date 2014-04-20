#pragma once

#include <QLineEdit>

class InputLine : public QLineEdit {
	Q_OBJECT
public:
	InputLine(QWidget*);
	~InputLine(void);

public slots:
	void signalFile(void);

signals:
	void sendText(QString);
};