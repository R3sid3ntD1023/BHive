#include "UndoRedo.h"

namespace BHive
{
	void UndoRedo::AddCommand(const std::string &name, const std::shared_ptr<ICommand> &cmd)
	{
		if (mCommandPtr < mCommandSize - 1)
		{
			for (int i = mCommandSize - 1; i > mCommandPtr; i--)
			{
				mCommands[i] = {};
			}

			mCommandSize = mCommandPtr + 1;
		}

		mCommands[mCommandSize] = {name, cmd};
		mCommandSize++;

		if (mCommandSize > 1 && mCommands[mCommandSize - 1].Command->CanMerge() && mCommands[mCommandSize - 2].Command->CanMerge())
		{
			if (mCommands[mCommandSize - 1].Command->Merge(mCommands[mCommandSize - 2].Command))
			{
				mCommands[mCommandSize - 1] = {};
				mCommandSize--;
			}
		}

		mCommandPtr = mCommandSize - 1;

		LOG_TRACE("Added Command : {}", name);
	}

	void UndoRedo::Undo()
	{

		if (mCommandPtr >= 0)
		{
			mCommands[mCommandPtr].Command->OnUndo();
			mCommandPtr--;
		}
	}

	void UndoRedo::Redo()
	{
		int command = mCommandPtr + 1;
		if (command < mCommandSize && command >= 0)
		{
			mCommands[command].Command->OnRedo();
			mCommandPtr++;
		}
	}

	void UndoRedo::Clear()
	{
		mCommands.clear();
	}

	UndoRedo::CommandList::iterator UndoRedo::begin()
	{
		return mCommands.begin();
	}

	UndoRedo::CommandList::iterator UndoRedo::end()
	{
		return mCommands.begin() + mCommandSize;
	}

} // namespace BHive