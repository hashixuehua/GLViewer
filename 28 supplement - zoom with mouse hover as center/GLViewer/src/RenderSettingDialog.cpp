#include "RenderSettingDialog.h"
#include "ViewerSetting.h"
#include <qboxlayout.h>

RenderSettingDialog::RenderSettingDialog(QWidget* parent): groupAntiAliasingGroup(tr("Setting Of MSAA"), this), rbMode0MSAA(this), rbMode1MSAA(this),
rbMode2MSAA(this), groupShowEdgeLine(tr("Show Edge Line"), this), rbPostProcessingAA4EdgeLine(this),
groupOther(tr("Other"), this), rbScaleByMousePoint(this)
{
	setFixedSize(390, 210);
	setWindowTitle("Render Setting");

	rbMode0MSAA.setText("Normal");
	rbMode1MSAA.setText("Medium");
	rbMode2MSAA.setText("Advanced");

	QHBoxLayout* hboxM = new QHBoxLayout;
	hboxM->addStretch(1);
	hboxM->addWidget(&rbMode0MSAA);
	hboxM->addStretch(2);
	hboxM->addWidget(&rbMode1MSAA);
	hboxM->addStretch(2);
	hboxM->addWidget(&rbMode2MSAA);
	hboxM->addStretch(1);
	groupAntiAliasingGroup.setAlignment(0);
	groupAntiAliasingGroup.setLayout(hboxM);
	groupAntiAliasingGroup.resize(350, 50);
	groupAntiAliasingGroup.move(15, 10);
	groupAntiAliasingGroup.setCheckable(true);
	connect(&groupAntiAliasingGroup, SIGNAL(toggled(bool)), this, SLOT(MSAARadioChanged(bool)));

	connect(&rbMode0MSAA, SIGNAL(clicked(bool)), this, SLOT(MSAARadioChanged(bool)));
	connect(&rbMode1MSAA, SIGNAL(clicked(bool)), this, SLOT(MSAARadioChanged(bool)));
	connect(&rbMode2MSAA, SIGNAL(clicked(bool)), this, SLOT(MSAARadioChanged(bool)));

	//
	QHBoxLayout* hboxE = new QHBoxLayout;
	hboxE->addStretch(1);
	hboxE->addWidget(&rbPostProcessingAA4EdgeLine);
	hboxE->addStretch(1);
	groupShowEdgeLine.setAlignment(0);
	groupShowEdgeLine.setLayout(hboxE);
	groupShowEdgeLine.resize(350, 50);
	groupShowEdgeLine.move(15, 70);
	groupShowEdgeLine.setCheckable(true);
	connect(&groupShowEdgeLine, SIGNAL(toggled(bool)), this, SLOT(EdgeLinePPAAChanged(bool)));

	rbPostProcessingAA4EdgeLine.setText("Post Processing AA for Edgeline");
	connect(&rbPostProcessingAA4EdgeLine, SIGNAL(clicked(bool)), this, SLOT(EdgeLinePPAAChanged(bool)));

	//
	QHBoxLayout* hboxO = new QHBoxLayout;
	hboxO->addStretch(1);
	hboxO->addWidget(&rbScaleByMousePoint);
	hboxO->addStretch(1);
	groupOther.setAlignment(0);
	groupOther.setLayout(hboxO);
	groupOther.resize(350, 50);
	groupOther.move(15, 130);
	groupOther.setCheckable(false);
	//connect(&groupOther, SIGNAL(toggled(bool)), this, SLOT(ScaleByMouseHoverChanged(bool)));

	rbScaleByMousePoint.setText("Scale By Mouse Hover Point");
	connect(&rbScaleByMousePoint, SIGNAL(clicked(bool)), this, SLOT(ScaleByMouseHoverChanged(bool)));

	Init();
}

void RenderSettingDialog::Init()
{
	if (Setting::sampleSieOfMSAA >= 16)
		rbMode2MSAA.setChecked(true);
	else if (Setting::sampleSieOfMSAA <= 4)
		rbMode0MSAA.setChecked(true);
	else
		rbMode1MSAA.setChecked(true);

	groupAntiAliasingGroup.setChecked(Setting::IsMSAAEnable());

	groupShowEdgeLine.setChecked(ViewerSetting::showEdgeLine);
	rbPostProcessingAA4EdgeLine.setChecked(ViewerSetting::edgeLinePPAA);
	rbScaleByMousePoint.setChecked(ViewerSetting::mouseScaleByCenter);
}

void RenderSettingDialog::MSAARadioChanged(bool checked)
{
	int mode = rbMode0MSAA.isChecked() ? 0 : (rbMode1MSAA.isChecked() ? 1 : 2);
	ViewerSetting::OnMSAAModelChanged(groupAntiAliasingGroup.isChecked(), mode);
}

void RenderSettingDialog::EdgeLinePPAAChanged(bool checked)
{
	ViewerSetting::OnEdgeLineModeChanged(groupShowEdgeLine.isChecked(), rbPostProcessingAA4EdgeLine.isChecked());
}

void RenderSettingDialog::ScaleByMouseHoverChanged(bool checked)
{
	ViewerSetting::mouseScaleByCenter = rbScaleByMousePoint.isChecked();
}
