#include "xmlparser.h"

xmlParser::xmlParser()
{
}

xmlParser::xmlParser(QString _xmlFile) : xmlFilename(_xmlFile)
{

}

void xmlParser::parseXml(void) {
	/* We'll parse the example.xml */
	QFile* file = new QFile(xmlFilename);
	/* If we can't open it, let's show an error message. */
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Couldn't open " << xmlFilename;
		return;
	}
	/* QXmlStreamReader takes any QIODevice. */
	QXmlStreamReader xml(file);

	/* We'll parse the XML until we reach end of it.*/
	while (!xml.atEnd() &&
		!xml.hasError()) {
		/* Read next element.*/
		QXmlStreamReader::TokenType token = xml.readNext();
		/* If token is just StartDocument, we'll go to next.*/
		if (token == QXmlStreamReader::StartDocument) {
			continue;
		}

		qDebug() << xml.tokenString();
		/* If token is StartElement, we'll see if we can read it.*/
		if (token == QXmlStreamReader::StartElement) {
			/* If it's named options, we'll go to the next.*/
			if (xml.name() == "options") {
				continue;
			}
			/* If it's named option, we'll dig the information from there.*/
			if (xml.name() == "option") {
				parseOption(xml, "option");
			}
		}
	}
	/* Error handling. */
	if (xml.hasError()) {
		qDebug() << "Error while parsing";
	}
	/* Removes any device() or data from the reader
	* and resets its internal state to the initial state. */
	xml.clear();
}

void xmlParser::parseOption(QXmlStreamReader& xml) {
	
	int i = 0;
	/* Let's check that we're really getting a option. */
	if (xml.tokenType() != QXmlStreamReader::StartElement &&
		xml.name() == "option") {
		return;
	}

	filterOption fOption;

	/* Let's get the attributes for person */
	QXmlStreamAttributes attributes = xml.attributes();
	/* Let's check that person has id attribute. */
	if (attributes.hasAttribute("id")) {
		//		/* We'll add it to the map. */
		//		fOption.setName( attributes.value("id").toString() );
		//	}
		/* Next element... */
		qDebug() << ++i << xml.tokenString();
		xml.readNext();
		/*
		* We're going to loop over the things because the order might change.
		* We'll continue the loop until we hit an EndElement named person.
		*/
		while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
			xml.name() == "option")) {
			qDebug() << ++i << "Name: "<< xml.name().toString() << "Text: " << xml.text().toString() << " " << xml.tokenString();


			if (xml.tokenType() == QXmlStreamReader::StartElement) {


				/* We've found first name. */
				if (xml.name() == "name")
				{
					xml.readNext();
					qDebug() << ++i << "Name: " << xml.name().toString() << "Text: " << xml.text().toString() << " " << xml.tokenString();

					if (xml.tokenType() != QXmlStreamReader::Characters) 
					{
						return;
					}
					fOption.setName(xml.text().toString());
				}

				/* We've found description. */
				if (xml.name() == "description") 
				{
					xml.readNext();
					qDebug() << ++i << "Name: " << xml.name().toString() << "Text: " << xml.text().toString() << " " << xml.tokenString();

					if (xml.tokenType() != QXmlStreamReader::Characters) 
					{
						return;
					}
					fOption.setDescription(xml.text().toString());
				}


			}
			xml.readNext();
		}
	}

	options.push_back(fOption);

}


void xmlParser::dump(void)
{
	qDebug() << "Parsed: \n\n";
	QVector<filterOption>::iterator it = options.begin();
	for (; it != options.end(); ++it)
	{
		it->dump();
	}
}

void xmlParser::parseOption(QXmlStreamReader& xml, QString pattern) {

	int i = 0;
	/* Let's check that we're really getting a option. */
	if (xml.tokenType() != QXmlStreamReader::StartElement &&
		xml.name() == pattern) {
		return;
	}

	filterOption fOption;

	/* Let's get the attributes for person */
	QXmlStreamAttributes attributes = xml.attributes();
	/* Let's check that person has id attribute. */
//	if (attributes.hasAttribute("id")) 
//	{
		//		/* We'll add it to the map. */
		//		fOption.setName( attributes.value("id").toString() );
		//	}
		/* Next element... */
		qDebug() << ++i << xml.tokenString();
		xml.readNext();
		/*
		* We're going to loop over the things because the order might change.
		* We'll continue the loop until we hit an EndElement named person.
		*/
		while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
			xml.name() == pattern)) {
			qDebug() << ++i << "Name: " << xml.name().toString() << "Text: " << xml.text().toString() << " " << xml.tokenString();


			if (xml.tokenType() == QXmlStreamReader::StartElement) {


				/* We've found first name. */
				if (xml.name() == "name")
				{
					xml.readNext();
					qDebug() << ++i << "Name: " << xml.name().toString() << "Text: " << xml.text().toString() << " " << xml.tokenString();

					if (xml.tokenType() != QXmlStreamReader::Characters)
					{
						return;
					}
					fOption.setName(xml.text().toString());
				}

				/* We've found description. */
				if (xml.name() == "description")
				{
					xml.readNext();
					qDebug() << ++i << "Name: " << xml.name().toString() << "Text: " << xml.text().toString() << " " << xml.tokenString();

					if (xml.tokenType() != QXmlStreamReader::Characters)
					{
						return;
					}
					fOption.setDescription(xml.text().toString());
				}

				if (xml.name() == "parameters")
				{
					parseOption(xml, "parameters");
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


			}
			xml.readNext();

		}
//	}

	options.push_back(fOption);

}


