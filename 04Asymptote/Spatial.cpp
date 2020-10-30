#include "stdafx.h"
#include "Spatial.h"

Spatial::Spatial()
{
}

SpatialDiff Spatial::operator-(const Spatial& right)
{
    // 当前只计算了水平方位角
    float fDeltaAzimuth = m_fAzimuth + m_fYaw - (right.m_fAzimuth + right.m_fYaw);
    
    SpatialDiff delta;
    delta.fDeltaAzimuth = fDeltaAzimuth;

    return delta;
}
