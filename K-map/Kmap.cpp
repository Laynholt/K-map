#include "Kmap.h"

kmap::Kmap::Kmap(int16_t number_of_variables, bool is_sop, std::string& parse_string, char delimiter) : _is_sop(is_sop),
_number_of_variables(number_of_variables), _is_colorized(true), _is_math(true)
{
	// Delete delimiter in the end
	if (parse_string.back() == delimiter)
		parse_string.pop_back();

	std::set<int16_t> values;
	std::stringstream ss(parse_string);
	std::string item;

	// Parse string
	while (std::getline(ss, item, delimiter)) 
		values.insert(std::stoi(item));

	// Filling truth table F values 
	int16_t size = pow(2, number_of_variables);
	truth_table_func_values.reserve(size);
	for (int16_t i = 0; i < size; ++i)
		truth_table_func_values.push_back((is_sop) ? 0 : 1);

	for (auto value : values)
	{
		if (value < truth_table_func_values.size())
		{
			truth_table_func_values[value] = (is_sop) ? 1 : 0;
		}
	}

	// Filling matrix for K-map
	matrix = std::make_unique<int16_t[]>(pow(2, number_of_variables));

	if (number_of_variables == 2) { _rows = 2; _cols = 2; }
	else if (number_of_variables == 3) { _rows = 4; _cols = 2; }
	else if (number_of_variables == 4) { _rows = 4; _cols = 4; }

	int16_t gray_i, gray_j;
	for (int16_t i = 0; i < _rows; ++i)
	{
		// Conver indexes to gray code
		gray_i = i ^ (i >> 1);
		for (int16_t j = 0; j < _cols; ++j)
		{
			gray_j = j ^ (j >> 1);
			matrix[gray_i * _cols + gray_j] = truth_table_func_values[i * _cols + j];
		}
	}

	// Filling alphabelt
	for (int16_t i = 0; i < number_of_variables; ++i)
	{
		alphabelt += ('A' + i);
	}
	alphabelt += ((alphabelt.back() >= 'F') ? (alphabelt.back() + 1) : 'F');
}

kmap::Kmap::Kmap(int16_t number_of_variables, const std::vector<int16_t>& truth_table_values) :_is_sop(true),
_number_of_variables(number_of_variables), _is_colorized(true), _is_math(true)
{
	truth_table_func_values = truth_table_values;
	matrix = std::make_unique<int16_t[]>(pow(2, number_of_variables));

	if (number_of_variables == 2) { _rows = 2; _cols = 2; }
	else if (number_of_variables == 3) { _rows = 4; _cols = 2; }
	else if (number_of_variables == 4) { _rows = 4; _cols = 4; }

	int16_t gray_i, gray_j;
	for (int16_t i = 0; i < _rows; ++i)
	{
		gray_i = i ^ (i >> 1);
		for (int16_t j = 0; j < _cols; ++j)
		{
			gray_j = j ^ (j >> 1);
			matrix[gray_i * _cols + gray_j] = truth_table_values[i * _cols + j];
		}
	}
}

void kmap::Kmap::analize()
{
	// Check all elements on the matrix and try to group them
	int16_t value = (_is_sop) ? 1 : 0;
	for (int16_t i = 0; i < _rows; ++i)
	{
		for (int16_t j = 0; j < _cols; ++j)
		{
			if (at(i, j) == value)
			{
				find_group_for_element(i, j);
			}
		}
	}

	// Delete dublicates groups and useless groups
	delete_dublicates();
	optimize();
	std::sort(groups.begin(), groups.end(), [](const Group& g1, const Group& g2) { return g1.group.size() < g2.group.size(); });
	
	if (_is_colorized)
		colorize();
}

void kmap::Kmap::print_truth_table()
{
	// Output truth table header
	std::string out_string = "   ";

	if (_number_of_variables > 3)
		out_string += " ";

	for (int16_t  i = 0; i < _number_of_variables; ++i)
	{
		out_string = out_string + alphabelt[i] + " ";
	}
	out_string += alphabelt.back();
	std::cout << out_string << std::endl;

	int16_t rows_in_table = pow(2, _number_of_variables);
	std::string current_row;

	// Output truth table
	std::string shift;
	for (int16_t i = 0; i < rows_in_table; ++i)
	{
		shift = std::to_string(i);
		if (rows_in_table > 9 && shift.size() == 1)
			shift = " " + shift;

		current_row = convert_to_binary(i, _number_of_variables);
		std::cout << shift << "] " << split_string_by_space(current_row) << truth_table_func_values[i] << std::endl;
	}
}

void kmap::Kmap::print(bool print_label)
{
	int16_t variables_in_row = std::log2(_rows);
	int16_t variables_in_col = std::log2(_cols);

	std::cout << std::setw(_rows) << " ";

	// Print columns headers
	int16_t gray_col;
	for (int16_t i = 0; i < _cols; ++i)
	{
		gray_col = i ^ (i >> 1);
		for (int16_t j = 0; j < variables_in_col; ++j)
		{
			std::cout << (((gray_col >> (variables_in_col - 1 - j)) & 1) ? " " : "!") << alphabelt[variables_in_row + j];
		}
		std::cout << " ";
	}
	std::cout << std::endl;

	// Print rows headers and data
	int16_t gray_row;
	for (int16_t i = 0; i < _rows; ++i)
	{
		// Print headers
		gray_row = i ^ (i >> 1);
		for (int16_t j = 0; j < variables_in_row; ++j)
		{
			std::cout << (((gray_row >> (variables_in_row - 1 - j)) & 1) ? " " : "!") << alphabelt[j];
		}

		// Print data
		for (int16_t j = 0; j < _cols; ++j)
		{
			gray_col = j ^ (j >> 1);

			uint16_t color = get_color_by_value(gray_row * _cols + gray_col);
			std::string prefix, postfix;

			if (is_fg_color(color))
			{
				prefix = "\x1B[" + std:: to_string(color) + "m";
				postfix = "\033[0m";
			}
			else
			{
				prefix = "\033[3;" + std::to_string(color) + ";30m";
				postfix = "\033[0m";
			}

			// Print data or indexes
			if (!print_label)
				std::cout  << prefix << std::setw(_cols) << matrix[i * _cols + j] << postfix << " ";
			else
				std::cout << prefix << std::setw(_cols) << gray_row * _cols + gray_col << postfix << " ";
		}

		std::cout << "\n";
	}

}

void kmap::Kmap::print_groups()
{
	std::cout << "\n";
	std::string prefix, postfix;

	for (auto& group : groups)
	{
		switch (group.group.size()) 
		{
		case 1: std::cout << "Unit:   "; break;
		case 2: std::cout << "Pair:   "; break;
		case 4: std::cout << "Quad:   "; break;
		case 8: std::cout << "Octant: "; break;
		}

		for (auto it = group.group.begin(); it != group.group.end(); ++it)
		{
			if (is_fg_color(group.color))
			{
				prefix = "\x1B[" + std::to_string(group.color) + "m";
				postfix = "\033[0m";
			}
			else
			{
				prefix = "\033[3;" + std::to_string(group.color) + ";30m";
				postfix = "\033[0m";
			}
			std::cout << prefix << std::setw(2) << *it << postfix << " ";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}

void kmap::Kmap::print_inputted_expression()
{
	std::string logical_operators[2] = { " v ", " ^ " };
	std::string math_operators[2] = { " + ", " " };

	bool first_print = true;

	int16_t i = 0;
	for (auto value : truth_table_func_values)
	{
		if ((_is_sop && value == 0) || (!_is_sop && value == 1))
		{
			++i;
			continue;
		}

		int16_t bits = _number_of_variables;
		int16_t current_bit = i;
		std::string word_sop;
		std::string word_pos;
		
		if (!first_print)
		{
			if (!_is_sop)
				std::cout << ((_is_math) ? math_operators[1] : logical_operators[1]);
			else
				std::cout << ((_is_math) ? math_operators[0] : logical_operators[0]);
		}

		// Output from lower bit to higt
		// From expample: 1010 = A!BC!D (SOP)
		// Input ->     !D
		//		 ->    C!D
		//		 ->  !BC!D
		//		 -> A!BC!D
		while (bits)
		{
			if (!_is_sop)
			{
				if (word_pos.size() > 0)
					word_pos = ((_is_math) ? math_operators[0] : logical_operators[0]) + word_pos;

				word_pos = alphabelt[bits - 1] + word_pos;
				word_pos = (current_bit & 1) ? ("!" + word_pos) : (word_pos);
			}
			else
			{
				word_sop = alphabelt[bits - 1] + word_sop;
				word_sop = (current_bit & 1) ? (word_sop) : ("!" + word_sop);
			}
			current_bit >>= 1;
			--bits;
		}

		if (!_is_sop)
			std::cout << "( " << word_pos << " )";
		else
			std::cout << word_sop;
		
		first_print = false;
		++i;
	}
}

void kmap::Kmap::print_minimize_expression()
{
	std::string logical_operators[2] = { " v ", " ^ " };
	std::string math_operators[2] = { " + ", " " };
	std::string prefix, postfix;
	bool first_print = true;

	for (auto& group : groups)
	{
		if (is_fg_color(group.color))
		{
			prefix = "\x1B[" + std::to_string(group.color) + "m";
			postfix = "\033[0m";
		}
		else
		{
			prefix = "\033[3;" + std::to_string(group.color) + ";30m";
			postfix = "\033[0m";
		}

		// Output operators +, v, ^
		if (!first_print)
		{
			if (!_is_sop)
				std::cout << ((_is_math) ? math_operators[1] : logical_operators[1]);
			else
				std::cout << ((_is_math) ? math_operators[0] : logical_operators[0]);
		}

		std::string word_sop;
		std::string word_pos;
		// For one element
		if (group.group.size() == 1)
		{
			int16_t bits = _number_of_variables;
			int16_t value = *group.group.begin();

			while (bits)
			{
				if (!_is_sop)
				{
					if (word_pos.size() > 0)
						word_pos = ((_is_math) ? math_operators[0] : logical_operators[0]) + word_pos;

					word_pos = alphabelt[bits - 1] + word_pos;
					word_pos = (value & 1) ? ("!" + word_pos) : (word_pos);
				}
				else
				{
					word_sop = alphabelt[bits - 1] + word_sop;
					word_sop = (value & 1) ? (word_sop) : ("!" + word_sop);
				}
				value >>= 1;
				--bits;
			}

			if (!_is_sop)
				std::cout << prefix << "( " << word_pos << " )" << postfix;
			else
				std::cout << prefix << word_sop << postfix;
		}
		else
		{// For group
			int16_t bits = _number_of_variables;

			while (bits)
			{
				int16_t buf = (*group.group.begin() >> _number_of_variables - bits) & 1;
				bool is_bit_up = (buf) ? true : false;

				bool save_bit = true;

				// If all values int the group have this bit (varibale) => print this
				for (auto& value : group.group)
				{
					if (((value >> _number_of_variables - bits) & 1) != buf)
					{
						save_bit = false;
						break;
					}
				}

				if (save_bit)
				{
					if (!_is_sop)
					{
						// Add operator +
						if (word_pos.size() > 0)
							word_pos = ((_is_math) ? math_operators[0] : logical_operators[0]) + word_pos;

						// Add variable (A, B...)
						word_pos = alphabelt[bits - 1] + word_pos;
						// Add not
						word_pos = (is_bit_up) ? ("!" + word_pos) : (word_pos);
					}
					else
					{
						// For Sop we dont need to add ^ operator between variables, so we simply add only variable and not
						word_sop = alphabelt[bits - 1] + word_sop;
						word_sop = (is_bit_up) ? (word_sop) : ("!" + word_sop);
					}
				}
				--bits;
			}

			if (!_is_sop)
				std::cout << prefix << "( " << word_pos << " )" << postfix;
			else
				std::cout << prefix << word_sop << postfix;
		}
		first_print = false;
	}
}

int16_t kmap::Kmap::at(int16_t i, int16_t j)
{
	if (i >= 0 && i < _rows && j >= 0 && j < _cols)
		return matrix[i * _cols + j];

	i = (i + _rows) % _rows;
	j = (j + _cols) % _cols;
	return matrix[i * _cols + j];
}

int16_t kmap::Kmap::at_index(int16_t i, int16_t j)
{
	int16_t gray_row, gray_col, element;
	if (i >= 0 && i < _rows && j >= 0 && j < _cols)
	{
		gray_row = i ^ (i >> 1);
		gray_col = j ^ (j >> 1);
		element = gray_row * _cols + gray_col;
		return element;
	}

	i = (i + _rows) % _rows;
	j = (j + _cols) % _cols;
	gray_row = i ^ (i >> 1);
	gray_col = j ^ (j >> 1);
	element = gray_row * _cols + gray_col;

	return element;
}

void kmap::Kmap::delete_dublicates()
{
	std::sort(groups.begin(), groups.end(), [](const Group& g1, const Group& g2) { 
		
		if (g1.group.size() != g2.group.size())
			return g1.group.size() < g2.group.size();

		auto it1 = g1.group.begin();
		auto it2 = g2.group.begin();

		for (; it1 != g1.group.end(); ++it1, ++it2)
		{
			if (*(it1) != *(it2))
				return *(it1) < *(it2);
		}

	});
	groups.erase(std::unique(groups.begin(), groups.end()), groups.end());
}

void kmap::Kmap::optimize()
{
	std::set<int16_t> values_without_current;

	for (int16_t i = 0; i <groups.size(); ++i)
	{
		for (int16_t j = 0; j < groups.size(); ++j)
		{
			if (groups[i] == groups[j])
				continue;

			// Filling set by values of others groups (dont add values of the current group)
			for (auto it = groups[j].group.begin(); it != groups[j].group.end(); ++it)
				values_without_current.insert(*it);
		}

		// Find values of the current group in the our set
		int16_t count = 0;
		for (auto it = groups[i].group.begin(); it != groups[i].group.end(); ++it)
		{
			auto result = std::find(values_without_current.begin(), values_without_current.end(), *it);

			// If found that increase counter
			if (result != values_without_current.end())
				++count;
		}

		// If all our values in the current group we have in another groups then
		// We should optimize this (delete)
		if (count == groups[i].group.size())
		{
			groups.erase(groups.begin() + i);
			--i;
		}
		values_without_current.clear();
	}
}

void kmap::Kmap::colorize()
{
	uint16_t color_fg = color::FG_Red;
	uint16_t color_bg = color::BG_Red;

	for (auto& group : groups)
	{
		if (group.group.size() == 1)
			group.color = color_fg++;
		else
			group.color = color_bg++;

		if (color_fg > color::FG_White)
			color_fg = color::FG_Bright_Red;
		else if (color_fg > color::FG_Bright_Red)
			color_fg = color::FG_Red;

		if (color_bg > color::BG_White)
			color_bg = color::BG_Bright_Red;
		else if (color_bg > color::BG_Bright_Red)
			color_bg = color::BG_Red;
	}
}

uint16_t kmap::Kmap::get_color_by_value(int16_t value)
{
	for (auto& group : groups)
	{
		auto result = std::find(group.group.begin(), group.group.end(), value);
		if (result != group.group.end())
			return group.color;
	}
	return color::FG_Bright_White;
}

bool kmap::Kmap::is_fg_color(uint16_t color)
{
	if ((color >= color::FG_Black && color <= color::FG_White) || (color >= color::FG_Bright_Black && color <= color::FG_Bright_White))
		return true;
	return false;
}

void kmap::Kmap::find_group_for_element(int16_t i, int16_t j)
{
	// Search in four directions
	bool was_grouped = false;
	bool result;

	// Check pairs
	std::vector<std::vector<int16_t>> directions_pairs = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };
	was_grouped = find_group(i, j, 2, directions_pairs, 1);

	// Check quads
	std::vector<std::vector<int16_t>> directions_quads1 = { {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1} };
	std::vector<std::vector<int16_t>> directions_quads2 = { {0, -1}, {0, -2}, {0, -3}, {-1, 0}, {-2, 0}, {-3, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {2, 0}, {3, 0} };
	result = find_group(i, j, 4, directions_quads1, 2);
	was_grouped = (result) ? result : was_grouped;

	result = find_group(i, j, 4, directions_quads2, 3);
	was_grouped = (result) ? result : was_grouped;

	// Check octs
	std::vector<std::vector<int16_t>> directions_octs = {
		{0, -1}, {0, -2}, {0, -3}, {1, 0}, {1, -1}, {1, -2}, {1, -3},		// left
		{-1, 0}, {-2, 0}, {-3, 0}, {0, -1}, {-1, -1}, {-2, -1}, {-3, -1},	// top
		{0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3},				// right
		{1, 0}, {2, 0}, {3, 0}, {0, -1}, {1, -1}, {2, -1}, {3, -1}			// bottom
	};

	result = find_group(i, j, 8, directions_octs, 7);
	was_grouped = (result) ? result : was_grouped;

	if (!was_grouped)
	{
		groups.push_back(Group());
		groups.back().group.insert(at_index(i, j));
	}
}

bool kmap::Kmap::find_group(int16_t i, int16_t j, int16_t group_size, const std::vector<std::vector<int16_t>>& directions, int16_t step)
{
	bool was_grouped = false;
	bool is_group = true;
	int16_t _i, _j, next;
	int16_t value = (_is_sop) ? 1 : 0;

	for (int16_t l = 0, d = 0; l < 4; ++l, d += step)
	{
		for (int16_t t = 0; t < group_size - 1; ++t)
		{
			next = (d + t) % directions.size();
			_i = i + directions[next][0];
			_j = j + directions[next][1];

			if (at(_i, _j) == !value)
			{
				is_group = false;
				break;
			}
		}

		if (is_group)
		{
			was_grouped = true;
			groups.push_back(Group());
			groups.back().group.insert(at_index(i, j));

			for (int16_t t = 0; t < group_size - 1; ++t)
			{
				next = (d + t) % directions.size();
				_i = i + directions[next][0];
				_j = j + directions[next][1];
				groups.back().group.insert(at_index(_i, _j));
			}
		}
		is_group = true;
	}
	return was_grouped;
}

std::string kmap::Kmap::convert_to_binary(int16_t variable, int16_t number_of_iterations)
{
	std::string binary;

	while (number_of_iterations > 0)
	{
		binary = (variable & 1) ? "1" + binary : "0" + binary;
		variable >>= 1;
		--number_of_iterations;
	}
	return binary;
}

std::string kmap::Kmap::split_string_by_space(const std::string& str)
{
	std::string splitted;
	int16_t size = str.size();

	for (int16_t i = 0; i < size; ++i)
	{
		splitted = splitted + str[i] + " ";
	}
	return splitted;
}

