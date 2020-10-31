#include "stdafx.h"
#include "Spatial.h"

Spatial::Spatial()
{
}

SpatialDiff Spatial::operator-(const Spatial& right)
{
    SpatialDiff delta;
    // ˮƽ
    delta.fDeltaAzimuth = (m_fAzimuth + m_fYaw) - (right.m_fAzimuth + right.m_fYaw);
    // ��ֱ
    delta.fDeltaPitch = m_fPitch - right.m_fPitch;

    return delta;
}
