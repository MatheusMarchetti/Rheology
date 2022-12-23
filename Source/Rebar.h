#pragma once

#include "Eigen/Dense"

class Rebar
{
public:
	Rebar(Eigen::Vector2d& position, double area, double fyk_MPa, double Es);

	Eigen::Matrix3d GetStiffnessMatrix() const { return m_StiffnessMatrix; }

private:
	void AssembleStiffnessMatrix();

private:
	Eigen::Vector2d m_Position;
	Eigen::Matrix3d m_StiffnessMatrix;

	double m_Area;
	double m_Fyk;
	double m_Es;
};

