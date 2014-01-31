#ifndef FILTEROPTION_H
#define FILTEROPTION_H

#include <QString>
#include <QVector>
#include <QDebug>

class filterOption
{
public:
	filterOption();
	filterOption(QString _name, QString description);
	void addParameter(QString parameter);

	int getNumberParameters(void);
	QString getParameter(int index);

	void setName(QString _name);
	void setDescription(QString _description);

	void dump(void);

private:
	QString name;
	QString description;
	QVector<QString> parameters;
};

#endif // FILTEROPTION_H