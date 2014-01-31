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

void filterOption::dump(void)
{
	qDebug() << "Name: " << name << "\nDescription: " << description;

	for (int i = 0; i < parameters.count(); ++i){
		qDebug() << parameters[i];
	}
	qDebug() << "\n\n";
}

