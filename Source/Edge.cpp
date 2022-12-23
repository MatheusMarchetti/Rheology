#include "Edge.h"

Edge::Edge(Eigen::Vector2d& initialPosition, Eigen::Vector2d& finalPosition)
{
	m_Positions[0] = initialPosition;
	m_Positions[1] = finalPosition;

	CalculateLength();
	CalculateDirection();
}

void Edge::CalculateLength()
{
	m_Length = sqrt(pow(m_Positions[1](0) - m_Positions[0](0), 2) + pow(m_Positions[1](1) - m_Positions[0](1), 2));
}
void Edge::CalculateDirection()
{
	m_Direction = m_Positions[1] - m_Positions[0];
	m_Direction /= m_Length;
}

