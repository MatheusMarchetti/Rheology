#pragma once

#include <vector>

#include "Eigen/Dense"

#include "Edge.h"

class Boundary
{
public:
	Boundary(std::vector<Edge>& edges);

	Eigen::Matrix3d GetStiffnessMatrix() const { return m_StiffnessMatrix; }
	double GetNotionalSize() const { return m_NotionalSize; }

private:
	void AssembleStiffnessMatrix();
	double CalculateA();
	double CalculateSy();
	double CalculateSz();
	double CalculateIyy();
	double CalculateIzz();
	double CalculateIyz();

private:
	std::vector<Edge> m_Edges;
	Eigen::Matrix3d m_StiffnessMatrix;

	double m_NotionalSize;
};

