#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MViewer.h"

class MViewer : public QMainWindow
{
	Q_OBJECT

public:
	MViewer(QWidget *parent = Q_NULLPTR);

private:
	Ui::MViewerClass ui;
};
