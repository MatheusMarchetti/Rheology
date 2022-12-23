#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "Timer.h"
#include "Boundary.h"
#include "CrossSection.h"
#include "Input.h"
#include "Output.h"

int main()
{
	// Assemble cross section
			// Cross section will be composed of concrete, rebar and tendons
			// Concrete will hold information about the stiffness matrix (geometry + material) and creep/shrinkage
			// Rebar will be a container of position and stiffness matrix
			// Tendon will be a container of position and will store the stiffness matrix and time information. It will also contain data about relaxation and initial forces

	try
	{
		Input input("Data/Input.dat");
		Output output("Data/Output.lis");
		Timer timer;
		CrossSection crossSection;
		int maxTime;

		output.SetFormat<int>("{:2d} ");
		output.SetFormat<double>("{:12.5f} ");

		timer.Start();

		output.WriteComment("Analysis started at ", timer.GetCurrentDate());

		input.GetData(maxTime);
		output.Write("Analysis performed on ", maxTime, " days");
		output.SkipLines();

		// read concrete information
		int concreteCount;
		input.GetData(concreteCount);

		for (int i = 0; i < concreteCount; i++)
		{
			double fck;
			uint32_t concretingDate;
			double aggregateFactor;
			double rh;
			std::string cement;
			input.GetData(fck, concretingDate, aggregateFactor, rh, cement);

			// Read boundary information
			int boundaryCount;

			input.GetData(boundaryCount);

			std::vector<Boundary> boundaryVector;
			boundaryVector.reserve(boundaryCount);

			for (int i = 0; i < boundaryCount; i++)
			{
				int coordinateCount;
				std::vector<Eigen::Vector2d> coordinates;

				input.GetData(coordinateCount);

				coordinates.reserve(coordinateCount);

				for (int j = 0; j < coordinateCount; j++)
				{
					double y, z;

					input.GetData(y, z);

					coordinates.push_back({ y, z });
				}

				std::vector<Edge> edges;
				edges.reserve(coordinates.size());

				for (size_t i = 0; i < edges.capacity(); i++)
				{
					Eigen::Vector2d initialCoordinate = coordinates[i];
					Eigen::Vector2d endCoordinate = coordinates[(i + 1) % coordinateCount];

					edges.push_back({ initialCoordinate, endCoordinate });
				}

				// Create boundary from edges
				Boundary boundary(edges);

				boundaryVector.push_back(boundary);
			}

			// Calculate total stiffness accounting for holes in cross section
			Eigen::Matrix3d stiffnessMatrix;
			stiffnessMatrix.setZero();
			double notionalSize = 0.0;

			for (auto& boundary : boundaryVector)
			{
				stiffnessMatrix += boundary.GetStiffnessMatrix();
				notionalSize += boundary.GetNotionalSize();
			}

			output.Write("Notional size: ", notionalSize, " m");
			output.Write("Stiffness matrix: ", stiffnessMatrix);

			// Store the final concrete cross section
			Concrete concrete(stiffnessMatrix, fck, concretingDate, aggregateFactor, rh, CementClassStringToEnum(cement), notionalSize);

			crossSection.AddConcrete(concrete);
		}

		// Read rebar information and add to cross section
		int rebarCount;
		input.GetData(rebarCount);

		for (int i = 0; i < rebarCount; i++)
		{
			double y, z;
			double rebarArea, fyk, Es;

			input.GetData(y, z, rebarArea, fyk, Es);

			Eigen::Vector2d rebarCoordinate = { y, z };

			Rebar rebar(rebarCoordinate, rebarArea, fyk, Es);

			crossSection.AddRebar(rebar);
		}

		// Read tendon information and add to cross section
		int tendonCount;
		input.GetData(tendonCount);

		for (int i = 0; i < tendonCount; i++)
		{
			int initialTime;
			double y, z;
			double tendonArea;
			double N, My, Mz;
			double fpk, Es;

			input.GetData(y, z, N, My, Mz, initialTime, tendonArea, fpk, Es);

			Eigen::Vector2d tendonCoordinate = { y, z };
			Eigen::Vector3d tendonForces = { N, My, Mz };

			Tendon tendon(tendonCoordinate, tendonForces, initialTime, tendonArea, fpk, Es);

			crossSection.AddTendon(tendon);
		}

		for (uint32_t currentTime = 1; currentTime <= (uint32_t)maxTime; currentTime++)
		{
			crossSection.CalculateTimeEffects(currentTime, Eigen::Vector3d(0.0, 0.0, 0.0));	// TODO: Get forces from input
			crossSection.CalculateStiffnessMatrix(currentTime);
			crossSection.CalculateDeferredStresses(Eigen::Vector3d(0.0, 0.0, 0.0), Eigen::Vector3d(0.0, 0.0, 0.0)); // TODO: Get from previous iteration
		}

		timer.End();

		output.WriteComment("Calculation took ", timer, " ms");
	}
	catch (std::exception& e)
	{
		MessageBox(NULL, e.what(), "Error!", MB_OK);
	}
}