#include "DrawingLinesHolder.h"

DrawingLinesHolder::DrawingLinesHolder(QOpenGLFunctions_4_5_Core* glFunc) : m_glFunc(glFunc)
, VBOLines(QOpenGLBuffer::VertexBuffer)
, EBOLines(QOpenGLBuffer::IndexBuffer)
, VBOSelectedLines(QOpenGLBuffer::VertexBuffer)
, EBOSelectedLines(QOpenGLBuffer::IndexBuffer)
{
}

DrawingLinesHolder::~DrawingLinesHolder()
{
    //  TODO 释放资源

}

//  TODO 抽取代码，复用
void DrawingLinesHolder::deleteLinesByRemovedLayer(const LayerData& layerData)
{
    //if (lines.size() == 1 && (lines.at(norDefualt).empty() || (*lines.at(norDefualt).begin())->vertex.size() < 2))
    //    return;

    for (auto& pair : lines)
    {
        for (auto itrItem = pair.second.begin(); itrItem != pair.second.end(); /*++itrItem*/)
        {
            auto ptrItem = *itrItem;
            if (ptrItem->layerName == layerData.name)
            {
                itrItem = pair.second.erase(itrItem);
                delete ptrItem;
                continue;
            }

            ++itrItem;
        }
    }
}

void DrawingLinesHolder::deleteSelectedLines()
{
    //if (lines.size() == 1 && (lines.at(norDefualt).empty() || (*lines.at(norDefualt).begin())->vertex.size() < 2))
    //    return;

    map<const CurveData*, bool> mapCurveData;
    for (auto ptrCurve : selectedLines)
    {
        mapCurveData[ptrCurve] = true;
    }

    //  delete lines
    for (auto& pair : lines)
    {
        for (auto itrItem = pair.second.begin(); itrItem != pair.second.end(); /*++itrItem*/)
        {
            auto itrFind = mapCurveData.find(*itrItem);
            if (itrFind != mapCurveData.end())
            {
                itrItem = pair.second.erase(itrItem);
                continue;
            }

            ++itrItem;
        }
    }

    //  delete nodes
    for (auto itrItem = modelNodes.begin(); itrItem != modelNodes.end(); /*++itrItem*/)
    {
        auto itrFind = mapCurveData.find(itrItem->curveId);
        if (itrFind != mapCurveData.end())
        {
            itrItem = modelNodes.erase(itrItem);
            continue;
        }

        ++itrItem;
    }

    for (auto ptrItem : selectedLines)
    {
        delete ptrItem;
    }
    selectedLines.clear();
}

////////// draw lines

void DrawingLinesHolder::BeginToDrawLines(GLCurveType curveType, int drawType/* = -1*/)
{
    auto itrFind = lines.find(norDefualt);
    if (itrFind == lines.end())
        lines.insert(make_pair(norDefualt, vector<CurveData*>()));

    if (lines.at(norDefualt).empty() || !lines.at(norDefualt).back()->vertex.empty())
    {
        lines.at(norDefualt).push_back(new CurveData());
    }
}

void DrawingLinesHolder::GetSnapPoint(Vector3f& result)
{
    //  捕捉吸附
    for (auto& node : modelNodes)
    {
        if (node.point.Distance(result) < 0.2)
        {
            result = node.point;
            break;
        }
    }
}

void DrawingLinesHolder::EndOfDrawingLine()
{
    auto itrFind = lines.find(norDefualt);
    if (itrFind != lines.end() && !itrFind->second.empty() && itrFind->second.back()->vertex.size() < 2)
        itrFind->second.back()->vertex.clear();
}

void DrawingLinesHolder::setCurrentWorkPlaneNormal(const Vector3f& normal)
{
    norDefualt = normal;
}

void DrawingLinesHolder::addLineModelNode(const Line& line, const CurveData* lnkCurve)
{
    modelNodes.push_back(ModelNode(GLNodeType::GLCurveNode0, line.pt0, lnkCurve));
    modelNodes.push_back(ModelNode(GLNodeType::GLCurveNode1, line.pt1, lnkCurve));
    modelNodes.push_back(ModelNode(GLNodeType::GLCurveNodeM, 0.5 * (line.pt0 + line.pt1), lnkCurve));
}

void DrawingLinesHolder::addModelNode(GLCurveType cType, const list<Line>& lstLine, const CurveData* lnkCurve)
{
    //  必须是complete，这里不做校验，由调用者保证
    if (cType == GLCurveType::GLLine)
    {
        addLineModelNode(*lstLine.begin(), lnkCurve);
    }
    else if (cType == GLCurveType::GLArc || cType == GLCurveType::GLCircle)
    {
        //  TODO 这里先硬编码，圆弧离散为11个点，取中间点
        auto itrItem = lstLine.begin();
        for (size_t i = 0; i < 5; i++)
        {
            ++itrItem;
        }

        modelNodes.push_back(ModelNode(GLNodeType::GLCurveNode0, lstLine.begin()->pt0, lnkCurve));
        modelNodes.push_back(ModelNode(GLNodeType::GLCurveNode1, lstLine.back().pt1, lnkCurve));
        modelNodes.push_back(ModelNode(GLNodeType::GLCurveNodeM, itrItem->pt0, lnkCurve));
    }
    else if (cType == GLCurveType::GLRectangle)
    {
        for (auto& ln : lstLine)
        {
            addLineModelNode(ln, lnkCurve);
        }
    }

}

void DrawingLinesHolder::pushCurveData(GLCurveType curveType, const list<Line>& lstLine)
{
    //  必须是complete，这里不做校验，由调用者保证
    if ((curveType == GLCurveType::GLArc
        || curveType == GLCurveType::GLCircle
        || curveType == GLCurveType::GLRectangle)
        && !lines.at(norDefualt).back()->vertex.empty())
    {
        lines.at(norDefualt).push_back(new CurveData());
    }

    auto& target = lines.at(norDefualt).back();
    if (curveType != GLCurveType::GLTemp &&
        curveType != GLCurveType::GLPoint)
    {
        target->curveType = curveType;
        target->layerName = LayerCache::currentLayer;
    }

    for (auto& line : lstLine)
    {
        target->vertex.push_back(line.pt0);
        target->vertex.push_back(line.pt1);
    }

    addModelNode(curveType, lstLine, target);
}

void DrawingLinesHolder::CopySelectedData(const Vector3f& currentWorkPlNormal, const Vector3f& offset)
{
    //  copy
    Transform trs;
    trs.SetTranslate(1000.0 * offset);

    //  lines
    list<Line> selectedLines;
    vector<string> layerNames;
    getSelectedLines(selectedLines, layerNames);

    CGUtils::TransformPolygon(selectedLines, trs);

    //  TODO 暂时只考虑XOY平面上的情况
    updateLinesByInputFileData(layerNames, currentWorkPlNormal, selectedLines);

}

////////// update lines by data of input file
void DrawingLinesHolder::updateLinesByInputFileData(const vector<string>& layerName, const Vector3f& normal, const list<Line>& lstLine)
{
    bool onlyOneLayer = layerName.size() == 1;
    vector<CurveData*> data(lstLine.size());
    int cntIndex = -1;
    list<Line> temp{ Line() };
    for (auto& item : lstLine)
    {
        ++cntIndex;
        Vector3f pt0 = item.pt0 / 1000.0;
        Vector3f pt1 = item.pt1 / 1000.0;

        data[cntIndex] = new CurveData();
        data[cntIndex]->curveType = GLCurveType::GLLine;
        data[cntIndex]->layerName = onlyOneLayer ? layerName[0] : layerName[cntIndex];
        data[cntIndex]->vertex.push_back(pt0);
        data[cntIndex]->vertex.push_back(pt1);

        temp.back().pt0 = pt0;
        temp.back().pt1 = pt1;
        addModelNode(GLCurveType::GLLine, temp, data[cntIndex]);
    }

    PointData_1e_3 key(normal);
    auto itrFind = lines.find(PointData_1e_3(key));
    if (itrFind == lines.end())
        lines.insert(make_pair(key, data));
    else
        itrFind->second.insert(itrFind->second.end(), data.begin(), data.end());

}

void DrawingLinesHolder::getSelectedLines(list<Line>& result, vector<string>& layerNames)
{
    if (selectedLines.empty())
        return;

    for (auto& ls : selectedLines)
    {
        if (ls->vertex.empty())
            continue;

        for (size_t i = 0; i < ls->vertex.size() - 1; i++)
        {
            Vector3f& cur = ls->vertex[i];
            Vector3f& next = ls->vertex[i + 1];

            Line lineItem{ cur * 1000.0, next * 1000.0 };
            result.push_back(lineItem);
            layerNames.push_back(ls->layerName);
        }
    }
}

void DrawingLinesHolder::UpdateSelectInfo(const Body& rectData)
{
    TriangleMesh mesh;
    CGUTILS::CGUtils::TessellateBody(rectData, mesh);

    vector<TriangleData> trisData;
    CGUtils::GetTriDataFromMesh(mesh, trisData);
    PlaneBSPTree* tree = new PlaneBSPTree();
    if (!CGUtils::ConstructBSPTree(trisData, tree))
        throw exception("GetSelectInfo");  //说明数据无效

    //  TODO 暂时只判断线数据
    for (auto& pair : lines)
    {
        for (auto& lineData : pair.second)
        {
            if (lineData->layerName.empty())  //无效图层
                continue;

            bool bSelect = true;
            if (lineData->curveType == GLCurveType::GLLine)
            {
                if (CGUtils::IsPointInBody(lineData->vertex[0] /** 1000.0*/, tree) != -1)
                {
                    bSelect = false;
                    continue;
                }

                for (size_t i = 0; i < lineData->vertex.size() / 2; i++)
                {
                    if (CGUtils::IsPointInBody(lineData->vertex[i * 2 + 1] /** 1000.0*/, tree) != -1)
                    {
                        bSelect = false;
                        break;
                    }
                }
            }
            else if (lineData->curveType == GLCurveType::GLArc
                || lineData->curveType == GLCurveType::GLCircle
                || lineData->curveType == GLCurveType::GLRectangle)
            {
                for (auto& vert : lineData->vertex)
                {
                    if (CGUtils::IsPointInBody(vert /** 1000.0*/, tree) != -1)
                    {
                        bSelect = false;
                        break;
                    }
                }
            }

            if (bSelect)
                selectedLines.push_back(lineData);
        }
    }
}

void DrawingLinesHolder::clearSelected()
{
    selectedLines.clear();
}

void DrawingLinesHolder::DrawSelectedLines(QOpenGLShaderProgram& shader)
{
    selectedLineIndices.clear();
    vector<float> lineVerts;
    updateSelectedLines(lineVerts, selectedLineIndices);
    setupSelectedLines(lineVerts, selectedLineIndices);

    shader.setUniformValue("objectColor", 0.f, 0.f, 1.f, 0.8f);
    m_glFunc->glLineWidth(3.5f);

    QOpenGLVertexArrayObject::Binder vaoBinder(&VAOSelectedLines);

    m_glFunc->glDrawElements(GL_LINES, static_cast<unsigned int>(selectedLineIndices.size()), GL_UNSIGNED_INT, 0);
}

void DrawingLinesHolder::DrawLines(QOpenGLShaderProgram& shader, const PreviewData& previewData)
{
    if (!ViewerSetting::showLines)
        return;

    //  sort by layer
    //  选中的lines回单独进行绘制，这里就跳过
    map<CurveData*, bool> mapCurveData;
    for (auto ptrCurve : selectedLines)
    {
        mapCurveData[ptrCurve] = true;
    }
    map<string, map<PointData_1e_3, vector<CurveData*>>> mapLayer2Lines;
    CGLibUtils::sortLinesByLayer(lines, mapCurveData, map<CurveData*, bool>(), mapLayer2Lines);

    int cntItem = -1;
    for (auto& data : mapLayer2Lines)
    {
        //if (data.first.empty())
        //    continue;

        lineIndices.clear();
        vector<float> lineVerts;
        updateLines(data.second, lineVerts, lineIndices);

        auto& cgColor = (lineIndices.empty() ? LayerCache::getCurrentLayer() : LayerCache::getLayer(data.first)).color;

        if (++cntItem == 0)
            updateLinesPreview(previewData, lineVerts, lineIndices);

        setupLines(lineVerts, lineIndices);

        if (lineIndices.empty())
            continue;

        //shader.setUniformValue("objectColor", 1.f, 0.0f, 0.2f, 0.8f);
        shader.setUniformValue("objectColor", cgColor.X / 255.0, cgColor.Y / 255.0, cgColor.Z / 255.0, 0.8f);
        m_glFunc->glLineWidth(2.0f);

        QOpenGLVertexArrayObject::Binder vaoBinder(&VAOLines);

        m_glFunc->glDrawElements(GL_LINES, static_cast<unsigned int>(lineIndices.size()), GL_UNSIGNED_INT, 0);
    }
}

void DrawingLinesHolder::DrawCopyElementsPreview(QOpenGLShaderProgram& shader)
{
    //  实例化的思路，复用之前的数据，设置对应的transform
    if (ViewerSetting::showLines)
    {
        //  selectedLines
        QVector4D colorEdge(0.0f, 1.0f, 1.0f, 0.8f);
        shader.setUniformValue("objectColor", colorEdge);
        //m_glFunc->glLineWidth(3.5f);

        QOpenGLVertexArrayObject::Binder vaoBinder(&VAOSelectedLines);
        m_glFunc->glDrawElements(GL_LINES, static_cast<unsigned int>(selectedLineIndices.size()), GL_UNSIGNED_INT, 0);
    }
}

void DrawingLinesHolder::updateLinesPreview(const PreviewData& previewData, vector<float>& lineVerts, vector<unsigned int>& lineInds)
{
    if (previewData.previewNextPt == nullptr)
        return;

    //if (lines.empty() || lines.back().size() < 1)
    //if (!isLinePreviewValid())
    //    return;

    list<Line> previewLines;
    previewData.getPreviewLines(previewLines);

    for (auto& line : previewLines)
    {
        lineVerts.push_back(line.pt0.X);
        lineVerts.push_back(line.pt0.Y);
        lineVerts.push_back(line.pt0.Z);
        lineVerts.push_back(previewData.wkPlaneNormal.X);//normal
        lineVerts.push_back(previewData.wkPlaneNormal.Y);
        lineVerts.push_back(previewData.wkPlaneNormal.Z);

        lineVerts.push_back(line.pt1.X);
        lineVerts.push_back(line.pt1.Y);
        lineVerts.push_back(line.pt1.Z);
        lineVerts.push_back(previewData.wkPlaneNormal.X);//normal
        lineVerts.push_back(previewData.wkPlaneNormal.Y);
        lineVerts.push_back(previewData.wkPlaneNormal.Z);

        lineIndices.push_back(lineVerts.size() / 6 - 2);
        lineIndices.push_back(lineVerts.size() / 6 - 1);
    }
}

void DrawingLinesHolder::updateOneLine(const CurveData& ls, vector<float>& lineVerts, vector<unsigned int>& lineInds)
{
    if (ls.vertex.empty())
        return;

    for (size_t i = 0; i < ls.vertex.size() - 1; i++)
    {
        const Vector3f& cur = ls.vertex[i];
        const Vector3f& next = ls.vertex[i + 1];

        lineVerts.push_back(cur.X);
        lineVerts.push_back(cur.Y);
        lineVerts.push_back(cur.Z);
        lineVerts.push_back(0.f);//normal
        lineVerts.push_back(0.f);
        lineVerts.push_back(1.f);

        lineVerts.push_back(next.X);
        lineVerts.push_back(next.Y);
        lineVerts.push_back(next.Z);
        lineVerts.push_back(0.f);//normal
        lineVerts.push_back(0.f);
        lineVerts.push_back(1.f);

        lineInds.push_back(lineVerts.size() / 6 - 2);
        lineInds.push_back(lineVerts.size() / 6 - 1);
    }
}

void DrawingLinesHolder::updateSelectedLines(vector<float>& lineVerts, vector<unsigned int>& lineInds)
{
    if (selectedLines.empty())
        return;

    for (auto& ls : selectedLines)
    {
        updateOneLine(*ls, lineVerts, lineInds);
    }
}

void DrawingLinesHolder::updateLines(const map<PointData_1e_3, vector<CurveData*>>& linesData, vector<float>& lineVerts, vector<unsigned int>& lineInds)
{
    //if (lines.empty() || lines.begin()->size() < 2)
    //if (lines.size() == 1 && (lines.at(norDefualt).empty() || (*lines.at(norDefualt).begin())->vertex.size() < 2))
    //    return;

    //  选中的lines回单独进行绘制，这里就跳过
    map<CurveData*, bool> mapCurveData;
    for (auto ptrCurve : selectedLines)
    {
        mapCurveData[ptrCurve] = true;
    }

    for (auto& pair : linesData/*lines*/)
    {
        for (auto& ls : pair.second)
        {
            if (mapCurveData.find(ls) != mapCurveData.end())
                continue;

            updateOneLine(*ls, lineVerts, lineInds);
        }
    }
}

void DrawingLinesHolder::setupSelectedLines(const vector<float>& lineVerts, const vector<unsigned int>& lineInds)
{
    if (lineVerts.empty() || lineInds.empty())
        return;

    // create buffers/arrays
    if (!VAOSelectedLines.isCreated())
        VAOSelectedLines.create();
    else
        VAOSelectedLines.release();
    QOpenGLVertexArrayObject::Binder vaoBinder(&VAOSelectedLines);
    if (!VBOSelectedLines.isCreated())
        VBOSelectedLines.create();
    else
        VBOSelectedLines.release();
    if (!EBOSelectedLines.isCreated())
        EBOSelectedLines.create();
    else
        EBOSelectedLines.release();

    VBOSelectedLines.bind();
    VBOSelectedLines.allocate(&lineVerts[0], sizeof(float) * lineVerts.size());

    EBOSelectedLines.bind();
    EBOSelectedLines.allocate(&lineInds[0], sizeof(unsigned int) * lineInds.size());

    //m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    //m_shader->enableAttributeArray(0);
    //
    ////  法线
    //m_shader->setAttributeBuffer(1, GL_FLOAT, sizeof(GLfloat) * 3, 3, 6 * sizeof(GLfloat));
    //m_shader->enableAttributeArray(1);


    // set the vertex attribute pointers
    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    // vertex normals
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

}

void DrawingLinesHolder::CopySelectedData(const Vector3f& offset)
{
    //  copy
    Transform trs;
    trs.SetTranslate(1000.0 * offset);

    //  lines
    list<Line> selectedLines;
    vector<string> layerNames;
    getSelectedLines(selectedLines, layerNames);
    CGUtils::TransformPolygon(selectedLines, trs);

    updateLinesByInputFileData(layerNames, ViewerSetting::currentWorkPlNormal, selectedLines);
}

void DrawingLinesHolder::setupLines(const vector<float>& lineVerts, const vector<unsigned int>& lineInds)
{
    if (lineVerts.empty() || lineInds.empty())
        return;

    // create buffers/arrays
    if (!VAOLines.isCreated())
        VAOLines.create();
    else
        VAOLines.release();
    QOpenGLVertexArrayObject::Binder vaoBinder(&VAOLines);
    if (!VBOLines.isCreated())
        VBOLines.create();
    else
        VBOLines.release();
    if (!EBOLines.isCreated())
        EBOLines.create();
    else
        EBOLines.release();

    VBOLines.bind();
    VBOLines.allocate(&lineVerts[0], sizeof(float) * lineVerts.size());

    EBOLines.bind();
    EBOLines.allocate(&lineInds[0], sizeof(unsigned int) * lineInds.size());

    //m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    //m_shader->enableAttributeArray(0);
    //
    ////  法线
    //m_shader->setAttributeBuffer(1, GL_FLOAT, sizeof(GLfloat) * 3, 3, 6 * sizeof(GLfloat));
    //m_shader->enableAttributeArray(1);

    // set the vertex attribute pointers
    // vertex Positions
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    // vertex normals
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));

}