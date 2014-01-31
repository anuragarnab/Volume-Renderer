/*
*	xmlParser.cpp
*	Anurag Arnab
*	30 January 2013
*
*	Header of class used to parse xml files
*   Parses for the following elements: <name>, <description>, <parameter> and <description> tags nested within <parameter> tags
*
*/


#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "filteroption.h"
#include <QtWidgets>
#include <qxmlstream.h>

class xmlParser
{
public:
	xmlParser();
	xmlParser(QString _filename);
	void parseXml(void);
	void dump(void);

	int getNumberFilters(void);

	QString getName(int index);
	QString getDescription(int index);
	QVector<QString> getParameters(int index);

private:
	QVector <filterOption> options;
	QString xmlFilename;

	void parseOption(QXmlStreamReader &xml, QString pattern, filterOption& fOption);

};

#endif // XMLPARSER_H