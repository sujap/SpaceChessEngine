#pragma once

#include "algo.h"
#include <nlohmann/json.hpp>
#include <optional>

namespace space {

	class AlgoFactory {
	public:
		using AlgoMachine = IAlgo::Ptr(*)(const nlohmann::json&);
		static bool registerAlgoMachine(const std::string& algoName, AlgoMachine algoMachine);
		static std::optional<IAlgo::Ptr> tryCreateAlgo(const nlohmann::json& config);

		static std::string AlgoNameField;

	private:
		std::map<std::string, AlgoMachine> m_algoMachines;

		AlgoFactory();
		static AlgoFactory& getInstance();
		
	};

} // end namespace space