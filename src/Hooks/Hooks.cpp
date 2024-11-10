#include "Hooks.h"

#include "Handlers/ActorManager.h"
#include "Handlers/MarkerManager.h"
#include "Patch.h"

void Hooks::Install()
{
	logger::info("installing hooks");

	SKSE::AllocTrampoline(64);
	auto& trampoline = SKSE::GetTrampoline();

	REL::Relocation<std::uintptr_t> det{ REL_ID(41659, 42742), REL_OF(0x526, 0x67B, 0x67B) };
	_DoDetect = trampoline.write_call<5>(det.address(), DoDetect);

	REL::Relocation<std::uintptr_t> upc{ RE::Character::VTABLE[0] };
	_UpdateCombat = upc.write_vfunc(0x0E4, UpdateCombat);

	REL::Relocation<std::uintptr_t> amd{ REL_ID(36359, 37350), REL_OF(0xF0, 0xFB, 0xFB) };
	_ApplyMovementDelta = trampoline.write_call<5>(amd.address(), ApplyMovementDelta);

	REL::Relocation<std::uintptr_t> gqm{ REL_ID(50826, 51691), REL_OF(0xFB, 0x167, 0xFB) };
	_GetQuestMarkerRef = trampoline.write_call<5>(gqm.address(), GetQuestMarkerRef);

	HINSTANCE dllHandle = LoadLibrary(TEXT("CompassNavigationOverhaul.dll"));
	if (dllHandle != NULL) {
		logger::info("installing cno patch");
		Patch::Install();
	} else {
		logger::info("installing cno hook");
		REL::Relocation<std::uintptr_t> udq{ REL_ID(50826, 51691), REL_OF(0x114, 0x180, 0x114) };
		_UpdateQuests = trampoline.write_call<5>(udq.address(), UpdateQuests);
	}
}

void Hooks::UpdateCombat(RE::Character* a_this)
{
	if (!ActorManager::IsCalmed(a_this)) {
		_UpdateCombat(a_this);
	} else if (a_this->IsInCombat()) {
		a_this->StopCombat();
	}
}

void Hooks::ApplyMovementDelta(RE::Actor* a_actor, float a_delta)
{
	if (!ActorManager::IsCollisionFrozen(a_actor)) {
		_ApplyMovementDelta(a_actor, a_delta);
	}
}

uint8_t* Hooks::DoDetect(RE::Actor* viewer, RE::Actor* target, int32_t& detectval, uint8_t& unk04, uint8_t& unk05, uint32_t& unk06, RE::NiPoint3& pos, float& unk08, float& unk09, float& unk10)
{
	if (viewer && ActorManager::IsCalmed(viewer) || target && ActorManager::IsCalmed(target)) {
		detectval = -1000;
		return nullptr;
	}

	return _DoDetect(viewer, target, detectval, unk04, unk05, unk06, pos, unk08, unk09, unk10);
}

RE::RefHandle Hooks::GetQuestMarkerRef(int64_t a_1, int64_t a_2, RE::TESQuest* a_3)
{
	MarkerManager::SetQuest(a_3);
	return _GetQuestMarkerRef(a_1, a_2, a_3);
}

bool Hooks::UpdateQuests(void* a_1, void* a_2, RE::NiPoint3* a_3, const RE::RefHandle& a_refHandle, std::int32_t a_5)
{
	const auto marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();
	MarkerManager::SetMarker(marker);
	return _UpdateQuests(a_1, a_2, a_3, a_refHandle, a_5);
}