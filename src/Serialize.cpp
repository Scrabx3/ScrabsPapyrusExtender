#include "Serialize.h"

#include "Handlers/ActorManager.h"

void Serialize::Save(SKSE::SerializationInterface* a_intfc) {
    if (!a_intfc->OpenRecord(RecordID, 0)) {
        SKSE::log::error("Unable to open record to write cosave data.");
        return;
    }

    ActorManager::Serialize(a_intfc);
}

void Serialize::Load(SKSE::SerializationInterface* a_intfc) {
    uint32_t type;
    uint32_t version;
    uint32_t length;
    while (a_intfc->GetNextRecordInfo(type, version, length)) {
        if (type == RecordID) {
            ActorManager::Deserialize(a_intfc);
            break;
        }
    }
}

void Serialize::Revert(SKSE::SerializationInterface* a_intfc) {
    ActorManager::Revert(a_intfc);
}