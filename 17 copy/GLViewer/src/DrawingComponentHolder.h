#pragma once
#include <CGLib/TriangleMesh.h>

#include "CGLibUtils.h"
#include "ViewerUtils.h"
#include "ViewerSetting.h"

using namespace std;
using namespace CGUTILS;
class DrawingComponentHolder
{
public:
	DrawingComponentHolder(QOpenGLFunctions_4_5_Core* glFunc);
	~DrawingComponentHolder();

	void setup();
	void draw(QOpenGLShaderProgram& shader);
	void drawSelectionInfo(QOpenGLShaderProgram& shader);
	void drawCopyElementsPreview(QOpenGLShaderProgram& shader);

	//  for select
	void setSelected(int elementId, bool addMode);
	void getSelectedBody(list<pair<int, const BodyInfo*>>& result);
	void clearSelected();

	void copySelectedData(const Vector3f& offset);

private:
	void GenerateLofting(Body& lofting);
	void GenerateBody1(Body& body);
	void GenerateBody2(Body& body);
	void GenerateBody3(Body& body);
	void GenerateBody4(Body& body);
	void GenerateBody5(Body& body);
	void GetCutFace(const Vector3f& origin, Face& cutFace);

	void BooleanOperateUsingCSGOctTree(TriangleMesh& meshA, TriangleMesh& meshB, TriangleMesh& meshInter, TriangleMesh& meshADiffB, TriangleMesh& meshBDiffA, TriangleMesh& meshUnion);

private:
	QOpenGLFunctions_4_5_Core* m_glFunc;

	//  scene
	vector<BodyInfo*> bodys;
	vector<MeshInfo*> meshes;

};