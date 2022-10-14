#include <iostream>

#include "InputAnalizer.h"
#include "Kmap.h"

int main()
{
	analizer::InputAnalizer input_analizer(1, "2", "4");
	
	std::string variable;
	variable = input_analizer.input("Enter number of variables [Currently supports only 2, 3, 4]: ");

	int16_t number_of_variables = std::stoi(variable);
	
	input_analizer.set_left_border("0");
	input_analizer.set_right_border("1");
	int16_t type_of_map = std::stoi(input_analizer.input("Enter [1] - SOP or [0] - POS: "));

	input_analizer.set_left_border("0");
	input_analizer.set_right_border(std::to_string(static_cast<int16_t>(pow(2, number_of_variables) - 1)));

	input_analizer.set_enumerate();
	input_analizer.set_max_symbols(38);
	std::string values = input_analizer.input("Enter SOP(where 1) or POS(where 0) values: ");

	kmap::Kmap map(number_of_variables, type_of_map, values);
	map.analize();
	map.colorizing(true);
	map.use_math_operators(false);

	std::cout << "\nInputted boolean expression: ";
	map.print_inputted_expression();

	std::cout << "\n\nTruth table:" << std::endl;
	map.print_truth_table();

	std::cout << "\nK-map:" << std::endl;
	map.print();

	std::cout << "\n\nK-map labels:" << std::endl;
	map.print(true);

	std::cout << "\n\nGroups:";
	map.print_groups();

	std::cout << "\nMinimized boolean expression: ";
	map.print_minimize_expression();

	// If we want to enter truth table value by hand

	//std::string alphabelt = "ABCDF";

	//// Output truth table header
	//std::cout << "Truth table:" << std::endl;
	//std::string out_string = "   ";

	//if (number_of_variables > 3)
	//	out_string += " ";

	//for (int16_t  i = 0; i < number_of_variables; ++i)
	//{
	//	out_string = out_string + alphabelt[i] + " ";
	//}
	//out_string += alphabelt.back();
	//std::cout << out_string << std::endl;

	//input_analizer.set_left_border("0");
	//input_analizer.set_right_border("1");

	//int16_t rows_in_table = pow(2, number_of_variables);
	//std::string current_row;
	//int16_t current_function_value;
	//std::vector<int16_t> function_values;
	//function_values.reserve(rows_in_table);

	//// Output truth table
	//std::string temp;
	//for (int16_t i = 0; i < rows_in_table; ++i)
	//{
	//	temp = std::to_string(i);
	//	if (rows_in_table > 9 && temp.size() == 1)
	//		temp = " " + temp;

	//	current_row = convert_to_binary(i, number_of_variables);
	//	current_function_value = std::stoi(input_analizer.input(temp + "] " + split_string_by_space(current_row)));
	//	function_values.push_back(current_function_value);
	//}

	// Output K-Map
	/*kmap::Kmap map(number_of_variables, function_values);
	map.analize();

	std::cout << "\n\nK-map:" << std::endl;
	map.print();

	std::cout << "\n\nK-map labels:" << std::endl;
	map.print(true);

	std::cout << "\n\nGroups:";
	map.print_groups();

	std::cout << "\nMinimized boolean expression: ";
	map.print_minimize_expression();*/

	return 0;
}