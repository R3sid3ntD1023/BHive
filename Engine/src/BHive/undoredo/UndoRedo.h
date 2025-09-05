#pragma once

#include "Command.h"

namespace BHive
{

	struct BHIVE_API UndoRedo
	{
		struct FCommand
		{
			std::string Name = "";

			std::shared_ptr<ICommand> Command;
		};

		using CommandList = std::vector<FCommand>;

		void AddCommand(const std::string &name, const std::shared_ptr<ICommand> &command);

		void Undo();

		void Redo();

		void Clear();

		int32_t GetCommandSize() const { return mCommandSize; }

		int32_t GetCurrentCommandIndex() const { return mCommandPtr; }

		template <typename T, typename... TArgs>
		void AddCommand(const std::string &name, TArgs &&...args)
		{
			AddCommand(name, std::make_shared<T>(std::forward<TArgs>(args)...));
		}

		CommandList::iterator begin();

		CommandList::iterator end();

	private:
		CommandList mCommands{1000};

		int32_t mCommandSize = 0;
		int32_t mCommandPtr = 0;
	};
} // namespace BHive