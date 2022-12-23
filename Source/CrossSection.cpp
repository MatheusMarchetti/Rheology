#include "CrossSection.h"

#include <iostream>

CrossSection::CrossSection(CrossSection& other)
    : m_ConcreteVector(other.m_ConcreteVector), m_PrestressedSteelVector(other.m_PrestressedSteelVector), m_SteelReinforcementVector(other.m_SteelReinforcementVector), m_StiffnessMatrix(other.m_StiffnessMatrix)
{
    std::cout << "CrossSection copied" << std::endl;
}

CrossSection::CrossSection(CrossSection&& other)
    : m_ConcreteVector(std::move(other.m_ConcreteVector)), m_PrestressedSteelVector(std::move(other.m_PrestressedSteelVector)), m_SteelReinforcementVector(std::move(other.m_SteelReinforcementVector)), m_StiffnessMatrix(std::move(other.m_StiffnessMatrix))
{
    std::cout << "CrossSection moved" << std::endl;
}
void CrossSection::AddConcrete(Concrete& concrete)
{
    m_ConcreteVector.push_back(std::move(concrete));

    m_ComplianceFunctionVector.resize(m_ConcreteVector.size());
    m_AccumulatedCreepStrain.resize(m_ConcreteVector.size());
    m_AccumulatedCreepStrain.back() = Eigen::Vector3d(0.0, 0.0, 0.0);
    m_ShrinkageFunctionVector.resize(m_ConcreteVector.size());
    m_ShrinkageStrain.resize(m_ConcreteVector.size());
    m_ShrinkageStrain.back() = Eigen::Vector3d(0.0, 0.0, 0.0);
}

void CrossSection::AddRebar(Rebar& rebar)
{
    m_SteelReinforcementVector.push_back(std::move(rebar));
}

void CrossSection::AddTendon(Tendon& tendon)
{
    m_PrestressedSteelVector.push_back(std::move(tendon));

    m_RelaxationFunctionVector.resize(m_PrestressedSteelVector.size());
}

void CrossSection::CalculateTimeEffects(uint32_t currentTime, const Eigen::Vector3d& forces)
{
    for (size_t i = 0; i < m_ConcreteVector.size(); i++)
    {
        m_ComplianceFunctionVector[i] = m_ConcreteVector[i].CalculateCompliance(currentTime, currentTime - 1);
        m_AccumulatedCreepStrain[i] += m_ComplianceFunctionVector[i] * forces;

        m_ShrinkageFunctionVector[i] = m_ConcreteVector[i].CalculateShrinkage(currentTime);
        m_ShrinkageStrain[i] += Eigen::Vector3d(m_ShrinkageFunctionVector[i], 0.0, 0.0);
    }

    for (size_t i = 0; i < m_PrestressedSteelVector.size(); i++)
    {
        m_RelaxationFunctionVector[i] = m_PrestressedSteelVector[i].CalculateRelaxation(currentTime);
    }
}

void CrossSection::CalculateStiffnessMatrix(uint32_t currentTime)
{
    m_StiffnessMatrix.setZero();

    for (size_t i = 0; i < m_ConcreteVector.size(); i++)
    {
        if (currentTime >= m_ConcreteVector[i].GetConcretingDate())
        {
            m_StiffnessMatrix += (1.0 / m_ComplianceFunctionVector[i]) * m_ConcreteVector[i].GetStiffnessMatrix();
        }
    }

    for (auto& rebar : m_SteelReinforcementVector)
    {
        m_StiffnessMatrix += rebar.GetStiffnessMatrix();
    }

    for (size_t i = 0; i < m_PrestressedSteelVector.size(); i++)
    {
        if (currentTime >= m_PrestressedSteelVector[i].GetInitialTime())
        {
            m_StiffnessMatrix += m_RelaxationFunctionVector[i] * m_PrestressedSteelVector[i].GetStiffnessMatrix();
        }
    }

//    std::cout << m_StiffnessMatrix << std::endl << std::endl;
}

void CrossSection::CalculateDeferredStresses(const Eigen::Vector3d& forces, const Eigen::Vector3d& strain)
{
    CalculateConcreteStress(forces, strain);
    CalculateTendonStrain(forces, strain);
}

void CrossSection::CalculateConcreteStress(const Eigen::Vector3d& forces, const Eigen::Vector3d& strain)
{
    std::vector<Eigen::Vector3d> stressVector(m_ConcreteVector.size());

    for (size_t i = 0; i < m_ConcreteVector.size(); i++)
    {
        Eigen::Matrix3d D = (1.0 / m_ComplianceFunctionVector[i]) * m_ConcreteVector[i].GetStiffnessMatrix();
        stressVector[i] = forces + D * (m_AccumulatedCreepStrain[i] + m_ConcreteVector[i].GetStiffnessMatrix() * (m_ShrinkageStrain[i] - strain));
    }
}

void CrossSection::CalculateTendonStrain(const Eigen::Vector3d& forces, const Eigen::Vector3d& strain)
{
}
