#pragma once
#include <string>
#include <list>
#include <vector>
#include "CGLib/Vector3.h"
#include "ViewerUtils.h"
#include "ViewerSetting.h"

using namespace std;
using namespace CGUTILS;

class CommandNode
{
public:
    enum CommandNodeType
    {
        Unknown,

        //拾取点
        PickPoint,

        //输入点XY坐标
        InputPointCorrdinateValue,

        //输入基于点的偏移（通用：两个double）
        InputOffsetBasePoint,

        //输入沿方向的距离
        InputDistanceAlongVector,

        //输入double值
        InputDoubleValue,

        //输入std::string值
        InputStringValue,

    };

public:
    CommandNode* RootNode;
    vector<CommandNode*> NextNodes;
    std::string CurrentStatus;
    std::string Identifying;
    CommandNodeType CommandType;
    CommandNode(const vector<CommandNode*>& nextNodes, const std::string& status, const std::string& identifying, CommandNodeType nodeType)
    {
        NextNodes = nextNodes;
        CurrentStatus = status;
        Identifying = identifying;
        CommandType = nodeType;
    }

    std::string GetNextCommandStatus()
    {
        if (NextNodes.empty())
            return "";

        //std::string strStatus = NextNodes[0].CurrentStatus.Split('[')[0];
        vector<std::string> splitRe;
        ViewerUtils::Stringsplit(NextNodes[0]->CurrentStatus, '[', splitRe);
        std::string strStatus = splitRe[0];

        strStatus += (NextNodes.size() == 1 ? "" : "或 ");
        for (int cntNode = 1; cntNode < NextNodes.size(); cntNode++)
        {
            strStatus += NextNodes[cntNode]->CurrentStatus + " ";
        }

        return strStatus;
    }

};

class Resources
{
public:
    static std::string Msg_PleaseSelectFirstPoint;
    static std::string Msg_PleaseSelectFirstCornerPoint;
    static std::string Msg_PleaseInputCoordinateValue;
    static std::string Msg_PleaseInputOffsetValueBaseCurPoint;
    static std::string Msg_PleaseSelectNextPoint;
    static std::string Msg_PleaseSelectNextCornerPoint;
    static std::string Msg_PleaseInputDistance;
    static std::string Msg_PleaseInputOffset;
    static std::string Msg_PleaseInputTileLengthAndWidth;
    static std::string Msg_PleaseSelectArc0Point;
    static std::string Msg_PleaseSelectArc1Point;
    static std::string Msg_PleaseSelectOnArcPoint;
    static std::string Msg_PleaseSelectArcCenter;
    static std::string Msg_PleasePickSweepAngle;
    static std::string Msg_PleasePickTextDirection;
    static std::string Msg_PleasePickTextBasePoint;
    static std::string Msg_PleaseInputText;
    static std::string Msg_PleaseInputTextSize;
    static std::string Msg_PleasePickRadius;
    static std::string Msg_PleaseInputSweepAngle;
    static std::string Msg_PleaseInputRadius;
    static std::string Msg_PleaseInputRotateAngle;
    static std::string Msg_PleaseSelectGeometry;
    static std::string Msg_NotSelectedAnyGeometry;
    static std::string Msg_LayerBelongToCanNotEdit;
    static std::string Msg_PleasePickNewInsertPoint;
    static std::string Msg_PleasePickArcStartAngle;
    static std::string Msg_PleaseInputArcStartAngle;
    static std::string Msg_PleasePickArcEndAngle;
    static std::string Msg_PleaseInputArcEndAngle;
    static std::string Msg_PleaseInputAnotherSemiaxisLength;
    static std::string Msg_PleasePickAnotherSemiaxisLength;
    static std::string Msg_PleasePickAxisAnotherNode;
    static std::string Msg_PleasePickAxisNode;
};

class ViewerCommand
{
public:
    static void ActiveCommand(ViewerStatus status, int drawType);

    static bool OnKeyEnterDown(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, CommandPara& para);
    static bool OnMouseLeftDown(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, CommandPara& para);

    static const string& GetCommandStatus()
    {
        return m_commandStatus;
    }

    static void Init()
    {
        m_curStatus = ViewerStatus::Unknown;
        m_curCommNode = nullptr;
        m_preCommNode = nullptr;
        m_commandStatus = "";
    }

private:
    static void ActivePointCommand();

    static void ActiveLineCommand();

    // 获取三点创建圆弧步骤流程
    static void ActiveArcCommand();

    // 获取圆心、起点和包含角方式圆弧步骤流程
    static void ActiveArc2Command();

    static void ActiveCircleCommand();

    static void ActiveRectangleCommand();

    static bool ConfirmCurrentCommandNode(const string& strText);

    static bool GetPoint(CommandNode::CommandNodeType nodeType, vector<void*> lstPara, Vector3f& point);

    static bool ExcuteCommand(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, bool mouseClick, CommandPara& para);

    static bool UpdatePaintOption(const string& strText, const Vector3f* point, const Vector3f* lastVPoint, bool mouseClick, CommandPara& para);

    static bool UpdateParameters(CommandNode::CommandNodeType type, const string& strText, const Vector3f* point, const Vector3f* lastVPoint, bool mouseClick, CommandPara& para);

    static void UpdatePreCommNode(CommandNode* value);

    static void Init(CommandNode* node)
    {
        UpdatePreCommNode(node);
    }
private:
    static ViewerStatus m_curStatus;
    static CommandNode* m_curCommNode;
    static CommandNode* m_preCommNode;
    static string m_commandStatus;

private:
    static string typePt;
    static string typeVec;
    static string typeDouble;
    static string typeInt;
    static string typeString;

    //
    static CommandNode* mPointCommand;
    static CommandNode* mLineCommand;
    static CommandNode* mArcCommand;
    static CommandNode* mArc2Command;
    static CommandNode* mCircleCommand;
    static CommandNode* mRectangleCommand;

};