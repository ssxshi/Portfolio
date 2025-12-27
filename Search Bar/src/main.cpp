#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <thread>

#define GLFW_EXPOSE_NATIVE_WIN32
#include "../ImGui_OpenGL/imgui.h"
#include "../ImGui_OpenGL/imgui_impl_glfw.h"
#include "../ImGui_OpenGL/imgui_impl_opengl3.h"
#include "../ImGui_OpenGL/include/GLFW/glfw3.h"
#include "../include/program_indexer.h"
#include "../ImGui_OpenGL/include/GLFW/glfw3native.h"

using namespace std;

bool open = true;
bool showing = false;

vector<ProgramEntry> searchResults;

const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
const ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_CallbackEdit;

static int search_bar_callback(ImGuiInputTextCallbackData *data){
    searchResults = searchPrograms(string(data->Buf));

    return 0;
}

int main(){
    if (!glfwInit()){
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);

    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);
    int window_w = screen_w / 2;
    int window_h_default = screen_h / 15;
    int window_h_max = window_h_default * 3;
    int pos_x = (screen_w - window_w) / 2;
    int pos_y = (screen_h - window_h_default) / 4;

    GLFWwindow *window = glfwCreateWindow(window_w, window_h_default, "Search Bar", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(1);
    }

    glfwSetWindowPos(window, pos_x, pos_y);

    HWND hwnd = glfwGetWin32Window(window);

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOOLWINDOW;
    exStyle &= ~WS_EX_APPWINDOW;
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    thread indexThread(buildProgramIndex);
    indexThread.detach();

    char buff[256];
    int current_window_h = window_h_default;

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000)) { // ALT
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                showing = !showing;
                if (showing) {
                    buff[0] = '\0';
                    searchResults.clear();
                    glfwFocusWindow(window);
                }
            }

            if (GetAsyncKeyState('C') & 0x8000) {
                glfwSetWindowShouldClose(window, 1);
            }
        }
        
        if (!searchResults.empty()) {
            int button_height = 30;
            int needed_height = window_h_default + (searchResults.size() * button_height) + 20;
            current_window_h = (needed_height > window_h_max) ? window_h_max : needed_height;
        } else {
            current_window_h = window_h_default;
        }

        glfwSetWindowSize(window, window_w, (showing)?current_window_h:0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(window_w, current_window_h));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        if (showing == true){
            ImGui::Begin("Search Bar", &open, window_flags);

            float text_filed_width = ((float)window_w) * 0.9825;
            float text_filed_height = ((float)window_h_default) * 0.139;

            ImGui::PushItemWidth(text_filed_width);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, text_filed_height));

            ImGui::InputText(" ", buff, sizeof(buff), input_text_flags, search_bar_callback, nullptr);

            ImGui::PopItemWidth();
            ImGui::PopStyleVar();

            if (!searchResults.empty()){
                ImGui::Spacing();
                ImGui::Separator();

                vector<ProgramEntry> resultsCopy = searchResults;
        
                for (size_t i = 0; i < resultsCopy.size(); i++){
                    if (ImGui::Button(resultsCopy[i].name.c_str(), ImVec2(text_filed_width, 0))){
                        launchProgram(resultsCopy[i].path);
                        showing = false;
                        buff[0] = '\0';
                        searchResults.clear();
                        break;
                    }

                    if (ImGui::IsItemHovered()){
                        ImGui::SetTooltip("%s", resultsCopy[i].path.c_str());
                    }
                }
            }
        
            ImGui::End();
        }
        
        ImGui::Render();
        int dis_w, dis_h;
        glfwGetFramebufferSize(window, &dis_w, &dis_h);
        glViewport(0, 0, dis_w, dis_h);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        this_thread::sleep_for(chrono::milliseconds(66)); // ~15fps
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(0);
}