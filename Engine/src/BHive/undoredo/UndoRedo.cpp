#include "UndoRedo.h"

namespace BHive
{
	void UndoRedo::init(int16_t size)
	{
		mMaxHistorySize = size;
		mCommands = new FCommand[size + 1];
	}

	void UndoRedo::shutdown()
	{
		delete[] mCommands;
	}

	const UndoRedo::FCommand &UndoRedo::get_command_at(uint16_t index) const
	{
		ASSERT(index < mCommandCount);

		return mCommands[index];
	}

	void UndoRedo::add_history_command(const std::string &name, FHistoryCommand *command)
	{
		ASSERT(mCommands, "Did you call undoredo::init()?");

		mCommands[mCurrentCommandIndex].Name = name;
		mCommands[mCurrentCommandIndex].CommandPtr = command;
		mCurrentCommandIndex++;

		mCommandCount++;
	}

	void UndoRedo::undo()
	{
		if (mCurrentCommandIndex <= 0)
			return;

		mCommands[mCurrentCommandIndex - 1].CommandPtr->OnUndo();
		mCurrentCommandIndex--;
	}

	void UndoRedo::redo()
	{
		if (mCurrentCommandIndex >= mCommandCount)
			return;

		mCurrentCommandIndex++;
		mCommands[mCurrentCommandIndex - 1].CommandPtr->OnRedo();
	}

	void UndoRedo::clear()
	{
		delete[] mCommands;

		mCommands = new FCommand[mMaxHistorySize + 1];
		mCurrentCommandIndex = 0;
		mCommandCount = 0;
	}

} // namespace BHive