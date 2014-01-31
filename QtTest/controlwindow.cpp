#include "controlwindow.h"

ControlWindow::ControlWindow(QWidget *parent)
: QWidget(parent)
{
	uploadOCT = new QCommandLinkButton("Upload OCT");
	renderFilter = new QCommandLinkButton("Render Filter");
	volumeRendering = new QCommandLinkButton("Volume Rendering");
	computeFingerprint = new QCommandLinkButton("Compute 2D-Fingerprint");
	enhanceFingerprint = new QCommandLinkButton("Enhance Fingerprint");

	renderLayout.addWidget(renderFilter);

	mainLayout.addWidget(uploadOCT);
	mainLayout.addLayout(&renderLayout);
	mainLayout.addWidget(volumeRendering);
	mainLayout.addWidget(computeFingerprint);
	mainLayout.addWidget(enhanceFingerprint);

	connect(volumeRendering, SIGNAL(clicked()), this, SLOT(handleVolumeRendering()));
	connect(renderFilter, SIGNAL(clicked()), this, SLOT(handleRenderFilter()));
	connect(uploadOCT, SIGNAL(clicked()), this, SLOT(handleUploadOCT()));
	connect(computeFingerprint, SIGNAL(clicked()), this, SLOT(handleComputeFingerprint()));
	connect(enhanceFingerprint, SIGNAL(clicked()), this, SLOT(handleEnhanceFingerprint()));

	radioButtons = new QRadioButton *[N_FILTERS];
	filterOptions = new QLineEdit *[N_FILTERS];

	initialiseRenderOptions();

	setLayout(&mainLayout);
	setWindowTitle("Control Window");
}

ControlWindow::~ControlWindow()
{

}

void ControlWindow::handleVolumeRendering(void)
{
	qDebug() << "Volume Rendering slot called";
	QString fileName = QFileDialog::getOpenFileName(this, "Specify the config file for the volume renderer", QDir::currentPath());
	emit volRendConfigFile(fileName);

}

void ControlWindow::handleRenderFilter(void)
{

	static int count = 0;

	if (count % 2 == 1)
	{
		++count;
		groupRenderFilter->hide();
		return;
	}

	qDebug() << "Render filter slot called";

	groupRenderFilter->show();
	++count;
}

void ControlWindow::handleUploadOCT(void)
{
	qDebug() << "Upload OCT slot called";
	QString fileName = QFileDialog::getOpenFileName(this, "Specify first image file in sequence", QDir::currentPath());
	emit imageFilename(fileName);
}

void ControlWindow::handleComputeFingerprint(void)
{
	qDebug() << "Compute fingerprint slot called";

}

void ControlWindow::handleEnhanceFingerprint(void)
{
	qDebug() << "Enhance fingerprint slot called";

}

void ControlWindow::initialiseRenderOptions(void)
{
	groupRenderFilter = new QGroupBox("Filters");
	QVBoxLayout *vRadioBox = new QVBoxLayout();

	for (int i = 0; i < N_FILTERS; ++i){
		radioButtons[i] = new QRadioButton(QString("Filter(%1)").arg(i + 1));
		filterOptions[i] = new QLineEdit("Parameters");

		filterOptions[i]->hide();

		connect(radioButtons[i], SIGNAL(toggled(bool)), this, SLOT(radioButtonToggled()));
		connect(filterOptions[i], SIGNAL(textChanged(QString)), this, SLOT(radioButtonToggled()));

		vRadioBox->addWidget(radioButtons[i]);
		vRadioBox->addWidget(filterOptions[i]);
	}

	groupRenderFilter->setLayout(vRadioBox);
	renderLayout.addWidget(groupRenderFilter);

	//    if (N_FILTERS > 0){
	//        radioButtons[0]->setChecked(true);
	//    }

	groupRenderFilter->hide();
}

/*
* Called whenever a radio button is toggled
* Since the radio buttons are mutually exclusive, this function is called twice each time a radio button is pressed
* Once for the radio button being de-selected and once for the button being selected *

* This function is also called when the text is changed
* */
void ControlWindow::radioButtonToggled(void)
{
	qDebug() << "Radio button toggled";

	for (int i = 0; i < N_FILTERS; ++i)
	{
		if (radioButtons[i]->isChecked()){
			filterOptions[i]->show();

			QString text = filterOptions[i]->text();
			bool ok;

			int parameter = text.toInt(&ok);
			if (ok){
				emit imageFilterChosen(i, parameter);
			}

			//emit radioButtonPressed(i, filterOptions[i]->);
		}
		else{
			filterOptions[i]->hide();
		}
	}
}
