#include <cstdlib>
#include <cctype>
#include <string>
#include <optional>


void reverse_piece_color(char & c)
{
	if (islower(c))
		c = toupper(c);
	else
		c = tolower(c);
}

bool is_piece_char(const char c)
{
	static std::string s = "rnbqkpRNBQKP";

	return (s.find(c) != std::string::npos);
}

int fill_board_from_fen(char (&array)[8][8], const char *fen)
{
	int x = 0;
	int y = 0;

	while (*fen && *fen != ' ')
	{
		if (std::isdigit(*fen))
			x += std::atoi(fen);
		else if (is_piece_char(*fen))
			array[x++][y] = *fen;
		else if (*fen == '/')
		{
			x = 0;
			y++;
		}
		else
		{
			dprintf(2, "Invalid FEN\n");
			return (1);
		}
		fen++;
	}
	return (0);
}

std::string get_fen_from_board(const char (&array)[8][8])
{
	std::string fen = "";

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			int empty_squares = 0;
			while (x < 8 && array[x][y] == 0) 
			{
				empty_squares++;
				x++;
			}
			if (empty_squares != 0)
			{
				fen.append(std::to_string(empty_squares));
				x--;
			}
			else
				fen += array[x][y];
		}
		if (y != 7)
			fen += '/';
	}
	return (fen);
}
