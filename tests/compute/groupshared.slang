// groupshared.slang

//TEST(compute):COMPARE_COMPUTE_EX:-slang -compute
//TEST(compute):COMPARE_COMPUTE_EX:-slang -compute -dx12
//TEST(compute, vulkan):COMPARE_COMPUTE_EX:-vk -compute

//TEST_INPUT:ubuffer(data=[0 0 0 0], stride=4):dxbinding(0),glbinding(0),out

#define THREAD_COUNT 4

groupshared int gA[THREAD_COUNT];
int test(int val)
{
	gA[val] = val;
	GroupMemoryBarrierWithGroupSync();
	val = gA[val ^ 1];

/* TODO: once function-scope `static` works
	static groupshared int gB[THREAD_COUNT];

	gB[val] = val;
	GroupMemoryBarrierWithGroupSync();
	val = gB[val ^ 2];
*/

	return val;
}

RWStructuredBuffer<int> gBuffer;

[numthreads(THREAD_COUNT, 1, 1)]
void computeMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint tid = dispatchThreadID.x;

	int val = int(tid);
	val = test(val);
	gBuffer[tid] = val;
}