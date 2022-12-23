#pragma once

#include <vector>

#include "Eigen/Dense"

#include "Concrete.h"
#include "Rebar.h"
#include "Tendon.h"

#include "Output.h"

class CrossSection
{
public:
	CrossSection() = default;
	CrossSection(CrossSection& other);
	CrossSection(CrossSection&& other);

	void AddConcrete(Concrete& concrete);
	void AddRebar(Rebar& rebar);
	void AddTendon(Tendon& tendon);

	void CalculateTimeEffects(uint32_t currentTime, const Eigen::Vector3d& forces);
	void CalculateStiffnessMatrix(uint32_t currentTime);
	void CalculateDeferredStresses(const Eigen::Vector3d& forces, const Eigen::Vector3d& strain);

	Eigen::Matrix3d GetStiffnessMatrix() const { return m_StiffnessMatrix; }

private:
	void CalculateConcreteStress(const Eigen::Vector3d& forces, const Eigen::Vector3d& strain);
	void CalculateTendonStrain(const Eigen::Vector3d& forces, const Eigen::Vector3d& strain);

private:
	std::vector<Concrete> m_ConcreteVector;
	std::vector<Rebar> m_SteelReinforcementVector;
	std::vector<Tendon> m_PrestressedSteelVector;

	Eigen::Matrix3d m_StiffnessMatrix;

	std::vector<double> m_ComplianceFunctionVector;
	std::vector<double> m_ShrinkageFunctionVector;
	std::vector<double> m_RelaxationFunctionVector;

	std::vector<Eigen::Vector3d> m_AccumulatedCreepStrain;
	std::vector<Eigen::Vector3d> m_ShrinkageStrain;
};
