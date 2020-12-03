#pragma once

#include <optional>
#include <string>

bool is_piece_char(const char c);
std::string get_fen_from_board(const char (&array)[8][8]);
int fill_board_from_fen(char (&array)[8][8], const char *fen);
void reverse_piece_color(char & c);


