#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#define N_FILTERS 4

#include <QMainWindow>
//#include <QtGui> // Qt4
#include <QtWidgets> //Qt5

class ControlWindow : public QWidget
{
	Q_OBJECT

public:
	ControlWindow(QWidget *parent = 0);
	~ControlWindow();

private:

	QCommandLinkButton * uploadOCT;
	QCommandLinkButton * renderFilter;
	QCommandLinkButton * volumeRendering;
	QCommandLinkButton * computeFingerprint;
	QCommandLinkButton * enhanceFingerprint;

	QGroupBox * groupRenderFilter;

	QVBoxLayout mainLayout;
	QVBoxLayout renderLayout;

	QRadioButton ** radioButtons;
	QLineEdit ** filterOptions;

	void initialiseRenderOptions(void);

private slots:
	void handleVolumeRendering(void);
	void handleRenderFilter(void);
	void handleUploadOCT(void);
	void handleComputeFingerprint(void);
	void handleEnhanceFingerprint(void);

	void radioButtonToggled(void);

signals:
	void imageFilename(QString filname);
	void volRendConfigFile(QString filename);

};

#endif // CONTROLWINDOW_H
