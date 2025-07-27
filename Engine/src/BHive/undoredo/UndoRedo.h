#pragma once

#include "Command.h"

namespace BHive
{

	struct UndoRedo
	{
		struct FCommand
		{
			std::string Name = "";

			FHistoryCommand *CommandPtr = nullptr;
		};

		void init(int16_t size);

		void shutdown();

		void add_history_command(const std::string &name, FHistoryCommand *command);

		void undo();

		void redo();

		uint16_t get_max_size() const { return mMaxHistorySize; }

		uint16_t get_command_count() const { return mCommandCount; }

		uint16_t get_current_command_index() const { return mCurrentCommandIndex; }

		const FCommand &get_command_at(uint16_t index) const;

		template <typename T, typename... TArgs>
		void add_history_command(const std::string &name, TArgs &&...args)
		{
			add_history_command(name, new T(std::forward<TArgs>(args)...));
		}

	private:
		FCommand *mCommands = nullptr;

		uint16_t mMaxHistorySize = 0;
		uint16_t mCommandCount = 0;
		uint16_t mCurrentCommandIndex = 0;
	};
} // namespace BHive