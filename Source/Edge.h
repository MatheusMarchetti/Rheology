#pragma once

#include "Eigen/Dense"

class Edge
{
public:
	Edge(Eigen::Vector2d& initialPosition, Eigen::Vector2d& finalPosition);

	double GetLength() const { return m_Length; }
	Eigen::Vector2d GetDirection() const { return m_Direction; }
	Eigen::Vector2d GetPosition(size_t position) const { return m_Positions[position]; }

	Eigen::Vector2d& operator[] (size_t position) { return m_Positions[position]; }

private:
	void CalculateLength();
	void CalculateDirection();

private:
	Eigen::Vector2d m_Positions[2];
	Eigen::Vector2d m_Direction;
	double m_Length;
};

