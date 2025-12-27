GCC = g++
CFLAGS = -Wall -std=c++23 -I./ImGui_OpenGL/include -mwindows
LDFLAGS = -L./ImGui_OpenGL/lib -lglfw3dll -lopengl32 -lgdi32 -ldwmapi -lglew32 -lshell32 -lshlwapi -lole32 -luuid

TARGET = SearchBar.exe
RESOURCE = app.res

SOURCES = $(wildcard ./src/*.cpp)

IMGUI_SOURCES = ImGui_OpenGL/imgui.cpp \
                ImGui_OpenGL/imgui_demo.cpp \
                ImGui_OpenGL/imgui_draw.cpp \
                ImGui_OpenGL/imgui_tables.cpp \
                ImGui_OpenGL/imgui_widgets.cpp \
                ImGui_OpenGL/imgui_impl_glfw.cpp \
                ImGui_OpenGL/imgui_impl_opengl3.cpp

all: $(RESOURCE)
	$(GCC) $(CFLAGS) $(SOURCES) $(IMGUI_SOURCES) $(RESOURCE) -o $(TARGET) $(LDFLAGS)

$(RESOURCE): app.rc search_bar_icon.ico
	windres app.rc -O coff -o $(RESOURCE)

clean:
	del $(TARGET) $(RESOURCE)

rebuild: clean all