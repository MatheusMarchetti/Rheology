#include "Concrete.h"

#include "Utils.inl"

#include "Output.h"

CementClass CementClassStringToEnum(std::string_view str)
{
	if (str == "325N")	return CementClass::cem_325N;
	if (str == "325R")	return CementClass::cem_325R;
	if (str == "425N")	return CementClass::cem_425N;
	if (str == "425R")	return CementClass::cem_425R;
	if (str == "525N")	return CementClass::cem_525N;

	return CementClass::cem_525R;
}

Concrete::Concrete(const Eigen::Matrix3d& stiffnessMatrix, double fck_MPa, uint32_t concretingDate_Days, double aggregate, double RH, CementClass cement, double notionalSize_Meters)
	: m_StiffnessMatrix(stiffnessMatrix), m_Fck(fck_MPa), m_ConcretingDate(concretingDate_Days), m_AggregateAlphaE(aggregate), m_RH(RH), m_CementClass(cement), m_NotionalSize(notionalSize_Meters), m_CementAlpha(0), m_CementS(0), m_ShrinkageStartDate(3)
{
	const double ONE_THIRD = 1.0 / 3.0;
	m_Fcm = m_Fck + 8;

	m_Eci = 21500 * m_AggregateAlphaE * pow(m_Fcm / 10.0, ONE_THIRD);

	double cementAlphaBS = 0.0;
	double cementAlphaDS1 = 0.0;
	double cementAlphaDS2 = 0.0;

	switch (cement)
	{
		case CementClass::cem_325N:
		{
			m_CementS = 0.38;
			m_CementAlpha = -1;
			cementAlphaBS = 800;
			cementAlphaDS1 = 3;
			cementAlphaDS2 = 0.013;
			break;
		}
		case CementClass::cem_325R:
		case CementClass::cem_425N:
		{
			m_CementS = 0.25;
			m_CementAlpha = 0;
			cementAlphaBS = 700;
			cementAlphaDS1 = 4;
			cementAlphaDS2 = 0.012;
			break;
		}
		case CementClass::cem_425R:
		case CementClass::cem_525N:
		case CementClass::cem_525R:
		{
			m_CementS = 0.20;
			m_CementAlpha = 1;
			cementAlphaBS = 600;
			cementAlphaDS1 = 6;
			cementAlphaDS2 = 0.012;
			break;
		}
	}

	if (m_Fcm > 60.0) m_CementS = 0.2;

	m_BetaBC1 = 1.8 / pow(m_Fcm, 0.7);
	m_BetaDC1 = 412 / pow(m_Fcm, 1.4);
	m_BetaDC2 = (1 - m_RH) / pow(m_NotionalSize, ONE_THIRD);

	double alphafcm = sqrt(35.0 / m_Fcm);

	m_BetaH = 1500 * m_NotionalSize + 250 * alphafcm;
	double betaHMax = 1500.0 * alphafcm;

	if (m_BetaH > betaHMax) m_BetaH = betaHMax;

	m_EpsilonCBS0 = -cementAlphaBS * pow(0.1 * m_Fcm / (6 + 0.1 * m_Fcm), 2.5) * 1.e-06;
	m_EpsilonCDS0 = ((220 + 110 * cementAlphaDS1) * exp(-cementAlphaDS2 * m_Fcm)) * 1.e-06;

	double betaS1 = pow(35.0 / m_Fcm, 0.1);

	if (betaS1 > 1.0) betaS1 = 1.0;

	m_BetaDS1 = 0.25;

	if (m_RH < 0.99 * betaS1)
	{
		m_BetaDS1 = -1.55 * (1 - m_RH * m_RH * m_RH);
	}

	m_NotionalSize *= 1000;
}

double Concrete::CalculateCompliance(double currentTime, double lastTime)
{
	double compliance = 0.0;

	double xg[3] = { -0.7745966692414834, 0.0000000000000000, 0.7745966692414834 };
	double wg[3] = { 0.5555555555555556, 0.8888888888888888, 0.5555555555555556 };

	for (int i = 0; i < 3; i++)
	{
		double tg = 0.5 * (lastTime + currentTime + xg[i] * (currentTime - lastTime));
		compliance += 0.5 * CalculateComplianceFunction(currentTime, tg) * wg[i];
	}

	return compliance;
}

double Concrete::CalculateShrinkage(double currentTime)
{
	double epsilonCS_tts;

	double t = currentTime - m_ConcretingDate;
	double ts = m_ShrinkageStartDate - m_ConcretingDate;

	double tts = t - ts;

	if (tts < 0)
	{
		epsilonCS_tts = 0;
		return epsilonCS_tts;
	}

	double betaBS_t = 1 - exp(-0.2 * sqrt(t));

	double epsilonCBS_t = m_EpsilonCBS0 * betaBS_t;

	double betaDS2_tts = sqrt(tts / (0.035 * m_NotionalSize * m_NotionalSize * 1e6 + tts));

	double epsilonCDS_tts = m_EpsilonCDS0 * m_BetaDS1 * betaDS2_tts;

	epsilonCS_tts = epsilonCBS_t + epsilonCDS_tts;

	return epsilonCDS_tts;
}

double Concrete::CalculateElasticModulusInTime(double time)
{
	if (isEqual(time, 0.0)) return m_Eci;

	double t = time - (double)m_ConcretingDate;
	double tTemp = t; // No temperature adjustment, for now

	double betaBcc_t = exp(m_CementS * (1 - sqrt(28.0 / tTemp)));

	double betaE_t = sqrt(betaBcc_t);

	double eci_t = betaE_t * m_Eci * 1000;

	return eci_t;
}

double Concrete::CalculateCreepCoefficient(double finalTime, double initialTime)
{
	double t = finalTime - (double)m_ConcretingDate;
	double t0 = initialTime - (double)m_ConcretingDate;

	double tt0 = t - t0;
//	if (tt0 < 1) return 0.0;

	double t0Temp = t0; // No temperature adjustment, for now

	double t0TempCem = t0Temp * pow(9. / (2. + pow(t0Temp, 1.2)) + 1., m_CementAlpha);

	if (t0TempCem < 0.5) t0TempCem = 0.5;

	double aux = 30. / t0TempCem + 0.035;
	double betaBC2_tt0 = log(aux * aux * tt0 + 1);

	double phiBC_tt0 = m_BetaBC1 * betaBC2_tt0;

	double betaDC3_t0 = 1 / (0.1 + pow(t0TempCem, 0.2));

	double gama = 1 / (2.3 + 3.5 / sqrt(t0TempCem));

	double betaDC4_tt0 = pow(tt0 / (m_BetaH + tt0), gama);

	double phiDC_tt0 = m_BetaDC1 * m_BetaDC2 * betaDC3_t0 * betaDC4_tt0;

	Output::Write("Creep coefficient: ", phiBC_tt0 + phiDC_tt0);

	return phiBC_tt0 + phiDC_tt0;
}

double Concrete::CalculateComplianceFunction(double finalTime, double initialTime)
{
	double creep = CalculateCreepCoefficient(finalTime, initialTime);
	double eModulus = CalculateElasticModulusInTime(initialTime);

	return 1.0 / eModulus + creep / m_Eci;
}
