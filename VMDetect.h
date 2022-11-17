#pragma once

namespace VMDetect {

	bool invalidTSC();
	bool cpu_known_vm_vendors();
	bool isVM();

}