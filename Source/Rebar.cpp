#include "Rebar.h"

Rebar::Rebar(Eigen::Vector2d& position, double area, double fyk_MPa, double Es)
    : m_Position(position), m_Area(area), m_Fyk(fyk_MPa), m_Es(Es)
{
	AssembleStiffnessMatrix();
}

void Rebar::AssembleStiffnessMatrix()
{
	Eigen::Vector3d position = { 1, m_Position(0), m_Position(1) };

	m_StiffnessMatrix = m_Es * m_Area * position * position.transpose();
}

