#include <windows.h>
#include <d3d9.h>
#include <filesystem>
#include "toml++/toml.hpp"

#include "nya_dx9_hookbase.h"
#include "nya_commonmath.h"
#include "nya_commonhooklib.h"

#include "fo1.h"
#include "../../nya-common-fouc/fo2versioncheck.h"
#include "chloemenulib.h"

void SetPlayerControl(bool on) {
	NyaHookLib::Patch<uint32_t>(0x66059C, on ? 0x449CF0 : 0x44A0A3);
}

void SetGhostVisuals(bool on) {

}

void UninitTimeTrials();

void DisableProps() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4CD314, 0x4C9758);
}

void EnableProps() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4CD314, 0x4C95E0);
}

#include "timetrialshared.h"

uintptr_t ProcessPlayerCarsASM_call = 0x44A0B0;
void __attribute__((naked)) ProcessPlayerCarsASM() {
	__asm__ (
		"pushad\n\t"
		"push 0\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ProcessPlayerCarsASM_call), "i" (ProcessGhostCar)
	);
}

uintptr_t FinishLapASM_jmp = 0x4541AF;
void __attribute__((naked)) FinishLapASM() {
	__asm__ (
		"mov [edi+0x60], edx\n\t"
		"mov dword ptr [edi+eax*4+0x68], 0\n\t"
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (FinishLapASM_jmp), "i" (OnFinishLap)
	);
}

void __fastcall SetAISameCar(Player* pPlayer) {
	if (!bTimeTrialsEnabled) return;

	pPlayer->nCarId = pGameFlow->nCar;
	pPlayer->nCarSkinId = 1;

	if (pPlayer->nPlayerId <= 2) return;
	auto tmp = LoadTemporaryGhostForSpawning(pPlayer->nCarId);
	if (!tmp->IsValid()) return;
	//if (tmp->sPlayerName[0] && pOpponentPlayerInfo) {
	//	wcscpy_s(pOpponentPlayerInfo->sPlayerName, 16, tmp->sPlayerName.c_str());
	//}
	if (tmp->nCarSkinId < 1 || tmp->nCarSkinId > GetNumSkinsForCar(pPlayer->nCarId)) return;
	pPlayer->nCarSkinId = tmp->nCarSkinId;
}

void __attribute__((naked)) AISameCarASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %0\n\t"
		"popad\n\t"

		"pop esi\n"
		"mov eax, edi\n"
		"pop ebx\n"
		"ret 8\n\t"
			:
			:  "i" (SetAISameCar)
	);
}

void UpdateD3DProperties() {
	g_pd3dDevice = pDeviceD3d->pD3DDevice;
	ghWnd = pDeviceD3d->hWnd;
	nResX = nGameResolutionX;
	nResY = nGameResolutionY;
}

bool bDeviceJustReset = false;
void D3DHookMain() {
	if (!g_pd3dDevice) {
		UpdateD3DProperties();
		InitHookBase();
	}

	if (bDeviceJustReset) {
		ImGui_ImplDX9_CreateDeviceObjects();
		bDeviceJustReset = false;
	}
	HookBaseLoop();
}

void OnEndScene() {
	D3DHookMain();
}

void OnD3DReset() {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
}

int __cdecl GetNumOpponents(int, int) {
	if (!bTimeTrialsEnabled) return 8;

	int numOpponents = 2;
	if (bIsCareerMode) numOpponents = bDisplayAuthorInCareer ? 5 : 3;
	return numOpponents+1;
}

void __fastcall NoGhostCollisions(float* matrix1, float* matrix2) {
	bool isGhost = false;

	if (bTimeTrialsEnabled) {
		for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType == PLAYERTYPE_LOCAL) continue;
			if (matrix1 == ply->pCar->mMatrix) {
				isGhost = true;
				break;
			}
			if (matrix2 == ply->pCar->mMatrix) {
				isGhost = true;
				break;
			}
		}
	}

	// prop activation
	NyaHookLib::Patch<uint64_t>(0x4B136C, isGhost ? 0x46D9900000021AE9 : 0x46D900000081840F);

	//// prop activation
	//NyaHookLib::Patch<uint64_t>(0x4B136C, isGhost ? 0x46D99000000082E9 : 0x46D900000081840F);
	//NyaHookLib::Patch<uint64_t>(0x4B13FA, isGhost ? 0x46D99000000084E9 : 0x46D900000083840F);
	//// car to car collision
	//NyaHookLib::Patch<uint64_t>(0x4B1492, isGhost ? 0x84D990000000F4E9 : 0x84D9000000F3850F);
}

uintptr_t NoGhostCollisionsASM_jmp = 0x4B133B;
void __attribute__((naked)) NoGhostCollisionsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"mov edx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"mov al, [edi+0xB4]\n\t"
		"test al, 0x40\n\t"
		"jmp %0\n\t"
			:
			:  "m" (NoGhostCollisionsASM_jmp), "i" (NoGhostCollisions)
	);
}


void InitTimeTrials() {
	static bool bOnce = false;
	if (!bOnce) {
		ProcessPlayerCarsASM_call = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x44A099, &ProcessPlayerCarsASM);
		bOnce = true;
	}

	NyaHookLib::Fill(0x43F073, 0x90, 5); // remove player count reset

	NyaHookLib::Patch(0x43F74E, bIsCareerMode ? &aAINamesCareer[-1] : &aAINames[-1]);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4064EB, &AISameCarASM);
	NyaHookLib::Patch<uint8_t>(0x423A66, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x41E442, 0xEB); // use regular skins for ai
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4541A4, &FinishLapASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43F05C, &GetNumOpponents);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x406CF0, &ProcessGhostCar); // disable ai control
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4B1333, &NoGhostCollisionsASM);
}

void UninitTimeTrials() {
	// revert ai name
	NyaHookLib::Patch(0x43F74E, 0x68BC34);

	// revert assembly patches
	NyaHookLib::Patch<uint64_t>(0x4541A4, 0x00688744C7605789);

	// revert ai count
	nNumPlayers = 8;

	// revert ai skins
	NyaHookLib::Patch<uint8_t>(0x423A66, 0x75); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x41E442, 0x75); // use regular skins for ai

	// revert ai control
	NyaHookLib::Patch<uint64_t>(0x406CF0, 0x033C858BE98B5553);

	// revert prop patches
	EnableProps();
}

#include "luafunctions.h"

void InitStandalone() {
	InitAndReadConfigFile();

	if (bTimeTrialsEnabled) {
		InitTimeTrials();
		if (nGhostVisuals == 0) SetGhostVisuals(false);
		if (bNoProps) {
			DisableProps();
		}
	}
	else {
		bPBTimeDisplayEnabled = false;
		bCurrentSessionPBTimeDisplayEnabled = false;
		bViewReplayMode = false;
	}
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FO1_1_1);

			NyaFO2Hooks::PlaceD3DHooks();
			NyaFO2Hooks::aEndSceneFuncs.push_back(OnEndScene);
			NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);

			ChloeMenuLib::RegisterMenu("Time Trial Ghosts - gaycoderprincess", &TimeTrialMenu);

			if (std::filesystem::exists("FlatOutChloeCollection_gcp.asi") || std::filesystem::exists("FlatOutCustomMP_gcp.asi") || std::filesystem::exists("fo1pack_gcp_misc.bfs")) {
				ApplyLUAPatches();
				bChloeCollectionIntegration = true;
				bTimeTrialsEnabled = false;

				auto config = toml::parse_file("FlatOutTimeTrialGhosts_gcp.toml");
				bReplayIgnoreMismatches = config["main"]["load_mismatched_replays"].value_or(false);
				bShowInputsWhileDriving = config["extras"]["always_show_input_display"].value_or(false);
				gInputRGBHighlight.r = config["input_display"]["highlight_r"].value_or(0);
				gInputRGBHighlight.g = config["input_display"]["highlight_g"].value_or(255);
				gInputRGBHighlight.b = config["input_display"]["highlight_b"].value_or(0);
				gInputRGBBackground.r = config["input_display"]["background_r"].value_or(200);
				gInputRGBBackground.g = config["input_display"]["background_g"].value_or(200);
				gInputRGBBackground.b = config["input_display"]["background_b"].value_or(200);
				fInputBaseXPosition = config["input_display"]["pos_x"].value_or(0.2);
				fInputBaseYPosition = config["input_display"]["pos_y"].value_or(0.85);
			}
			else {
				InitStandalone();
			}
		} break;
		default:
			break;
	}
	return TRUE;
}