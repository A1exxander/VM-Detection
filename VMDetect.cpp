#include <iostream>
#include <windows.h>
#include <intrin.h>
#include "VMDetect.h"

	bool VMDetect::invalidTSC() {

		UINT64 timestampOne{ __rdtsc() };
		UINT64 timestampTwo{ __rdtsc() };
		return ((timestampTwo - timestampOne) > 500);

	}

	bool VMDetect::isVirtualizedCPU() {

		int cpuInfo[4];
		__cpuid(cpuInfo, 1);

		if (!(cpuInfo[2] & (1 << 31))) {
			return false;
		}
		
		const auto queryVendorIdMagic{ 0x40000000 };
		constexpr int vendorIdLength{ 13 };
		char hyperVendorId[vendorIdLength];

		__cpuid(cpuInfo, queryVendorIdMagic);
		memcpy(hyperVendorId + 0, &cpuInfo[1], 4);
		memcpy(hyperVendorId + 4, &cpuInfo[2], 4);
		memcpy(hyperVendorId + 8, &cpuInfo[3], 4);
		hyperVendorId[12] = '\0';

		static const char* vendors[] {
		"KVMKVMKVM\0\0\0", // KVM 
		"Microsoft Hv",    // Microsoft Hyper-V or Windows Virtual PC */
		"VMwareVMware",    // VMware 
		"XenVMMXenVMM",    // Xen 
		"prl hyperv  ",    // Parallels
		"VBoxVBoxVBox"     // VirtualBox 
		};

		for (const auto& vendor : vendors) {
			if (!memcmp(vendor, hyperVendorId, vendorIdLength)) {
				return true;
			}
		}

		return false;
		
	}

	bool VMDetect::isVM() { return (VMDetect::invalidTSC() && VMDetect::isVirtualizedCPU()) }
