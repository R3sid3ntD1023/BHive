#include "UndoRedo.h"

namespace BHive
{
	void UndoRedo::add_history_command(const std::string &name, ICommand *cmd)
	{
		if (mCommandPtr < mCommandSize - 1)
		{
			for (int i = mCommandSize - 1; i > mCommandPtr; i--)
			{
				delete mCommands[i];
			}

			mCommandSize = mCommandPtr + 1;
		}

		mCommands[mCommandSize] = new FCommand{name, cmd};
		mCommandSize++;

		if (mCommandSize > 1 && mCommands[mCommandSize - 1]->Command->can_merge() && mCommands[mCommandSize - 2]->Command->can_merge())
		{
			if (mCommands[mCommandSize - 1]->Command->merge(mCommands[mCommandSize - 2]->Command))
			{
				delete mCommands[mCommandSize - 1];
				mCommandSize--;
			}
		}

		mCommandPtr = mCommandSize - 1;

		LOG_TRACE("Added Command : {}", name);
	}

	void UndoRedo::undo()
	{

		if (mCommandPtr >= 0)
		{
			mCommands[mCommandPtr]->Command->on_undo();
			mCommandPtr--;
		}
	}

	void UndoRedo::redo()
	{
		int command = mCommandPtr + 1;
		if (command < mCommandSize && command >= 0)
		{
			mCommands[command]->Command->on_redo();
			mCommandPtr++;
		}
	}

	void UndoRedo::clear()
	{
		for (uint16_t i = 0; i < mCommandSize; i++)
		{
			delete mCommands[i];
		}
	}

	UndoRedo::Iterator UndoRedo::begin()
	{
		return Iterator(mCommands[0]);
	}

	UndoRedo::Iterator UndoRedo::end()
	{
		return Iterator(mCommands[mCommandSize]);
	}

} // namespace BHive