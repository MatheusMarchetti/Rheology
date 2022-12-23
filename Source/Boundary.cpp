#include "Boundary.h"

#include <iostream>

Boundary::Boundary(std::vector<Edge>& edges)
    : m_Edges(std::move(edges))
{
    AssembleStiffnessMatrix();
}

void Boundary::AssembleStiffnessMatrix()
{
    double A = CalculateA();
    double Sy = CalculateSy();
    double Sz = CalculateSz();
    double Iyy = CalculateIyy();
    double Izz = CalculateIzz();
    double Iyz = CalculateIyz();

    m_StiffnessMatrix <<
         A,  Sy,  Sz,
        Sy, Iyy, Iyz,
        Sz, Iyz, Izz;

    if (A < 0)
    {
        m_NotionalSize = 0.0;
        return;
    }

    double perimeter = 0.0;

    for (auto& edge : m_Edges)
    {
        perimeter += edge.GetLength();
    }

    m_NotionalSize = 2 * A / perimeter;
}

double Boundary::CalculateA()
{
    double result = 0;

    for (auto& edge : m_Edges)
    {
        double x1 = edge[0](0);
        double y1 = edge[0](1);
        double x2 = edge[1](0);
        double y2 = edge[1](1);

        result += (x1 * y2 - x2 * y1);
    }

    return 0.5 * result;
}

double Boundary::CalculateSy()
{
    double result = 0;

    for (auto& edge : m_Edges)
    {
        double x1 = edge[0](0);
        double y1 = edge[0](1);
        double x2 = edge[1](0);
        double y2 = edge[1](1);

        result += (x1 * y2 - x2 * y1) * (x1 + x2);
    }

    return result / 6.0;
}

double Boundary::CalculateSz()
{
    double result = 0;

    for (auto& edge : m_Edges)
    {
        double x1 = edge[0](0);
        double y1 = edge[0](1);
        double x2 = edge[1](0);
        double y2 = edge[1](1);

        result += (x1 * y2 - x2 * y1) * (y1 + y2);
    }

    return result / 6.0;
}

double Boundary::CalculateIyy()
{
    double result = 0;

    for (auto& edge : m_Edges)
    {
        double x1 = edge[0](0);
        double y1 = edge[0](1);
        double x2 = edge[1](0);
        double y2 = edge[1](1);

        result += (x1 * y2 - x2 * y1) * (y1 * y1 + y1 * y2 + y2 * y2);
    }

    return result / 12.0;
}

double Boundary::CalculateIzz()
{
    double result = 0;

    for (auto& edge : m_Edges)
    {
        double x1 = edge[0](0);
        double y1 = edge[0](1);
        double x2 = edge[1](0);
        double y2 = edge[1](1);

        result += (x1 * y2 - x2 * y1) * (x1 * x1 + x1 * x2 + x2 * x2);
    }

    return result / 12.0;
}

double Boundary::CalculateIyz()
{
    double result = 0;

    for (auto& edge : m_Edges)
    {
        double x1 = edge[0](0);
        double y1 = edge[0](1);
        double x2 = edge[1](0);
        double y2 = edge[1](1);

        result += (x1 * y2 - x2 * y1) * (x1 * y2 + 2.0 * x1 * y1 + 2.0 * x2 * y2 + x2 * y1);
    }

    return result / 24.0;
}

