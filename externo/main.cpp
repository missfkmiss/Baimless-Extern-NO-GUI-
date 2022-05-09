#include <iostream>
#include <Windows.h>
#include <fstream>
#include "includes.h"
#include <vector>
#include "math.h"
#include "draw.h"	

bool active = false;

using namespace std;

void esp()
{
	Vector3 posLocal;
	while (pGlobals.active)
	{
		
		view_matrix_t vm = RPM<view_matrix_t>(pGlobals.modulebase + offsets::dwViewMatrix);
		
		uintptr_t localPlayerPtr = RPM<DWORD>(pGlobals.modulebase + offsets::dwLocalPlayer);
		
		int localTeam = RPM<int>((pGlobals.modulebase + offsets::dwLocalPlayer) + offsets::m_iTeamNum);
		
		for (int i = 1; i < 32; ++i)
		{
			
			uintptr_t pEnt = RPM<DWORD>(pGlobals.modulebase + offsets::dwEntityList + (i * 0x10));
			
			int enemyTeam = RPM<int>(pEnt + offsets::m_iTeamNum);
			
			int health = RPM<int>(pEnt + offsets::m_iHealth);

			
			Vector3 pos = RPM<Vector3>(pEnt + offsets::m_vecOrigin);
			Vector3 posLocal = RPM<Vector3>(localPlayerPtr + offsets::m_vecOrigin);
			
			Vector3 posHead;
			posHead.x = pos.x;
			posHead.y = pos.y;
			posHead.z = pos.z + 75.f;

			Vector3 bonePos = getEntBonePos(pEnt, 8);

			
			Vector3 screenPos = WorldToScreen(pos, vm);
			Vector3 screenHeadPos = WorldToScreen(posHead, vm);
			Vector3 screenboneheadPos = WorldToScreen(bonePos, vm);

		
			float height = screenHeadPos.y - screenPos.y;
			
			float width = height / 2.4f;

			
			float healthheight = health * height / 100;

			
			float healthwidth = height / 20.f;

			
			if (screenPos.z >= 0.01f && enemyTeam != localTeam && health > 0 && health < 101)
			{
				
				DrawBackGroundHealth(screenPos.x + (width / 1.8), screenPos.y, healthwidth, height);
				DrawHealth(screenPos.x + (width / 1.8), screenPos.y, healthwidth, healthheight);
			}
		}
	}
}

void triggerbot()
{
	while (pGlobals.trigger)
	{
		uintptr_t localPlayer = RPM<uintptr_t>(pGlobals.modulebase + offsets::dwLocalPlayer);
		int entity_id = RPM<int>(localPlayer + offsets::m_iCrosshairId);
		int entity = RPM<int>(pGlobals.modulebase + offsets::dwEntityList + entity_id * 0x10);
		int entityTeam = RPM<int>(entity + offsets::m_iTeamNum);
		int localplayerTeam = RPM<int>(localPlayer + offsets::m_iTeamNum);

		if (GetAsyncKeyState(VK_LEFT))
		{
			if (entity_id > 0 && entity_id < 32 && localplayerTeam != entityTeam)
			{
				WPM<int>(6, pGlobals.modulebase + offsets::dwForceAttack);
			}
		}
	}
}

void bunnyhop()
{
	while (pGlobals.bhop)
	{
		uintptr_t localPlayer = RPM<uintptr_t>(pGlobals.modulebase + offsets::dwLocalPlayer);
		int flags = RPM<int>(localPlayer + offsets::m_fFlags);
		if (GetAsyncKeyState(VK_SPACE) & 0x8000 && (flags == 257))
		{
			WPM<int>(5, pGlobals.modulebase + offsets::dwForceJump);
		}
		else if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !(flags == 257))
		{
			WPM<int>(4, pGlobals.modulebase + offsets::dwForceJump);
			WPM<int>(5, pGlobals.modulebase + offsets::dwForceJump);
			WPM<int>(4, pGlobals.modulebase + offsets::dwForceJump);
		}
		else
		{
			WPM<int>(4, pGlobals.modulebase + offsets::dwForceJump);
		}
	}

}

void glow()
{
	while (pGlobals.glow)
	{
		uintptr_t localPlayer = RPM<uintptr_t>(pGlobals.modulebase + offsets::dwLocalPlayer);
		uintptr_t glowObjectManager = RPM<uintptr_t>(pGlobals.modulebase + offsets::dwGlowObjectManager);
		uintptr_t localPlayerTeam = RPM<uintptr_t>(localPlayer + offsets::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			auto entity = RPM<uintptr_t>(pGlobals.modulebase + offsets::dwEntityList + i * 0x10);

			if (!entity)
				continue;

			// dont glow if they are on our team
			if (RPM<uintptr_t>(entity + offsets::m_iTeamNum) == localPlayerTeam)
				continue;

			auto glowIndex = RPM<int32_t>(entity + offsets::m_iGlowIndex);
			auto R = glowObjectManager + (glowIndex * 0x38) + 0x8;
			auto G = glowObjectManager + (glowIndex * 0x38) + 0xC;
			auto B = glowObjectManager + (glowIndex * 0x38) + 0x10;
			auto A = glowObjectManager + (glowIndex * 0x38) + 0x14;
			auto B1 = glowObjectManager + (glowIndex * 0x38) + 0x28;
			auto B2 = glowObjectManager + (glowIndex * 0x38) + 0x29;
			WPM<float>(1.f, R);
			WPM<float>(0.f, G);
			WPM<float>(0.f, B);
			WPM<float>(1.f, A);
			WPM<bool>(true, B1);
			WPM<bool>(false, B2);

		}
	}
}

DWORD playerfunction()
{

	DWORD nextaimplayer = NULL;
	Vector3 poslocal;
	Vector3 screenheadpos;
	float mindistace = 9999;
	view_matrix_t vm = RPM<view_matrix_t>(pGlobals.modulebase + offsets::dwViewMatrix);
	uintptr_t localplayerptr = RPM<DWORD>(pGlobals.modulebase + offsets::dwLocalPlayer);
	for (int i = 1; i < 10; ++i)
	{
		auto pEnt = RPM<DWORD>(pGlobals.modulebase + offsets::dwEntityList + (i * 0x10));
		int health = RPM<int>(pEnt + offsets::m_iHealth);
		Vector3 pos = RPM<Vector3>(pEnt + offsets::m_vecOrigin);
		poslocal = RPM<Vector3>(localplayerptr + offsets::m_vecOrigin);

		Vector3 bonepos = getEntBonePos(pEnt, 8);
		Vector3 screnboneheadpos = WorldToScreen(bonepos, vm);

		Vector3 screenpos = WorldToScreen(pos, vm);
		auto dormant = RPM<bool>(pEnt + offsets::m_bDormant);
		if (health > 0 && health <= 100 && !dormant)
		{
			double calcDist = sqrt(pow((GetSystemMetrics(SM_CXSCREEN) / 2) - screnboneheadpos.x, 2) + pow((GetSystemMetrics(SM_CYSCREEN) / 2) - screnboneheadpos.y, 2));

			if (calcDist < mindistace)
			{
				mindistace = calcDist;
				nextaimplayer = pEnt;
			}
		}
	}
	return nextaimplayer;
}

void aimbot()
{
	while (pGlobals.activeAimbot)
	{
		DWORD target = playerfunction();
		if (target != NULL)
		{
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				uintptr_t clientstate = RPM<DWORD>(pGlobals.engineAddress + offsets::dwClientState);
				view_matrix_t vm = RPM<view_matrix_t>(pGlobals.modulebase + offsets::dwViewMatrix);
				uintptr_t localplayerptr = RPM<DWORD>(pGlobals.modulebase + offsets::dwLocalPlayer);
				Vector3 poslocal = RPM<Vector3>(localplayerptr + offsets::m_vecOrigin);
				Vector3 semitarget = RPM<Vector3>(target + offsets::m_vecOrigin);
				Vector3 aimangles = CalcAngle(poslocal , semitarget);
				WPM<Vector3>(aimangles, clientstate + offsets::dwClientState_ViewAngles);
			}
		}
	}
}

int main()
{
	
	pGlobals.modulebase = GetModuleBaseAddress(GetProcId(L"csgo.exe"), L"client.dll");
	pGlobals.engineAddress = GetModuleBaseAddress(GetProcId(L"csgo.exe"), L"engine.dll");
	pGlobals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetProcId(L"csgo.exe"));
	pGlobals.hdc = GetDC(FindWindowA(NULL, "Counter-Strike: Global Offensive - Direct3D 9"));
	pGlobals.screenX = GetSystemMetrics(SM_CXSCREEN);
	pGlobals.screenY = GetSystemMetrics(SM_CYSCREEN);
	while (true)
	{
		
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			pGlobals.active = !pGlobals.active;
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)esp, NULL, 0, NULL));
		}
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			pGlobals.trigger = !pGlobals.trigger;
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)triggerbot, NULL, 0, NULL));
		}
		if (GetAsyncKeyState(VK_F3) & 1)
		{
			pGlobals.bhop = !pGlobals.bhop;
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)bunnyhop, NULL, 0, NULL));
		}
		if (GetAsyncKeyState(VK_F4) & 1)
		{
			pGlobals.glow = !pGlobals.glow;
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)glow, NULL, 0, NULL));
		}
		if (GetAsyncKeyState(VK_F5) & 1)
		{
			pGlobals.activeAimbot = !pGlobals.activeAimbot;
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)aimbot, NULL, 0, NULL));
		}
	}
	CloseHandle(pGlobals.hProcess);
	return 0;
}


