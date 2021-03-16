#include "algo_factory.h"

#include <mutex>

namespace {
	std::mutex& getSingletonMutex()
	{
		static std::mutex singleton_mutex;
		return singleton_mutex;
	}
} // end anonymous namespace


namespace space {

	std::string AlgoFactory::AlgoNameField = "AlgoName";

	bool AlgoFactory::registerAlgoMachine(const std::string& algoName, AlgoMachine algoMachine)
	{
		AlgoFactory& instance = getInstance();
		{
			std::lock_guard<std::mutex> lock(getSingletonMutex());
			instance.m_algoMachines.emplace(algoName, algoMachine);
		}
		return true;
	}

	std::optional<IAlgo::Ptr> AlgoFactory::tryCreateAlgo(const nlohmann::json& config)
	{
		auto algoNameIt = config.find(AlgoNameField);
		if (config.end() == algoNameIt)
			return std::optional<IAlgo::Ptr>();
		auto algoName = algoNameIt->get<std::string>();
		auto instance = getInstance();
		std::lock_guard<std::mutex> lock(getSingletonMutex());
		auto algoIt = instance.m_algoMachines.find(algoName);
		if (algoIt == instance.m_algoMachines.end())
			return std::optional<IAlgo::Ptr>();
		return (*algoIt->second)(config);
	}

	AlgoFactory::AlgoFactory() {}

	AlgoFactory& AlgoFactory::getInstance() {
		static AlgoFactory instance;
		return instance;
	}



} // end namespace space