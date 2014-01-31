/*
*	xmlParser.cpp
*	Anurag Arnab
*	30 January 2013
*
*	Implementation of class used to parse xml files
*   Parses for the following elements: <name>, <description>, <parameter> and <description> tags nested within <parameter> tags
*
*/

#include "xmlparser.h"

xmlParser::xmlParser()
{
}

xmlParser::xmlParser(QString _xmlFile) : xmlFilename(_xmlFile)
{

}

/*
*
* Starts parsing the xml file
* It only parses data that is enclosed with an <option> element
*
*/
void xmlParser::parseXml(void) {

	QFile* file = new QFile(xmlFilename);

	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Couldn't open " << xmlFilename;
		return;
	}

	QXmlStreamReader xml(file);

	while (!xml.atEnd() && !xml.hasError()) {

		QXmlStreamReader::TokenType token = xml.readNext();

		if (token == QXmlStreamReader::StartDocument) {
			continue;
		}

		if (token == QXmlStreamReader::StartElement) {

			if (xml.name() == "options") {
				continue;
			}

			if (xml.name() == "option") {
				filterOption f;
				parseOption(xml, "option", f);
				options.push_back(f);
			}
		}
	}
	/* Error handling. */
	if (xml.hasError()) {
		qDebug() << "Error while parsing xml file";
	}
	/* Removes any device() or data from the reader
	* and resets its internal state to the initial state. */
	xml.clear();
}

/*
*
* Used for debugging. Displays all the parsed data
*
*/
void xmlParser::dump(void)
{
	qDebug() << "Parsed: \n\n";
	QVector<filterOption>::iterator it = options.begin();
	for (; it != options.end(); ++it)
	{
		it->dump();
	}
}

/*
*
* Starts parsing all the data contained in one xml element
* The parameter, pattern, indicates what that element is
* For example, if the pattern is "option", then data between <option> and </option> will be parsed
* The parameter fOption, indicates which filterOption object to store the parsed data in
*
* The function uses if statements to deal with "name", "description" and "parameter" tags since this is what
* we are interested in parsing.
* The function is called recursively to parse the nested data within the parameter tags
*
*/
void xmlParser::parseOption(QXmlStreamReader& xml, QString pattern, filterOption& fOption) {
	
	/* Check that the correct element is received. */
	if (xml.tokenType() != QXmlStreamReader::StartElement &&
		xml.name() == pattern) {
		return;
	}

	QXmlStreamAttributes attributes = xml.attributes();

		xml.readNext();

		while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
			xml.name() == pattern)) {

			if (xml.tokenType() == QXmlStreamReader::StartElement) {


				/* We've found first name. */
				if (xml.name() == "name")
				{
					xml.readNext();

					if (xml.tokenType() != QXmlStreamReader::Characters)
					{
						return;
					}
					fOption.setName(xml.text().toString());
				}

				if (xml.name() == "description" && pattern == "parameters")
				{
					xml.readNext();
					if (xml.tokenType() != QXmlStreamReader::Characters)
					{
						return;
					}
					fOption.addParameter(xml.text().toString());
				}
				/* We've found description. */
				else if (xml.name() == "description")
				{
					xml.readNext();

					if (xml.tokenType() != QXmlStreamReader::Characters)
					{
						return;
					}
					fOption.setDescription(xml.text().toString());
				}

				if (xml.name() == "parameters")
				{
					parseOption(xml, "parameters", fOption);
				}

			}
			xml.readNext();

		}
	}


/*
*
* Returns the number of filters parsed
*
*/
int xmlParser::getNumberFilters(void)
{
	return options.count();
}

/*
*
* Returns the name of a filter at a particular index (ie 0 = first filter etc)
*
*/
QString xmlParser::getName(int index)
{
	return options[index].getName();
}

/*
*
* Returns the description of a filter at a particular index (ie 0 = first filter etc)
*
*/
QString xmlParser::getDescription(int index)
{
	return options[index].getDescription();
}


/*
*
* Returns the parameters of a filter at a particular index (ie 0 = first filter etc)
*
*/
QVector<QString> xmlParser::getParameters(int index)
{
	return options[index].getParameters();
}
