int TimeTrial_SetEnabled(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	if (on) {
		InitTimeTrials();
		bLastRaceWasTimeTrial = true;
	}
	else if (bTimeTrialsEnabled) {
		UninitTimeTrials();
		bLastRaceWasTimeTrial = false;
	}
	bTimeTrialsEnabled = on;
	return 0;
}

int TimeTrial_Set3LapMode(void* a1) {
	b3LapMode = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_Get3LapMode(void* a1) {
	lua_pushnumber(a1, b3LapMode);
	return 1;
}

int TimeTrial_SetPropsEnabled(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	if (on) EnableProps();
	else DisableProps();
	bNoProps = !on;
	return 0;
}

int TimeTrial_SetPropsEnabledBool(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	bNoProps = !on;
	return 0;
}

int TimeTrial_SetNitroType(void* a1) {
	nNitroType = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetUpgradeLevel(void* a1) {
	nUpgradeLevel = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetHandlingMode(void* a1) {
	nHandlingMode = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetTrackReversed(void* a1) {
	bTrackReversed = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_GetPropsEnabled(void* a1) {
	lua_pushnumber(a1, !bNoProps);
	return 1;
}

int TimeTrial_GetNitroType(void* a1) {
	lua_pushnumber(a1, nNitroType);
	return 1;
}

int TimeTrial_SetCareerMode(void* a1) {
	bIsCareerMode = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetCareerSuperAuthors(void* a1) {
	bDisplaySuperAuthorTime = (int)luaL_checknumber(a1, 1) >= 1;
	bDisplayAuthorInCareer = (int)luaL_checknumber(a1, 1) >= 2;
	return 0;
}

int TimeTrial_WasLastRaceTimeTrial(void* a1) {
	lua_pushboolean(a1, bLastRaceWasTimeTrial);
	bLastRaceWasTimeTrial = false;
	return 1;
}

int TimeTrial_GetLastEventGhostTime(void* a1) {
	lua_pushnumber(a1, OpponentsCareer[(int)luaL_checknumber(a1, 1)].nLastRacePBTime);
	return 1;
}

int TimeTrial_IsLastEventGhostValid(void* a1) {
	lua_pushnumber(a1, OpponentsCareer[(int)luaL_checknumber(a1, 1)].nLastRacePBTime != UINT_MAX);
	return 1;
}

int TimeTrial_GetLastEventPlayerTime(void* a1) {
	lua_pushnumber(a1, nCareerLastRacePBTime);
	return 1;
}

int TimeTrial_CheckCheatCode(void* a1) {
	if (!pMenuEventManager->wsKeyboardInput.Get()) return 0;
	std::wstring str = pMenuEventManager->wsKeyboardInput.Get();
	std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c){ return std::tolower(c); });
	if (str == L"ghostson") {
		bDisplayGhostsInCareer = true;
	}
	return 0;
}

void RegisterLUAFunction(void* a1, void* function, const char* name) {
	lua_setglobal(a1, name);
	lua_pushcfunction(a1, function, 0);
	lua_settable(a1, LUA_ENVIRONINDEX);
}

void CustomLUAFunctions(void* a1) {
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetEnabled, "TimeTrial_SetEnabled");
	RegisterLUAFunction(a1, (void*)&TimeTrial_Set3LapMode, "TimeTrial_Set3LapMode");
	RegisterLUAFunction(a1, (void*)&TimeTrial_Get3LapMode, "TimeTrial_Get3LapMode");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetPropsEnabled, "TimeTrial_SetPropsEnabled");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetPropsEnabledBool, "TimeTrial_SetPropsEnabledBool");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetNitroType, "TimeTrial_SetNitroType");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetUpgradeLevel, "TimeTrial_SetUpgradeLevel");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetHandlingMode, "TimeTrial_SetHandlingMode");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetTrackReversed, "TimeTrial_SetTrackReversed");
	RegisterLUAFunction(a1, (void*)&TimeTrial_GetPropsEnabled, "TimeTrial_GetPropsEnabled");
	RegisterLUAFunction(a1, (void*)&TimeTrial_GetNitroType, "TimeTrial_GetNitroType");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetCareerMode, "TimeTrial_SetCareerMode");
	RegisterLUAFunction(a1, (void*)&TimeTrial_SetCareerSuperAuthors, "TimeTrial_SetCareerSuperAuthors");
	RegisterLUAFunction(a1, (void*)&TimeTrial_GetLastEventGhostTime, "TimeTrial_GetLastEventGhostTime");
	RegisterLUAFunction(a1, (void*)&TimeTrial_IsLastEventGhostValid, "TimeTrial_IsLastEventGhostValid");
	RegisterLUAFunction(a1, (void*)&TimeTrial_GetLastEventPlayerTime, "TimeTrial_GetLastEventPlayerTime");
	RegisterLUAFunction(a1, (void*)&TimeTrial_WasLastRaceTimeTrial, "TimeTrial_WasLastRaceTimeTrial");
	RegisterLUAFunction(a1, (void*)&TimeTrial_CheckCheatCode, "TimeTrial_CheckCheatCode");
}

void ApplyLUAPatches() {
	NyaFO2Hooks::PlaceScriptHook();
	NyaFO2Hooks::aScriptFuncs.push_back(CustomLUAFunctions);
}

extern "C" __declspec(dllexport) void __cdecl ChloeTimeTrial_GetCareerMedalTimes(int level, int car, int* outTimes) {
	bIsCareerMode = true;
	bTrackReversed = false;
	for (int i = 0; i < 5; i++) {
		OpponentsCareer[i].nLastRacePBTime = UINT_MAX;
		LoadPB(&OpponentsCareer[i], car, level, LAPTYPE_STANDING, i+1);
		outTimes[i] = OpponentsCareer[i].GetFinishTime();
	}
}

extern "C" __declspec(dllexport) const wchar_t* __cdecl ChloeTimeTrial_GetCareerSuperAuthorName(int level, int car) {
	bIsCareerMode = true;
	bTrackReversed = false;
	OpponentsCareer[4].nLastRacePBTime = UINT_MAX;
	LoadPB(&OpponentsCareer[4], car, level, LAPTYPE_STANDING, 5);
	return OpponentsCareer[4].sPlayerName.c_str();
}

extern "C" __declspec(dllexport) uint32_t __cdecl ChloeTimeTrial_GetCareerPBTime(int level, int car) {
	bIsCareerMode = true;
	bTrackReversed = false;
	StandingLapPB.nLastRacePBTime = UINT_MAX;
	LoadPB(&StandingLapPB, car, level, LAPTYPE_STANDING, false);
	return StandingLapPB.GetFinishTime();
}

extern "C" __declspec(dllexport) uint32_t __cdecl ChloeTimeTrial_GetCurrentRacePBTime(bool standing, bool opponent) {
	if (b3LapMode) return opponent ? OpponentThreeLapPB.GetFinishTime() : ThreeLapPB.GetFinishTime();

	if (standing) {
		return opponent ? OpponentStandingLapPB.GetFinishTime() : StandingLapPB.GetFinishTime();
	}
	else {
		return opponent ? OpponentRollingLapPB.GetFinishTime() : RollingLapPB.GetFinishTime();
	}
}

extern "C" __declspec(dllexport) bool __cdecl ChloeTimeTrial_GetCurrentRacePBTimeJustUpdated(bool standing, bool opponent) {
	if (b3LapMode) return opponent ? OpponentThreeLapPB.fTextHighlightTime > 0 : ThreeLapPB.fTextHighlightTime > 0;

	if (standing) {
		return opponent ? OpponentStandingLapPB.fTextHighlightTime > 0 : StandingLapPB.fTextHighlightTime > 0;
	}
	else {
		return opponent ? OpponentRollingLapPB.fTextHighlightTime > 0 : RollingLapPB.fTextHighlightTime > 0;
	}
}