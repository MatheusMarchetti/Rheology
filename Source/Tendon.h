#pragma once

#include "Eigen/Dense"

class Tendon
{
public:
	Tendon(Eigen::Vector2d& position, Eigen::Vector3d& forces, uint32_t initialTime, double area, double fpk_MPa, double Es);

	double CalculateRelaxation(uint32_t currentTime);

	Eigen::Matrix3d GetStiffnessMatrix() const { return m_StiffnessMatrix; }
	uint32_t GetInitialTime() const { return m_InitialTime; }

private:
	void AssembleStiffnessMatrix();

private:
	Eigen::Vector2d m_Position;
	Eigen::Vector3d m_InitialForces;
	Eigen::Matrix3d m_StiffnessMatrix;
	uint32_t m_InitialTime;
	double m_Area;
	double m_Fpk;
	double m_Es;

	//Add information about relaxation
};

