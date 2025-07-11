#include "Setting.h"

float Setting::devicePixelRatio = 1.0;
int Setting::sampleSieOfMSAA = 4;
float Setting::edgeLineWidth = 2.f;
float Setting::edgeLineWidthArray[2]{ 2.f, 1.f };
float Setting::curveWidth = 2.f;
float Setting::curveWidth4Selected = 3.5f;
QVector4D Setting::edgeLineColor = QVector4D(0.f, 1.f, 1.f, 0.8f);
QVector4D Setting::edgeLineColorArray[4]{QVector4D(0.f, 1.f, 1.f, 0.8f),QVector4D(0.f, 0.f, 0.f, 1.f),QVector4D(1.0, 0.0, 1.0, 1.0),QVector4D(1.0, 0.0, 1.0, 1.0)};

float Setting::outlinningWidth = 100.f;

bool Setting::IsMSAAEnable()
{
	return Setting::sampleSieOfMSAA > 0;
}