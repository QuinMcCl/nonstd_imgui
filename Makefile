LIB_DIR = $(PWD)/external
DEPS = nonstd nonstd_glfw_opengl 
LIB_DIRS =     $(foreach d, $(DEPS), $(LIB_DIR)/$d) $(LIB_DIR)/glew 
LIB_INCLUDES = $(foreach d, $(LIB_DIRS), $d/include) 

LIBSCLEAN=$(addsuffix clean,$(LIB_DIRS))
LIBSfCLEAN=$(addsuffix fclean,$(LIB_DIRS))
LIBSALL=$(addsuffix all,$(LIB_DIRS))

LIB_NAME = libnonstd_imgui

INC_DIR = include
SRC_DIR = src
IMGUI_DIR = external/imgui
OBJ_DIR = obj
LIB_BIN_DIR = lib

EXE = $(LIB_BIN_DIR)/$(LIB_NAME).a
SRC = $(wildcard $(SRC_DIR)/*.cpp)
IMGUI_SRC += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
IMGUI_BACKEND_SRC += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# OBJ = $(addsuffix .o, $(OBJ_DIR)/$(basename $(notdir $(SRC))))
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ += $(IMGUI_SRC:$(IMGUI_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ += $(IMGUI_BACKEND_SRC:$(IMGUI_DIR)/backends/%.cpp=$(OBJ_DIR)/%.o)

# CPPFLAGS = -Iinclude -MMD -MP -Ofast
CFLAGS   = -Wall -Wextra -g -fpic 
LDFLAGS  = $(foreach d, $(LIB_DIRS), -L $d/lib) -shared 
LDLIBS   = $(foreach d, $(DEPS), -l$d) -lGL -lglfw -lGLEW
INCLUDES = $(foreach d, $(LIB_INCLUDES), -I$d) -I ./external/imgui -I  ./external/imgui/backends

.PHONY: all clean  fclean re
all: $(LIBSALL) $(EXE)

$(EXE): $(OBJ) | $(LIB_BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@ $(INCLUDES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(IMGUI_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(IMGUI_DIR)/backends/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB_BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean: $(LIBSCLEAN)
	@$(RM) -rv $(LIB_BIN_DIR) $(OBJ_DIR)

fclean: $(LIBSfCLEAN) clean
	rm -f $(EXE)

re: fclean | $(EXE)

%clean: %
	$(MAKE) -C $< clean

%fclean: %
	$(MAKE) -C $< fclean

%all: %
	$(MAKE) -C $< all

-include $(OBJ:.o=.d)