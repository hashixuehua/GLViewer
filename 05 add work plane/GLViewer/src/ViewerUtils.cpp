#include "ViewerUtils.h"

Vector3f ViewerUtils::normalizeToAxis(const Vector3f& dir, const Vector3f& localX, const Vector3f& localY, const Vector3f& localZ)
{
    double angleX = localX.Angle(dir);
    double angleY = localY.Angle(dir);
    double angleZ = localZ.Angle(dir);
    double rAng90 = PI / 2.0;
    bool bTX = angleX > rAng90;
    bool bTY = angleY > rAng90;
    bool bTZ = angleZ > rAng90;
    if (bTX)
        angleX = PI - angleX;
    if (bTY)
        angleY = PI - angleY;
    if (bTZ)
        angleZ = PI - angleZ;

    if (angleX < angleY && angleX < angleZ)
        return bTX ? -localX : localX;
    else if (angleY < angleX && angleY < angleZ)
        return bTY ? -localY : localY;
    else// if (angleZ < angleX && angleZ < angleY)
        return bTZ ? -localZ : localZ;
}
