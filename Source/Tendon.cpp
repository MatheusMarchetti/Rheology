#include "Tendon.h"

Tendon::Tendon(Eigen::Vector2d& position, Eigen::Vector3d& forces, uint32_t initialTime, double area, double fpk_MPa, double Es)
	: m_Position(position), m_InitialForces(forces), m_InitialTime(initialTime), m_Area(area), m_Fpk(fpk_MPa), m_Es(Es)
{
	AssembleStiffnessMatrix();
}

double Tendon::CalculateRelaxation(uint32_t currentTime)
{
	return 1.0;
}

void Tendon::AssembleStiffnessMatrix()
{
	Eigen::Vector3d position = { 1, m_Position(0), m_Position(1)};

	m_StiffnessMatrix = m_Es * m_Area * position * position.transpose();
}

