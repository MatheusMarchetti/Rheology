#pragma once

#include "Eigen/Dense"

enum class CementClass
{
	cem_325N,
	cem_325R,
	cem_425N,
	cem_425R,
	cem_525N,
	cem_525R,
};

CementClass CementClassStringToEnum(std::string_view str);

class Concrete
{
public:
	Concrete(const Eigen::Matrix3d& stiffnessMatrix, double fck_MPa, uint32_t concretingDate_Days, double aggregate, double RH, CementClass cement, double notionalSize_Meters);

	double CalculateCompliance(double currentTime, double lastTime);
	double CalculateShrinkage(double currentTime);

	Eigen::Matrix3d GetStiffnessMatrix() { return m_StiffnessMatrix; }
	uint32_t GetConcretingDate() { return m_ConcretingDate; }

private:
	double CalculateElasticModulusInTime(double time);
	double CalculateCreepCoefficient(double finalTime, double initialTime);
	double CalculateComplianceFunction(double finalTime, double initialTime);

private:
	double m_Fck;
	uint32_t m_ConcretingDate;
	uint32_t m_ShrinkageStartDate;
	double m_AggregateAlphaE;
	double m_RH;
	CementClass m_CementClass;
	double m_NotionalSize;

	double m_Fcm;
	double m_Eci;
	double m_CementS;
	double m_CementAlpha;
	double m_BetaBC1;
	double m_BetaDC1;
	double m_BetaDC2;
	double m_BetaH;
	double m_EpsilonCBS0;
	double m_EpsilonCDS0;
	double m_BetaDS1;

	Eigen::Matrix3d m_StiffnessMatrix;
};