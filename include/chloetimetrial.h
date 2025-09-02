namespace ChloeTimeTrial {
	template<typename T>
	T GetFuncPtr(const char* funcName) {
		if (auto dll = LoadLibraryA("FlatOutTimeTrialGhosts_gcp.dll")) {
			return (T)GetProcAddress(dll, funcName);
		}
		return nullptr;
	}

	void GetCareerMedalTimes(int level, int car, uint32_t* outTimes) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int, int, uint32_t*)>("ChloeTimeTrial_GetCareerMedalTimes");
		if (!funcPtr) return;
		funcPtr(level, car, outTimes);
	}

	const wchar_t* GetCareerSuperAuthorName(int level, int car) {
		static auto funcPtr = GetFuncPtr<const wchar_t*(__cdecl*)(int, int)>("ChloeTimeTrial_GetCareerSuperAuthorName");
		if (!funcPtr) return nullptr;
		return funcPtr(level, car);
	}

	uint32_t GetCareerPBTime(int level, int car) {
		static auto funcPtr = GetFuncPtr<uint32_t(__cdecl*)(int, int)>("ChloeTimeTrial_GetCareerPBTime");
		if (!funcPtr) return UINT_MAX;
		return funcPtr(level, car);
	}
}