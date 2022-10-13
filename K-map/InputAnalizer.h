#ifndef _INPUT_ANALIZER_H_
#define _INPUT_ANALIZER_H_

#include <conio.h>
#include <cstdint>
#include <string>
#include <iostream>

namespace analizer
{
	enum KeyCode
	{
		VK_ESCAPE    = 27,
		VK_RETURN	 = 13,
		VK_BACKSPACE = 8

	};

	class InputAnalizer
	{
	public:
		InputAnalizer(uint16_t max_symbols, const std::string& left_border, const std::string& right_border) : _max_symbols(max_symbols),
			_left_border(left_border), _right_border(right_border), _enumerate(false), _delimiter(' ') {};

		void set_delimiter(char delimiter) { _delimiter = delimiter; }
		char get_delimiter() { return _delimiter; }

		/// <summary>
		/// Enable or Disable enumerate mode. On this mode you can input few numbers by onetime
		/// </summary>
		void set_enumerate() { _enumerate = !_enumerate; }

		/// <summary>
		/// Set max length of the input string.
		/// </summary>
		/// <param name="max_symbols"></param>
		void set_max_symbols(uint16_t max_symbols) { _max_symbols = max_symbols; }
		
		/// <summary>
		/// Set minimized number in the input string.
		/// </summary>
		/// <param name="left_border"></param>
		void set_left_border(const std::string& left_border) { _left_border = left_border; }
		
		/// <summary>
		/// Set maximized number in the input string.
		/// </summary>
		/// <param name="right_border"></param>
		void set_right_border(const std::string& right_border) { _right_border = right_border; }
		
		/// <summary>
		/// Use this method for input value.
		/// </summary>
		/// <param name="title"> - Output title string. (For example: Enter something...)</param>
		/// <returns>Inputted value</returns>
		std::string input(const std::string& title = "");

	private:
		bool in_range(const std::string& buffer);

	private:
		uint16_t _max_symbols{};
		std::string _left_border{}, _right_border{};

		bool _enumerate{};
		char _delimiter{};
	};
}
#endif // !_INPUT_ANALIZER_H_
