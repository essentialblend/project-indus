#pragma once

class Camera
{
public: 
	Camera() {};

	Camera(double aspRatio, unsigned short imgW, const Vec3& focalLength, const PointVec3& cameraCenter, const std::vector<Vec3>& pixelBuffer, bool useMT, int jitterSamples, int maxDepth) : aspectRatio(aspRatio), imageWidthPixels(imgW), focalLength(focalLength), cameraCenter(cameraCenter), pixelBuffer(pixelBuffer), useMT(useMT), jitterSamplesAA(jitterSamples), maxRayBouncesDepth(maxDepth) {}

	unsigned short getResolutionWidthPixels() const 
	{
		return imageWidthPixels;
	}

	unsigned short getResolutionHeightPixels() const
	{
		return imageHeightPixels;
	}

	const std::vector<Vec3>& getPixelBuffer() const
	{
		return pixelBuffer;
	}

	void renderFrame(const WorldObject& mainWorld)
	{
		initializeCamera();

		int numAvailableThreads{ static_cast<int>(std::thread::hardware_concurrency()) };
		int rowsPerThread = imageHeightPixels / numAvailableThreads;

		auto logTimeStart = std::chrono::high_resolution_clock::now();
		if (useMT)
		{
			auto coutString = std::format("Starting multi-threaded rendering with {} threads...", numAvailableThreads);
			UWriteToCout(coutString);
			std::vector<std::thread> mainThreads;

			for (unsigned short t = 0; t < numAvailableThreads; ++t)
			{
				int startRow = t * rowsPerThread;
				int endRow = (t + 1) * rowsPerThread;

				// Cover remaining rows with the last thread.
				if (t == numAvailableThreads - 1)
				{
					endRow = imageHeightPixels;
				}

				coutString = std::format("Thread {} processing pixel rows {} to {}...", t, startRow, endRow);
				UWriteToCout(coutString);

				mainThreads.emplace_back(&Camera::renderRowSegments, this, startRow, endRow, std::cref(mainWorld));
			}

			// Wait for all threads to finish and log their completion.
			for (unsigned short t = 0; t < mainThreads.size(); ++t)
			{
				mainThreads[t].join();
				coutString = std::format("Thread {} finished...", t);
				UWriteToCout(coutString);
			}
		}
		else
		{
			renderRowSegments(0, imageHeightPixels, mainWorld);
		}

		// Write to image file.
		std::ofstream outPPM("image.ppm");
		if (!outPPM)
		{
			std::cerr << "Error: Could not open output image." << std::endl;
			return;
		}
		outPPM << "P3\n" << imageWidthPixels << " " << imageHeightPixels << "\n255\n";
		for (const auto& pixel : pixelBuffer)
		{
			outPPM << pixel.getX() << " " << pixel.getY() << " " << pixel.getZ() << "\n";
		}
		outPPM.close();
		auto logTimeEnd = std::chrono::high_resolution_clock::now();
		UPrintSuccessLog(logTimeStart, logTimeEnd, imageWidthPixels * imageHeightPixels, jitterSamplesAA, useMT, maxRayBouncesDepth);

	}



private:
	// Camera and viewport variables.
	double aspectRatio{ 0 };
	unsigned short imageWidthPixels{ 0 };
	unsigned short imageHeightPixels{ 0 };
	Vec3 focalLength{ Vec3() };
	PointVec3 cameraCenter{ PointVec3() };
	std::vector<Vec3> pixelBuffer{ 0 };
	bool useMT{ false };
	PointVec3 topLeftPixelLocation{ Vec3() };
	Vec3 viewportDeltaX{ Vec3() };
	Vec3 viewportDeltaY{ Vec3() };
	int jitterSamplesAA{ 0 };
	int jitterSqrt{ 0 };
	int maxRayBouncesDepth{ 0 };

	void initializeCamera()
	{
		imageHeightPixels = static_cast<unsigned short>(imageWidthPixels / aspectRatio);
		imageHeightPixels = imageHeightPixels < 1 ? 1 : imageHeightPixels;
		pixelBuffer.resize(static_cast<unsigned long long>(imageWidthPixels * imageHeightPixels));
		float viewportHeight{ 2.f };
		float viewportWidth{ viewportHeight * (static_cast<float>(imageWidthPixels) / imageHeightPixels) };
		jitterSqrt = static_cast<int>(std::sqrt(jitterSamplesAA));

		Vec3 viewportUX{ Vec3(viewportWidth, 0, 0) };
		Vec3 viewportVY{ Vec3(0, -viewportHeight, 0) };

		viewportDeltaX = viewportUX / imageWidthPixels;
		viewportDeltaY = viewportVY / imageHeightPixels;

		PointVec3 viewportUpperLeftPoint = cameraCenter - focalLength - (viewportUX / 2) - (viewportVY / 2);
		topLeftPixelLocation = viewportUpperLeftPoint + (0.5 * (viewportDeltaX + viewportDeltaY));
	}

	ColorVec3 computePixelColor(const Ray& inputRay, int bounceDepthParam, const WorldObject& mainWorld) const
	{
		HitRecord tempRec;

		if (bounceDepthParam <= 0)
		{
			return ColorVec3(0.f);
		}

		if (mainWorld.checkHit(inputRay, Interval(0.001, +Uinf), tempRec))
		{
			Ray scatteredRay;
			ColorVec3 attenuationValue;

			if (tempRec.hitMaterial->handleRayScatter(inputRay, scatteredRay, tempRec, attenuationValue))
			{
				return attenuationValue * computePixelColor(scatteredRay, bounceDepthParam - 1, mainWorld);
			}
			return ColorVec3(0.f);
		}

		Vec3 unitDirection = computeUnitVector(inputRay.getRayDirection());
		double lerpFactor = 0.5 * (unitDirection.getY() + 1.f);
		return ((1 - lerpFactor) * ColorVec3(1.f)) + (lerpFactor * ColorVec3(0.5, 0.7, 1.0));
	}

	// Render chunks if MT, else treat it as a typical render function for the entire screen for ST.
	void renderRowSegments(int startRow, int endRow, const WorldObject& mainWorld)
	{
		for (int j = startRow; j < endRow; ++j)
		{
			for (int i = 0; i < imageWidthPixels; ++i)
			{
				ColorVec3 pixelColor(0.f);

				// Accumulate color from jittered samples (AA).
				for (int pixelY{ 0 }; pixelY < jitterSqrt; ++pixelY)
				{
					for (int pixelX{ 0 }; pixelX < jitterSqrt; ++pixelX)
					{
						double offsetX{ (pixelX + UGenRNGDouble()) / jitterSqrt };
						double offsetY{ (pixelY + UGenRNGDouble()) / jitterSqrt };

						PointVec3 samplePoint{ topLeftPixelLocation + ((i + offsetX) * viewportDeltaX) + ((j + offsetY) * viewportDeltaY) };
						Vec3 rayDirection = samplePoint - cameraCenter;
						Ray primaryRay(cameraCenter, rayDirection);
						pixelColor += computePixelColor(primaryRay, maxRayBouncesDepth, mainWorld);
					}
				}
				pixelColor /= jitterSamplesAA;
				Vec3 gammaCorrected{ linearToGamma(pixelColor) };

				int bufferIndex = j * imageWidthPixels + i;
				pixelBuffer[bufferIndex] = ColorVec3(static_cast<int>(256 * std::clamp(gammaCorrected.getX(), 0.000, 0.999)), static_cast<int>(256 * std::clamp(gammaCorrected.getY(), 0.000, 0.999)), static_cast<int>(256 * std::clamp(gammaCorrected.getZ(), 0.000, 0.999)));
			}
		}
	}

	Vec3 linearToGamma(const Vec3& linearColorComp) const
	{
		return Vec3(std::pow(linearColorComp.getX(), 1 / 2.2f), std::pow(linearColorComp.getY(), 1 / 2.2f), std::pow(linearColorComp.getZ(), 1 / 2.2f));
	}
};