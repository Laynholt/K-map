#ifndef _KMAP_H_
#define _KMAP_H_

#include <memory>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace kmap
{
	namespace color
	{
		enum Colors
		{
			FG_Black = 30,
			FG_Red = 31,
			FG_Green = 32,
			FG_Yellow = 33,
			FG_Blue = 34,
			FG_Magenta = 35,
			FG_Cyan = 36,
			FG_White = 37,
			FG_Bright_Black = 90,
			FG_Bright_Red = 91,
			FG_Bright_Green = 92,
			FG_Bright_Yellow = 93,
			FG_Bright_Blue = 94,
			FG_Bright_Magenta = 95,
			FG_Bright_Cyan = 96,
			FG_Bright_White = 97,

			BG_Black = 40,
			BG_Red = 41,
			BG_Green = 42,
			BG_Yellow = 43,
			BG_Blue = 44,
			BG_Magenta = 45,
			BG_Cyan = 46,
			BG_White = 47,
			BG_Bright_Black = 100,
			BG_Bright_Red = 101,
			BG_Bright_Green = 102,
			BG_Bright_Yellow = 103,
			BG_Bright_Blue = 104,
			BG_Bright_Magenta = 105,
			BG_Bright_Cyan = 106,
			BG_Bright_White = 107
		};
	}

	struct Group
	{
	public:
		std::set<int16_t> group;
		uint16_t color{color::FG_Bright_White};

	public:
		bool operator==(const Group& obj)
		{
			if (group.size() != obj.group.size())
				return false;

			auto it1 = group.begin();
			auto it2 = obj.group.begin();

			for (int16_t i = 0; i < group.size(); ++i, ++it1, ++it2)
			{
				if (*it1 != *it2)
					return false;
			}
			return true;
		}

		Group operator=(const Group& obj)
		{
			group = obj.group;
			color = obj.color;
			return *this;
		}
	};

	class Kmap
	{
	public:
		/// <summary>
		/// Use this constructor to create from SOP or POS string
		/// </summary>
		/// <param name="number_of_variables"></param>
		/// <param name="is_sop"></param>
		/// <param name="parse_string"></param>
		/// <param name="delimiter"></param>
		Kmap(int16_t number_of_variables, bool is_sop, std::string& parse_string, char delimiter = ' ');
		
		/// <summary>
		/// Older constructor. You can use this, if you want to create from truth table
		/// </summary>
		/// <param name="number_of_variables"></param>
		/// <param name="truth_table_values"></param>
		Kmap(int16_t number_of_variables, const std::vector<int16_t>& truth_table_values);

		/// <summary>
		/// Main function, which do all stuff. Call it before all prints.
		/// </summary>
		void analize();
		
		/// <summary>
		/// Output truth table.
		/// </summary>
		void print_truth_table();
		
		/// <summary>
		/// Output K-map.
		/// </summary>
		/// <param name="print_label"> - if its true, then output K-map with indexes.</param>
		void print(bool print_label = false);
		
		/// <summary>
		/// Output groups.
		/// </summary>
		void print_groups();

		/// <summary>
		/// Output inputted boolean expression.
		/// </summary>
		void print_inputted_expression();

		/// <summary>
		/// Output minimized boolean expression.
		/// </summary>
		void print_minimize_expression();

		/// <summary>
		/// Change operators between variables in the boolean expression. (For example: A v B => A + B )
		/// </summary>
		/// <param name="enable"></param>
		void use_math_operators(bool enable) { _is_math = enable; }

		/// <summary>
		/// Colorize groups and K-map output.
		/// </summary>
		/// <param name="enable"></param>
		void colorizing(bool enable) { _is_colorized = enable; }

	private:
		int16_t at(int16_t i, int16_t j);
		int16_t at_index(int16_t i, int16_t j);

		void delete_dublicates();
		void optimize();
		void colorize();

		uint16_t get_color_by_value(int16_t value);
		bool is_fg_color(uint16_t color);

		void find_group_for_element(int16_t i, int16_t j);
		bool find_group(int16_t i, int16_t j, int16_t group_size, const std::vector<std::vector<int16_t>>& directions, int16_t step);

		std::string convert_to_binary(int16_t variable, int16_t number_of_iterations);
		std::string split_string_by_space(const std::string& str);

	private:
		std::unique_ptr<int16_t[]> matrix;
		std::vector<Group> groups;
		std::vector<int16_t> truth_table_func_values;
		std::string alphabelt;

		int16_t _number_of_variables{};
		int16_t _rows{};
		int16_t _cols{};

		bool _is_sop{};
		bool _is_colorized{};
		bool _is_math{};
	};
}

#endif // !_KMAP_H_
