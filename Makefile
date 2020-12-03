######################################################################
#                            DEFINITIONS                             #
######################################################################

#------------------------------------------------#
#                     PROJECT                    |
#------------------------------------------------#

#possible values: debug, release
BUILDTYPE       := debug
NAME            := chessOCR 
PROJECT         := chessOCR 

#------------------------------------------------#
#                   LIBRARIES                    |
#------------------------------------------------#

INC_PATH		:= include
LFLAGS          := `pkg-config --libs --cflags opencv4`

#------------------------------------------------#
#                     FLAGS                      |
#------------------------------------------------#

CC              := g++
CFLAGS          := -Wall -Wextra -std=c++20
ifeq ($(BUILDTYPE), debug)
	CFLAGS := $(CFLAGS) $(DBG_FLAGS)
endif
PFLAGS          := -I$(INC_PATH) -I/usr/include/opencv4

#------------------------------------------------#
#                    SOURCES                     |
#------------------------------------------------#

SRC_PATH        := src
SRC_NAME		:= main.cpp\
				   geometry.cpp\
				   partition.cpp\
				   fen.cpp\
				   predict.cpp\
				   draw.cpp
SRC             := $(addprefix $(SRC_PATH)/,$(SRC_NAME))

OBJ_PATH        := obj
OBJ_NAME        := $(SRC_NAME:.cpp=.o)
OBJ             := $(addprefix $(OBJ_PATH)/,$(OBJ_NAME))

#------------------------------------------------#
#                     BUILD                      |
#------------------------------------------------#

BUILD_PATH      := build
OBJ             := $(addprefix $(BUILD_PATH)/,$(OBJ))

#------------------------------------------------#
#                     EXTRA                      |
#------------------------------------------------#

CLEAR           := "\033[0K\n\033[F"
CR              := "\r"$(CLEAR)
EOC             := "\033[0;0m"
RED             := "\033[0;31m"
GREEN           := "\033[0;32m"
BASENAME        := `basename $(PWD)`

######################################################################
#                               RULES                                #
######################################################################

#------------------------------------------------#
#                 BUILD-RULES                    |
#------------------------------------------------#

all: $(NAME)

$(NAME): $(OBJ) $(LIB) 
	$(CC) -o $(NAME) $(OBJ) $(LFLAGS) $(CFLAGS)

$(BUILD_PATH)/$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	@mkdir -p $(dir $@) 2>/dev/null || true
	$(CC) $(CFLAGS) $(PFLAGS) -c $< -o $@

#------------------------------------------------#
#                  CLEAN-RULES                   |
#------------------------------------------------#

soft: clean all

re: fclean all

clean:
	$(RM) -r $(OBJ)
	$(RM) -r $(BUILD_PATH)/$(OBJ_PATH)

fclean: clean
	$(RM) -r $(NAME)

#------------------------------------------------#
#                  OTHER-RULES                   |
#------------------------------------------------#

format:
	clang-format -i $(SRC_PATH)/* $(INC_PATH)/*


	

.PHONY: all, clean, fclean, re, norme
