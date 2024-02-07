#pragma once

// Large root signature may be bad for performance
/*
* https://www.gamedev.net/forums/topic/711211-direct-x12-how-bad-is-a-large-root-signature/
	The effects of a large root signature really depends on the exact hardware and driver.
	AMD makes that recommendation because they have 16 registers that can be set from the command buffer, and they use that to implement a root signature. 
	Anything that doesn't fit in those registers has to be “spilled” to memory, which can add CPU overhead and potentially affect GPU performance. 
	If you keep your root signature smaller by building constant buffers yourself, then that overhead is instead in your own code which means you can directly measure the cost and make the right trade-offs for your usage patterns.
*/
#define MAX_SRVS		32
#define MAX_UAVS		16
#define MAX_ROOT_CBV	16 // if increase this, Bit count of "CBVRegisterMask" variable should be changed properly