//TEST(compute):COMPARE_COMPUTE:
//TEST_INPUT:ubuffer(data=[0 0 0 0], stride=4):dxbinding(0),glbinding(0),out

// Access fields of a `struct` type from within a "method" by
// using an explicit `this` expression.

struct A
{
	float x;

	float addWith(float y)
	{
		return this.x + y;
	}
};

RWStructuredBuffer<float> outputBuffer : register(u0);


float test(float inVal)
{
	A a;
	a.x = inVal;
	return a.addWith(inVal*inVal);
}

[numthreads(4, 1, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint tid = dispatchThreadID.x;
	float inVal = float(tid);
	float outVal = test(inVal);
	outputBuffer[tid] = outVal;
}