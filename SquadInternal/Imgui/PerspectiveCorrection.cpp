#include "PerspectiveCorrection.h"
#include "../externals/HookFunctions/HookFunctions.h"
#define PI_VALUE 3.141592f

DWORD jmpFovAddress;
DWORD jmpRadarAddress;
float aspectRatio = 1.3333f;
float writeFOV = 85.0f;
float zero = 0.0f;

static float increaseHorFOV()
{
	float tempradian = PI_VALUE * writeFOV / 180.0f;
	tempradian = 2 * atanf(tanf(tempradian / 2.0f) * 0.75f);
	tempradian = (2.0f * atanf(tanf(tempradian / 2.0f) * aspectRatio));
	return tempradian * (180.0f / PI_VALUE);
}

DWORD* transparencyAddy;
void __declspec(naked) removeRadarJunk()
{
	/*"Rogue Squadron.EXE"+7B5DC
	fmul dword ptr ["Rogue Squadron.EXE"+304CC0] responsible for transparency (the element it applies to is stored in edx register)
	*/

	__asm
	{
		cmp edx, 0x08
		je blank
		cmp edx, 0x07
		je blank
		push eax
		mov eax, [transparencyAddy]
		fmul dword ptr[eax] //located at "Rogue Squadron.EXE"+0x304CC0
		pop eax
		jmp jmpRadarAddress

		blank :
		fmul[zero]
			jmp jmpRadarAddress
	}
}

void __declspec(naked) fovHack()
{
	/*""Rogue Squadron.EXE"+D520C
	mov eax,[edx+20]
	push eax
	mov ecx,[ebp-04]

	Responsible for Drawing Terrain - we have to increase it, although it has a chance of breaking Terrain rendering
	*/

	__asm
	{
		mov eax, [edx + 0x20]
		mov[writeFOV], eax
	}

	writeFOV = increaseHorFOV();

	__asm
	{
		mov eax, [writeFOV]
		push eax
		mov ecx, [ebp - 0x04]
		jmp[jmpFovAddress]
	}
}

PerspectiveCorrection::PerspectiveCorrection(const char * configPath)
{
	CIniReader reader(configPath);
	this->Width = reader.ReadInteger("MAIN", "Width", 640);
	this->Height = reader.ReadInteger("MAIN", "Height", 480);

}

void PerspectiveCorrection::FixStuff()
{
	if (!this->hackInstalled)
	{
		HMODULE baseModule = GetModuleHandle(NULL);
		UnprotectModule(baseModule);

		*(short*)((DWORD)baseModule + 0x20355C) = this->Width;
		*(short*)((DWORD)baseModule + 0x203560) = this->Height;
		aspectRatio = this->Width / (float)this->Height;
		*(float*)((DWORD)baseModule + 0x16EC42) = aspectRatio;


		HookInsideFunction((DWORD)baseModule + 0xD520C, fovHack, &jmpFovAddress, 7);
		transparencyAddy = (DWORD*)((DWORD)baseModule + 0x304CC0);
		HookInsideFunction((DWORD)baseModule + 0x7B5DC, removeRadarJunk, &jmpRadarAddress, 6);

		this->hackInstalled = true;
	}
}
