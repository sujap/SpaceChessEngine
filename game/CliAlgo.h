#pragma once
#include <chess/algo.h>
#include <iostream>

namespace space {
	class CliAlgo : public IAlgo {
	public:
		using Ptr = std::shared_ptr<CliAlgo>;
		Move getNextMove(IBoard::Ptr board) override;
		CliAlgo(std::istream& inputStream, std::ostream& outputStream) :
			m_inputStream(inputStream),
			m_outputStream(outputStream)
		{ }

		static IAlgo::Ptr create(std::istream& inputStream, std::ostream& outputStream)
		{
			return std::make_shared<CliAlgo>(inputStream, outputStream);
		}

	private:
		std::istream& m_inputStream;
		std::ostream& m_outputStream;
	};
}
