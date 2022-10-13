#include "InputAnalizer.h"

std::string analizer::InputAnalizer::input(const std::string& title)
{
	char key;
	std::string variable;
	std::string current_number;
	
	do
	{
		std::cout << "\r" << title << variable;
		key = _getch();

		// Allow to input only number and delimiter
		if (key >= '0' && key <= '9' || (_enumerate && key == _delimiter))
		{
			if (key == _delimiter)
			{
				// Clear buffer for previous number
				current_number.clear();

				// If we already have this delimiter in the our string, that ignore him
				if (variable.size() > 0)
				{
					if (variable.back() != _delimiter)
						variable += key;
				}
			}
			else
			{
				variable += key;
			}

			// If current char is not equal delimiter, that add it to the number buffer
			if (_enumerate && key != _delimiter)
			{
				current_number += key;
				if (_right_border.size() <= current_number.size())
				{
					// If current number buffer > max value, that delete last symbol
					if (std::stoi(_right_border) < std::stoi(current_number))
					{
						current_number.pop_back();
						variable.pop_back();
					}
				}
			}
			else
			{
				// If buffer > max length, that delete last symbol
				if (!in_range(variable))
				{
					variable.pop_back();

					if (current_number.size())
						current_number.pop_back();
				}
			}
		}

		// If Backspace was pressed, that delete last symbol
		if (key == KeyCode::VK_BACKSPACE && variable.size() > 0)
		{
			variable.pop_back();
			if (current_number.size())
				current_number.pop_back();

			std::cout << "\b ";
		}

	} while (key != KeyCode::VK_RETURN || variable.size() == 0);

	// Check our lower border, if buffer < left border, that increase buffer to left border
	if (std::stoi(_left_border) > std::stoi(variable))
	{
		variable = _left_border;
		std::cout << "\r" << title << variable;
	}

	std::cout << "\n";
	return variable;
}

bool analizer::InputAnalizer::in_range(const std::string& buffer)
{
	return (buffer.size() != _max_symbols) ? (_max_symbols > buffer.size()) : (std::stoi(_right_border) >= std::stoi(buffer));
}
