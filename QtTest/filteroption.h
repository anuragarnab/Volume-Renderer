/*
*	filterOption.h
*	Anurag Arnab
*	30 January 2013
*
*	Header of an abstract data type used to describe the filter parameters that are parsed from an xml file
*
*	The class has three member variables: name, description and parameters
*   Name - name of the filter
*   Description - a description of the filter
*   Parameters - a vector containing the names of the various parameters. Can be empty
*
*   There are getters and setters for all the above members
*
*/

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