#include "Papyrus/Papyrus.h"
#include "Console.h"
#include "Interface/CustomMenu.h"

// static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
// {
// 	switch (message->type) {
// 	case SKSE::MessagingInterface::kPostLoad:
// 	case SKSE::MessagingInterface::kSaveGame:
// 	case SKSE::MessagingInterface::kDataLoaded:
// 	case SKSE::MessagingInterface::kNewGame:
// 	case SKSE::MessagingInterface::kPostLoadGame:
// 		break;
// 	}
// }

#ifdef SKYRIM_SUPPORT_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Scrab Joséline"sv);
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });
	// v.CompatibleVersions({ SKSE::RUNTIME_1_6_353 });
	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION.pack();
	return true;
}
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	const auto InitLogger = []() -> bool {
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path)
			return false;
		*path /= std::format("{}.log", Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
#ifndef NDEBUG
		log->set_level(spdlog::level::trace);
		log->flush_on(spdlog::level::trace);
#else
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
#endif
		spdlog::set_default_logger(std::move(log));
#ifndef NDEBUG
		spdlog::set_pattern("%s(%#): [%T] [%^%l%$] %v"s);
#else
		spdlog::set_pattern("[%T] [%^%l%$] %v"s);
#endif

		logger::info("{} v{}", Plugin::NAME, Plugin::VERSION.string());
		return true;
	};
	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible");
		return false;
	} else if (!InitLogger()) {
		return false;
	}

	SKSE::Init(a_skse);

	if (!Papyrus::Register()) {
		logger::critical("Failed to register papyrus functions");
		return false;
	}

	Internal::Interface::CustomMenu::Register();
	// Registry::Console::Install();

	// const auto msging = SKSE::GetMessagingInterface();
	// if (!msging->RegisterListener("SKSE", SKSEMessageHandler)) {
	// 	logger::critical("Failed to register Listener");
	// 	return false;
	// }

	logger::info("{} loaded", Plugin::NAME);

	return true;
}
