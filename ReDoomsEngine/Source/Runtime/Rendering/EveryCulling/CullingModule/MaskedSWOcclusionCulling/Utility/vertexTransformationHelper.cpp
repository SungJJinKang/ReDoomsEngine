#include "vertexTransformationHelper.h"

void culling::vertexTransformationHelper::TransformThreeVerticesToClipSpace(culling::EVERYCULLING_M256F* outClipVertexX, culling::EVERYCULLING_M256F* outClipVertexY, culling::EVERYCULLING_M256F* outClipVertexZ, culling::EVERYCULLING_M256F* outClipVertexW, const float* const toClipspaceMatrix)
{
	EA_ASSERT(toClipspaceMatrix != nullptr);
	for (size_t i = 0; i < 3; ++i)
	{
		const culling::EVERYCULLING_M256F tmpX = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[0]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[4]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
		const culling::EVERYCULLING_M256F tmpY = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[1]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[5]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
		const culling::EVERYCULLING_M256F tmpZ = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[2]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[6]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));
		const culling::EVERYCULLING_M256F tmpW = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[3]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[7]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));

		outClipVertexX[i] = tmpX;
		outClipVertexY[i] = tmpY;
		outClipVertexZ[i] = tmpZ;
		outClipVertexW[i] = tmpW;

	}
}

void culling::vertexTransformationHelper::TransformVertexToClipSpace(culling::EVERYCULLING_M256F& outClipVertexX, culling::EVERYCULLING_M256F& outClipVertexY, culling::EVERYCULLING_M256F& outClipVertexZ, culling::EVERYCULLING_M256F& outClipVertexW, const float* const toClipspaceMatrix)
{
	EA_ASSERT(toClipspaceMatrix != nullptr);
	const culling::EVERYCULLING_M256F tmpX = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[0]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[4]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
	const culling::EVERYCULLING_M256F tmpY = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[1]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[5]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
	const culling::EVERYCULLING_M256F tmpZ = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[2]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[6]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));
	const culling::EVERYCULLING_M256F tmpW = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[3]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[7]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[11]), _mm256_set1_ps(toClipspaceMatrix[15]))));

	outClipVertexX = tmpX;
	outClipVertexY = tmpY;
	outClipVertexZ = tmpZ;
	outClipVertexW = tmpW;
}

void culling::vertexTransformationHelper::ConvertClipSpaceThreeVerticesToNDCSpace(culling::EVERYCULLING_M256F* outClipVertexX, culling::EVERYCULLING_M256F* outClipVertexY, culling::EVERYCULLING_M256F* outClipVertexZ, const culling::EVERYCULLING_M256F* oneDividedByW)
{
	for (size_t i = 0; i < 3; i++)
	{
		//Why Do This??
		//compute 1/w in advance 

		outClipVertexX[i] = culling::EVERYCULLING_M256F_MUL(outClipVertexX[i], oneDividedByW[i]);
		outClipVertexY[i] = culling::EVERYCULLING_M256F_MUL(outClipVertexY[i], oneDividedByW[i]);
		outClipVertexZ[i] = culling::EVERYCULLING_M256F_MUL(outClipVertexZ[i], oneDividedByW[i]);

		//This code is useless
		//outClipVertexW[i] = culling::EVERYCULLING_M256F_MUL(outClipVertexW[i], outClipVertexW[i]);
	}
}

void culling::vertexTransformationHelper::ConvertClipSpaceVertexToNDCSpace(culling::EVERYCULLING_M256F& outClipVertexX, culling::EVERYCULLING_M256F& outClipVertexY, culling::EVERYCULLING_M256F& outClipVertexZ, const culling::EVERYCULLING_M256F& oneDividedByW)
{
	outClipVertexX = culling::EVERYCULLING_M256F_MUL(outClipVertexX, oneDividedByW);
	outClipVertexY = culling::EVERYCULLING_M256F_MUL(outClipVertexY, oneDividedByW);
	outClipVertexZ = culling::EVERYCULLING_M256F_MUL(outClipVertexZ, oneDividedByW);
}

void culling::vertexTransformationHelper::TransformVertexToClipSpace(culling::EVERYCULLING_M256F& outClipVertexX, culling::EVERYCULLING_M256F& outClipVertexY, culling::EVERYCULLING_M256F& outClipVertexZ, const float* const toClipspaceMatrix)
{
	EA_ASSERT(toClipspaceMatrix != nullptr);

	const culling::EVERYCULLING_M256F tmpX = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[0]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[4]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
	const culling::EVERYCULLING_M256F tmpY = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[1]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[5]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
	const culling::EVERYCULLING_M256F tmpZ = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX, _mm256_set1_ps(toClipspaceMatrix[2]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY, _mm256_set1_ps(toClipspaceMatrix[6]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ, _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));

	outClipVertexX = tmpX;
	outClipVertexY = tmpY;
	outClipVertexZ = tmpZ;
}

void culling::vertexTransformationHelper::TransformThreeVerticesToClipSpace(culling::EVERYCULLING_M256F* outClipVertexX, culling::EVERYCULLING_M256F* outClipVertexY, culling::EVERYCULLING_M256F* outClipVertexZ, const float* const toClipspaceMatrix)
{
	EA_ASSERT(toClipspaceMatrix != nullptr);
	for (size_t i = 0; i < 3; ++i)
	{
		const culling::EVERYCULLING_M256F tmpX = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[0]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[4]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[8]), _mm256_set1_ps(toClipspaceMatrix[12]))));
		const culling::EVERYCULLING_M256F tmpY = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[1]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[5]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[9]), _mm256_set1_ps(toClipspaceMatrix[13]))));
		const culling::EVERYCULLING_M256F tmpZ = culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexX[i], _mm256_set1_ps(toClipspaceMatrix[2]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexY[i], _mm256_set1_ps(toClipspaceMatrix[6]), culling::EVERYCULLING_M256F_MUL_AND_ADD(outClipVertexZ[i], _mm256_set1_ps(toClipspaceMatrix[10]), _mm256_set1_ps(toClipspaceMatrix[14]))));

		outClipVertexX[i] = tmpX;
		outClipVertexY[i] = tmpY;
		outClipVertexZ[i] = tmpZ;

	}
}
