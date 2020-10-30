#include "stdafx.h"
#include "Spatial.h"

Spatial::Spatial()
{
}

SpatialDiff Spatial::operator-(const Spatial& right)
{
    // ��ǰֻ������ˮƽ��λ��
    float fDeltaAzimuth = m_fAzimuth + m_fYaw - (right.m_fAzimuth + right.m_fYaw);
    
    SpatialDiff delta;
    delta.fDeltaAzimuth = fDeltaAzimuth;

    return delta;
}
