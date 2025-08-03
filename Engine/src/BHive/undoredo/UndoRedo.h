#pragma once

#include "Command.h"

namespace BHive
{

	struct UndoRedo
	{
		struct FCommand
		{
			std::string Name = "";

			ICommand *Command = nullptr;

			~FCommand() { delete Command; }
		};

		struct Iterator
		{
			using iterator_category = std::forward_iterator_tag;
			using differnce_type = std::ptrdiff_t;
			using value_type = FCommand;
			using pointer = FCommand **;
			using reference = FCommand *;

			Iterator(pointer ptr)
				: m_ptr(ptr)
			{
			}

			reference operator*() const { return *m_ptr; }
			pointer operator->() { return m_ptr; }
			Iterator &operator++()
			{
				m_ptr++;
				return *this;
			}
			Iterator &operator++(int)
			{
				Iterator temp = *this;
				++(*this);
				return temp;
			}

			operator bool() const { return m_ptr != nullptr; }
			friend bool operator==(const Iterator &a, const Iterator &b) { return a.m_ptr == b.m_ptr; }

		private:
			pointer m_ptr;
		};

		void add_history_command(const std::string &name, ICommand *command);

		void undo();

		void redo();

		void clear();

		int32_t get_command_size() const { return mCommandSize; }

		int32_t get_current_command_index() const { return mCommandPtr; }

		template <typename T, typename... TArgs>
		void add_history_command(const std::string &name, TArgs &&...args)
		{
			add_history_command(name, new T(std::forward<TArgs>(args)...));
		}

		Iterator begin();

		Iterator end();

	private:
		FCommand *mCommands[1000] = {};

		int32_t mCommandSize = 0;
		int32_t mCommandPtr = 0;
	};
} // namespace BHive