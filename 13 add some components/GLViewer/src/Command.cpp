#include "Command.h"

string Resources::Msg_PleaseSelectFirstPoint = "指定第一个点";
string Resources::Msg_PleaseSelectFirstCornerPoint = "指定第一个角点";
string Resources::Msg_PleaseInputCoordinateValue = "输入坐标";
string Resources::Msg_PleaseInputOffsetValueBaseCurPoint = "以当前位置为基点，输入偏移";
string Resources::Msg_PleaseSelectNextPoint = "指定下一个点";
string Resources::Msg_PleaseSelectNextCornerPoint = "指定下一个角点";
string Resources::Msg_PleaseInputDistance = "输入距离";
string Resources::Msg_PleaseInputOffset = "输入偏移";
string Resources::Msg_PleaseInputTileLengthAndWidth = "输入瓦片长和宽";
string Resources::Msg_PleaseSelectArc0Point = "请指定圆弧起始点";
string Resources::Msg_PleaseSelectArc1Point = "请指定圆弧终止点";
string Resources::Msg_PleaseSelectOnArcPoint = "请指定弧上点";
string Resources::Msg_PleaseSelectArcCenter = "请指定圆心";
string Resources::Msg_PleasePickSweepAngle = "请点击以确定包含角";
string Resources::Msg_PleasePickTextDirection = "请点击以确定文字方向";
string Resources::Msg_PleasePickTextBasePoint = "请点击以确定文字放置点";
string Resources::Msg_PleaseInputText = "请输入文字";
string Resources::Msg_PleaseInputTextSize = "请输入文字尺寸";
string Resources::Msg_PleasePickRadius = "请点击以确定半径";
string Resources::Msg_PleaseInputSweepAngle = "输入包含角";
string Resources::Msg_PleaseInputRadius = "输入半径";
string Resources::Msg_PleaseInputRotateAngle = "请输入旋转角度";
string Resources::Msg_PleaseSelectGeometry = "请选择对象";
string Resources::Msg_NotSelectedAnyGeometry = "没有选中任何对象";
string Resources::Msg_LayerBelongToCanNotEdit = "所在图层不可编辑";
string Resources::Msg_PleasePickNewInsertPoint = "请点选新的插入点";
string Resources::Msg_PleasePickArcStartAngle = "请点击以确定圆弧起始角";
string Resources::Msg_PleaseInputArcStartAngle = "请输入圆弧起始角";
string Resources::Msg_PleasePickArcEndAngle = "请点击以确定圆弧终止角";
string Resources::Msg_PleaseInputArcEndAngle = "请输入圆弧终止角";
string Resources::Msg_PleaseInputAnotherSemiaxisLength = "请输入另一半轴长度";
string Resources::Msg_PleasePickAnotherSemiaxisLength = "请点击以确定另一半轴长度";
string Resources::Msg_PleasePickAxisAnotherNode = "请点击以确定轴的另一端点";
string Resources::Msg_PleasePickAxisNode = "请点击以确定轴的端点";

//////

ViewerStatus ViewerCommand::m_curStatus = ViewerStatus::Unknown;
CommandNode* ViewerCommand::m_curCommNode = nullptr;
CommandNode* ViewerCommand::m_preCommNode = nullptr;
string ViewerCommand::m_commandStatus = "";

string ViewerCommand::typePt = "Vector3f";
string ViewerCommand::typeVec = "Vector3f";
string ViewerCommand::typeDouble = "double";
string ViewerCommand::typeInt = "int";
string ViewerCommand::typeString = "string";

CommandNode* ViewerCommand::mPointCommand = nullptr;
CommandNode* ViewerCommand::mLineCommand = nullptr;
CommandNode* ViewerCommand::mArcCommand = nullptr;
CommandNode* ViewerCommand::mArc2Command = nullptr;
CommandNode* ViewerCommand::mCircleCommand = nullptr;
CommandNode* ViewerCommand::mRectangleCommand = nullptr;

void ViewerCommand::ActiveCommand(ViewerStatus status, int drawType)
{
    m_curStatus = status;

    int nR = ViewerSetting::IsStatusNeedDraw(m_curStatus);

    if (nR == 1)
        ActiveLineCommand();
    else if (nR == 2)
        drawType == 0 ? ActiveArcCommand() : ActiveArc2Command();
    else if (nR == 3 || nR == 4)
        ActivePointCommand();
    else if (nR == 5)
        ActiveCircleCommand();
    else if (nR == 6)
        ActiveRectangleCommand();
    else
        throw exception("ActiveCommand.");
}

void ViewerCommand::ActiveLineCommand()
{
    if (!mLineCommand)
    {
        vector<CommandNode*> emptyCmds;
        auto node1 = new CommandNode(emptyCmds, Resources::Msg_PleaseSelectNextPoint + "[C]", "C", CommandNode::CommandNodeType::PickPoint);
        auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputDistance + "[D]", "D", CommandNode::CommandNodeType::InputDistanceAlongVector);
        auto node3 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputOffset + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        auto node3i = new CommandNode(emptyCmds, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        vector<CommandNode*> lstNode1{ node1, node2, node3,node3i };

        node1->NextNodes = lstNode1;  //连续绘制
        node2->NextNodes = lstNode1;
        node3->NextNodes = lstNode1;
        node3i->NextNodes = lstNode1;

        auto node4 = new CommandNode(lstNode1, Resources::Msg_PleaseSelectFirstPoint + "[C]", "C", CommandNode::CommandNodeType::PickPoint);
        auto node5 = new CommandNode(lstNode1, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        auto node6 = new CommandNode(lstNode1, Resources::Msg_PleaseInputOffsetValueBaseCurPoint + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        vector<CommandNode*> lstNode2{ node4, node5, node6 };

        mLineCommand = new CommandNode(lstNode2, "DrawLine", "DrawLine", CommandNode::CommandNodeType::Unknown);
        mLineCommand->RootNode = mLineCommand;
        node1->RootNode = mLineCommand;
        node2->RootNode = mLineCommand;
        node3->RootNode = mLineCommand;
        node3i->RootNode = mLineCommand;
        node4->RootNode = mLineCommand;
        node5->RootNode = mLineCommand;
        node6->RootNode = mLineCommand;
    }

    Init(mLineCommand);
}

void ViewerCommand::ActivePointCommand()
{
    if (!mPointCommand)
    {
        vector<CommandNode*> emptyCmds;
        auto node1 = new CommandNode(emptyCmds, Resources::Msg_PleaseSelectNextPoint + "[C]", "C", CommandNode::CommandNodeType::PickPoint);
        //auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputDistance + "[D]", "D", CommandNode::CommandNodeType::InputDistanceAlongVector);
        auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputOffset + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        auto node3 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        vector<CommandNode*> lstNode1{ node1, node2, node3 };

        node1->NextNodes = lstNode1;  //连续绘制
        node2->NextNodes = lstNode1;
        node3->NextNodes = lstNode1;

        auto node4 = new CommandNode(lstNode1, Resources::Msg_PleaseSelectFirstPoint + "[C]", "C", CommandNode::CommandNodeType::PickPoint);
        auto node5 = new CommandNode(lstNode1, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        auto node6 = new CommandNode(lstNode1, Resources::Msg_PleaseInputOffsetValueBaseCurPoint + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        vector<CommandNode*> lstNode2{ node4, node5, node6 };

        mPointCommand = new CommandNode(lstNode2, "DrawPoint", "DrawPoint", CommandNode::CommandNodeType::Unknown);
        mPointCommand->RootNode = mPointCommand;
        node1->RootNode = mPointCommand;
        node2->RootNode = mPointCommand;
        node3->RootNode = mPointCommand;
        node4->RootNode = mPointCommand;
        node5->RootNode = mPointCommand;
        node6->RootNode = mPointCommand;
    }

    Init(mPointCommand);
}

void ViewerCommand::ActiveArcCommand()
{
    if (!mArcCommand)
    {
        vector<CommandNode*> emptyCmds;
        auto node1 = new CommandNode(emptyCmds, Resources::Msg_PleaseSelectOnArcPoint + "[M]", "M", CommandNode::CommandNodeType::PickPoint);
        auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputDistance + "[D]", "D", CommandNode::CommandNodeType::InputDistanceAlongVector);
        auto node3 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputOffset + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        auto node3i = new CommandNode(emptyCmds, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        vector<CommandNode*> lstNode1{ node1, node2, node3, node3i };

        auto node4 = new CommandNode(lstNode1, Resources::Msg_PleaseSelectArc1Point + "[E]", "E", CommandNode::CommandNodeType::PickPoint);
        auto node5 = new CommandNode(lstNode1, Resources::Msg_PleaseInputDistance + "[D]", "D", CommandNode::CommandNodeType::InputDistanceAlongVector);
        auto node6 = new CommandNode(lstNode1, Resources::Msg_PleaseInputOffset + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        auto node6i = new CommandNode(lstNode1, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        vector<CommandNode*> lstNode2{ node4, node5, node6,node6i };

        auto node7 = new CommandNode(lstNode2, Resources::Msg_PleaseSelectArc0Point + "[S]", "S", CommandNode::CommandNodeType::PickPoint);
        auto node8 = new CommandNode(lstNode2, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        auto node9 = new CommandNode(lstNode2, Resources::Msg_PleaseInputOffsetValueBaseCurPoint + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        vector<CommandNode*> lstNode3{ node7, node8, node9 };

        //  连续绘制
        node1->NextNodes = lstNode3;
        node2->NextNodes = lstNode3;
        node3->NextNodes = lstNode3;
        node3i->NextNodes = lstNode3;

        mArcCommand = new CommandNode(lstNode3, "DrawArc", "DrawArc", CommandNode::CommandNodeType::Unknown);
        mArcCommand->RootNode = mArcCommand;
        node1->RootNode = mArcCommand;
        node2->RootNode = mArcCommand;
        node3->RootNode = mArcCommand;
        node3i->RootNode = mArcCommand;
        node4->RootNode = mArcCommand;
        node5->RootNode = mArcCommand;
        node6->RootNode = mArcCommand;
        node6i->RootNode = mArcCommand;
        node7->RootNode = mArcCommand;
        node8->RootNode = mArcCommand;
        node9->RootNode = mArcCommand;
    }

    Init(mArcCommand);
}

void ViewerCommand::ActiveArc2Command()
{
    if (!mArc2Command)
    {
        vector<CommandNode*> emptyCmds;
        auto node1 = new CommandNode(emptyCmds, Resources::Msg_PleasePickSweepAngle + "[S]", "S", CommandNode::CommandNodeType::PickPoint);
        auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputSweepAngle + "[A]", "A", CommandNode::CommandNodeType::InputDoubleValue);
        vector<CommandNode*> lstNode1{ node1, node2 };

        auto node3 = new CommandNode(lstNode1, Resources::Msg_PleaseSelectArc0Point + "[S]", "S", CommandNode::CommandNodeType::PickPoint);
        auto node4 = new CommandNode(lstNode1, Resources::Msg_PleaseInputDistance + "[D]", "D", CommandNode::CommandNodeType::InputDistanceAlongVector);
        auto node5 = new CommandNode(lstNode1, Resources::Msg_PleaseInputOffset + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        auto node5i = new CommandNode(lstNode1, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        vector<CommandNode*> lstNode2{ node3, node4, node5, node5i };

        auto node6 = new CommandNode(lstNode2, Resources::Msg_PleaseSelectArcCenter + "[S]", "S", CommandNode::CommandNodeType::PickPoint);
        auto node7 = new CommandNode(lstNode2, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        auto node8 = new CommandNode(lstNode2, Resources::Msg_PleaseInputOffsetValueBaseCurPoint + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        vector<CommandNode*> lstNode3{ node6, node7, node8 };

        //  连续绘制
        node1->NextNodes = lstNode3;
        node2->NextNodes = lstNode3;

        mArc2Command = new CommandNode(lstNode3, "DrawArc2", "DrawArc2", CommandNode::CommandNodeType::Unknown);
        mArc2Command->RootNode = mArc2Command;
        node1->RootNode = mArc2Command;
        node2->RootNode = mArc2Command;
        node3->RootNode = mArc2Command;
        node4->RootNode = mArc2Command;
        node5->RootNode = mArc2Command;
        node5i->RootNode = mArc2Command;
        node6->RootNode = mArc2Command;
        node7->RootNode = mArc2Command;
    }

    Init(mArc2Command);
}

//  用圆心和半径创建圆步骤流程
void ViewerCommand::ActiveCircleCommand()
{
    if (!mCircleCommand)
    {
        vector<CommandNode*> emptyCmds;
        auto node1 = new CommandNode(emptyCmds, Resources::Msg_PleasePickRadius + "[S]", "S", CommandNode::CommandNodeType::PickPoint);
        auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputRadius + "[R]", "R", CommandNode::CommandNodeType::InputDoubleValue);
        vector<CommandNode*> lstNode1 { node1, node2 };

        auto node3 = new CommandNode(lstNode1, Resources::Msg_PleaseSelectArcCenter + "[S]", "S", CommandNode::CommandNodeType::PickPoint);
        auto node4 = new CommandNode(lstNode1, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        auto node5 = new CommandNode(lstNode1, Resources::Msg_PleaseInputOffsetValueBaseCurPoint + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        vector<CommandNode*> lstNode2 { node3, node4, node5 };

        node1->NextNodes = lstNode2;
        node2->NextNodes = lstNode2;

        mCircleCommand = new CommandNode(lstNode2, "DrawCircle", "DrawCircle", CommandNode::CommandNodeType::Unknown);
        mCircleCommand->RootNode = mCircleCommand;
        node1->RootNode = mCircleCommand;
        node2->RootNode = mCircleCommand;
        node3->RootNode = mCircleCommand;
        node4->RootNode = mCircleCommand;
        node5->RootNode = mCircleCommand;
    }

    Init(mCircleCommand);
}

// 获取矩形多段线步骤
void ViewerCommand::ActiveRectangleCommand()
{
    if (!mRectangleCommand)
    {
        vector<CommandNode*> emptyCmds;
        auto node1 = new CommandNode(emptyCmds, Resources::Msg_PleaseSelectNextCornerPoint + "[C]", "C", CommandNode::CommandNodeType::PickPoint);
        auto node2 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputDistance + "[D]", "D", CommandNode::CommandNodeType::InputDistanceAlongVector);
        auto node3 = new CommandNode(emptyCmds, Resources::Msg_PleaseInputOffset + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        auto node3i = new CommandNode(emptyCmds, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);

        vector<CommandNode*> lstNode1 { node1, node2, node3, node3i };
        auto node4 = new CommandNode(lstNode1, Resources::Msg_PleaseInputRotateAngle + "[R]", "R", CommandNode::CommandNodeType::InputDoubleValue);
        vector<CommandNode*> lstNode2 { node1, node2, node3, node3i, node4 };

        auto node5 = new CommandNode(lstNode2, Resources::Msg_PleaseSelectFirstCornerPoint + "[C]", "C", CommandNode::CommandNodeType::PickPoint);
        auto node6 = new CommandNode(lstNode2, Resources::Msg_PleaseInputCoordinateValue + "[N]", "N", CommandNode::CommandNodeType::InputPointCorrdinateValue);
        auto node7 = new CommandNode(lstNode2, Resources::Msg_PleaseInputOffsetValueBaseCurPoint + "[O]", "O", CommandNode::CommandNodeType::InputOffsetBasePoint);
        vector<CommandNode*> lstNode3 { node5, node6, node7 };

        node1->NextNodes = lstNode3;
        node2->NextNodes = lstNode3;
        node3->NextNodes = lstNode3;
        node3i->NextNodes = lstNode3;

        mRectangleCommand = new CommandNode(lstNode3, "DrawRectangle", "DrawRectangle", CommandNode::CommandNodeType::Unknown);
        mRectangleCommand->RootNode = mRectangleCommand;
        node1->RootNode = mRectangleCommand;
        node2->RootNode = mRectangleCommand;
        node3->RootNode = mRectangleCommand;
        node3i->RootNode = mRectangleCommand;
        node4->RootNode = mRectangleCommand;
        node5->RootNode = mRectangleCommand;
        node6->RootNode = mRectangleCommand;
        node7->RootNode = mRectangleCommand;
    }

    Init(mRectangleCommand);
}

bool ViewerCommand::ConfirmCurrentCommandNode(const string& strText)
{
    //PolylineMode = false;
    if (strText.size() != 1)
    {
        /*if (strText.ToLower() == "cp")
        {
            CallBackChangeCommand(CommandType.CopyGeometrys);
            return true;
        }
        else if (strText.ToLower() == "text")
        {
            CallBackChangeCommand(CommandType.DrawText);
            return true;
        }
        else if (strText.ToLower() == "image")
        {
            CallBackChangeCommand(CommandType.DrawImage);
            return true;
        }
        else if (strText.ToLower() == "st")
        {
            CallBackChangeCommand(CommandType.TextOptionsSetting);
            return true;
        }
        else if (strText.ToLower() == "inter")
        {
            CallBackChangeCommand(CommandType.CalPolygonIntersection);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "union")
        {
            CallBackChangeCommand(CommandType.CalPolygonUnion);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "diff")
        {
            CallBackChangeCommand(CommandType.CalPolygonDifference);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "paving")
        {
            CallBackChangeCommand(CommandType.CalPaving);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "delaunay")
        {
            CallBackChangeCommand(CommandType.Delaunay2d);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "earclipping")
        {
            CallBackChangeCommand(CommandType.EarClipping);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "extractpolygons")
        {
            CallBackChangeCommand(CommandType.ExtractPolygons);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "calconvexhull")
        {
            CallBackChangeCommand(CommandType.CalConvexHull);
            MouseDownInvoke(null);
            return true;
        }
        else if (strText.ToLower() == "convexdeco")
        {
            CallBackChangeCommand(CommandType.ConvexDecomposition);
            MouseDownInvoke(null);
            return true;
        }*/
        //else if (strText.ToLower() == "pl")
        //{
        //   PolylineMode = true;
        //   CallBackChangeCommand(CommandType.DrawLine);
        //   MouseDownInvoke(null);
        //   return true;
        //}

        return false;
    }

    bool bRe = false;
    string strCopy = strText;
    transform(strText.begin(), strText.end(), strCopy.begin(), ::toupper);
    char keyChar = strCopy[0];
    if (m_preCommNode->NextNodes.size() != 1)
    {
        for (auto node : m_preCommNode->NextNodes)
        {
            if (node->Identifying.find(keyChar) != node->Identifying.npos)
            {
                m_curCommNode = node;
                vector<string> strSplit;
                ViewerUtils::Stringsplit(m_curCommNode->CurrentStatus, '[', strSplit);
                m_commandStatus = strSplit[0];
                bRe = true;
                break;
            }
        }
    }

    return bRe;
}

bool ViewerCommand::GetPoint(CommandNode::CommandNodeType nodeType, vector<void*> lstPara, Vector3f& point)
{
    if (lstPara.empty())
        return false;

    switch (nodeType)
    {
    case CommandNode::CommandNodeType::PickPoint:
    {
        if (lstPara.size() != 1)
            return false;

        point = *reinterpret_cast<Vector3f*>(lstPara[0]);
        break;
    }
    case CommandNode::CommandNodeType::InputDistanceAlongVector:
    {
        if (lstPara.size() != 3)
            return false;

        auto ptBase = *reinterpret_cast<Vector3f*>(lstPara[0]);
        auto ptCur = *reinterpret_cast<Vector3f*>(lstPara[1]);
        auto dir = ptCur - ptBase;
        dir.Normalize();
        if (abs(dir.Length() - 1.0) > 1e-5)
            return false;

        point = ptBase + *reinterpret_cast<double*>(lstPara[2]) * dir;
        break;
    }
    case CommandNode::CommandNodeType::InputOffsetBasePoint:
    {
        if (lstPara.size() != 4)
            return false;

        auto ptBase = *reinterpret_cast<Vector3f*>(lstPara[0]);
        point.X = ptBase.X + *reinterpret_cast<double*>(lstPara[1]);
        point.Y = ptBase.Y + *reinterpret_cast<double*>(lstPara[2]);
        point.Z = ptBase.Z + *reinterpret_cast<double*>(lstPara[3]);
        break;
    }
    case CommandNode::CommandNodeType::InputPointCorrdinateValue:
    {
        if (lstPara.size() != 3)
            return false;

        point.X = *reinterpret_cast<double*>(lstPara[0]);
        point.Y = *reinterpret_cast<double*>(lstPara[1]);
        point.Z = *reinterpret_cast<double*>(lstPara[2]);
        //point = PainterDB.MatrixL2W.Inverse * point;

        break;
    }
    }

    return true;
}

bool ViewerCommand::OnKeyEnterDown(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, CommandPara& para)
{
    if (strText.empty())
        return false;

    if (ConfirmCurrentCommandNode(strText))
        return true;

    return ExcuteCommand(strText, point, lastVPoint, false, para);

    //HideCommandParaTextBox();
}

bool ViewerCommand::OnMouseLeftDown(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, CommandPara& para)
{
    return ExcuteCommand(strText, point, lastVPoint, true, para);
}

bool ViewerCommand::ExcuteCommand(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, bool mouseClick, CommandPara& para)
{
    return UpdatePaintOption(strText, point, lastVPoint, mouseClick, para);
}

void ViewerCommand::UpdatePreCommNode(CommandNode* value)
{
    m_preCommNode = value;
    m_curCommNode = m_preCommNode->NextNodes.size() == 0 ? nullptr : m_preCommNode->NextNodes[0];
    m_commandStatus = m_preCommNode->GetNextCommandStatus();
};

bool ViewerCommand::UpdatePaintOption(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, bool mouseClick, CommandPara& para)
{
    //switch (m_curStatus)
    //{
    //case ViewerStatus::DrawingLines:
    //case ViewerStatus::DrawingArc:
    {
        if (!UpdateParameters(m_curCommNode->CommandType, strText, point, lastVPoint, mouseClick, para))
            return false;

        UpdatePreCommNode(m_curCommNode);
        //break;
    }
    //}

    return true;
}

bool ViewerCommand::UpdateParameters(CommandNode::CommandNodeType type, const string& strText, const Vector3f* point, const Vector3f* lastVPoint, bool mouseClick, CommandPara& para)
{
    Vector3f hPoint;

    try
    {
        vector<std::string> arrayPara;
        ViewerUtils::Stringsplit(strText, ',', arrayPara);

        switch (type)
        {
        case CommandNode::CommandNodeType::PickPoint:
        {
            if (!mouseClick)
                return false;

            hPoint = *point;
            para.Init(hPoint);
            //if (!UpdatePaintOptionData(hPoint))
            //    return false;
            break;
        }
        case CommandNode::CommandNodeType::InputDistanceAlongVector:
        {
            auto ptCur = point;
            if (arrayPara.size() != 1)
                return false;

            double dis = std::stod(arrayPara[0]) / 1000.0;
            if (!ViewerCommand::GetPoint(type, vector<void*> { (void*)lastVPoint, (void*)ptCur, (void*)(&dis) }, hPoint))
                return false;
            para.Init(hPoint);
            //if (!UpdatePaintOptionData(hPoint))
            //    return false;
            break;
        }
        case CommandNode::CommandNodeType::InputOffsetBasePoint:
        {
            //Vector3f* ptCur = nullptr;
            //if (m_currentOption.Point0 == null)
            //    ptCur = new HPoint2d(point.X, point.Y);
            //else

            if (arrayPara.size() != 2 && arrayPara.size() != 3)
                return false;

            double dOffX = std::stod(arrayPara[0]) / 1000.0;
            double dOffY = std::stod(arrayPara[1]) / 1000.0;
            double dOffZ = arrayPara.size() == 2 ? 0.0 : std::stod(arrayPara[2]) / 1000.0;
            if (!ViewerCommand::GetPoint(type, vector<void*> { lastVPoint ? (void*)lastVPoint : (void*)point, (void*)(&dOffX), (void*)(&dOffY), (void*)(&dOffZ) }, hPoint))
                return false;
            para.Init(hPoint);
            //if (!UpdatePaintOptionData(hPoint))
            //    return false;
            break;
        }
        case CommandNode::CommandNodeType::InputPointCorrdinateValue:
        {
            if (arrayPara.size() != 2 && arrayPara.size() != 3)
                return false;

            double dX = std::stod(arrayPara[0]) / 1000.0;
            double dY = std::stod(arrayPara[1]) / 1000.0;
            double dZ = arrayPara.size() == 2 ? 0.0 : std::stod(arrayPara[2]) / 1000.0;
            if (!ViewerCommand::GetPoint(type, vector<void*> { (void*)(&dX), (void*)(&dY), (void*)(&dZ) }, hPoint))
                return false;
            para.Init(hPoint);
            //if (!UpdatePaintOptionData(hPoint))
            //    return false;
            break;
        }
        case CommandNode::CommandNodeType::InputDoubleValue:
        {
            if (arrayPara.size() != 1)
                return false;

            double dSweepAng = std::stod(arrayPara[0]);
            //if (!double.TryParse(arrayPara[0], out dSweepAng))
            //    return false;

            para.Init(dSweepAng);
            //if (!UpdatePaintOptionData(dSweepAng))
            //    return false;
            break;
        }
        case CommandNode::CommandNodeType::InputStringValue:
        {
            para.Init(strText);
            //if (!UpdatePaintOptionData(strText))
            //    return false;
            break;
        }
        }
    }
    catch (exception ex)
    {
        return false;
    }

    return true;
}

