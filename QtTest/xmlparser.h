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

private:
	QVector <filterOption> options;
	QString xmlFilename;

	void parseOption(QXmlStreamReader &xml);
	void parseOption(QXmlStreamReader &xml, QString pattern, filterOption& fOption);

};

#endif // XMLPARSER_H