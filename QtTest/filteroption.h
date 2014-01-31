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
	QVector<QString> getParameters(void);

	void setName(QString _name);
	QString getName(void);

	void setDescription(QString _description);
	QString getDescription(void);

	void dump(void);

private:
	QString name;
	QString description;
	QVector<QString> parameters;
};

#endif // FILTEROPTION_H