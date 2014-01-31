/*
*	filterOption.cpp
*	Anurag Arnab
*	30 January 2013
*
*	Implemenation of an abstract data type used to describe the filter parameters that are parsed from an xml file
*
*	The class has three member variables: name, description and parameters
*   Name - name of the filter
*   Description - a description of the filter
*   Parameters - a vector containing the names of the various parameters. Can be empty
*
*   There are getters and setters for all the above members
*
*/

#include "filteroption.h"

filterOption::filterOption()
{
}

filterOption::filterOption(QString _name, QString _description) :
name(_name), description(_description)
{

}


void filterOption::addParameter(QString parameter)
{
	parameters.push_back(parameter);
}

int filterOption::getNumberParameters(void){
	return parameters.count();
}

QString filterOption::getParameter(int index){
	return parameters[index];
}

void filterOption::setDescription(QString _description)
{
	description = _description;
}


void filterOption::setName(QString _name)
{
	name = _name;
}

/*
* Displays content of object. Used for debugging 
*/
void filterOption::dump(void)
{
	qDebug() << "Name: " << name << "\nDescription: " << description;

	for (int i = 0; i < parameters.count(); ++i){
		qDebug() << parameters[i];
	}
	qDebug() << "\n\n";
}

QString filterOption::getDescription(void)
{
	return description;
}

QString filterOption::getName(void)
{
	return name;
}

QVector<QString> filterOption::getParameters(void)
{
	return parameters;
}