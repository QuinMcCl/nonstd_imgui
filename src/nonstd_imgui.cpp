#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <nonstd.h>

#include "nonstd_imgui.h"

int imgui_init(nonstd_imgui_t *gui, GLFWwindow *window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    {
        gui->options.file_options.options_enabled = 0;
        gui->options.file_options.should_close = 0;
        gui->options.file_options.requesting_close = 0;
        gui->options.file_options.unsaved_changes = 1;
        gui->options.file_options.save_changes = 0;
        gui->options.tool_options.show_tool_about = 0;
        gui->options.tool_options.show_tool_debug_log = 0;
        gui->options.tool_options.show_tool_id_stack_tool = 0;
        gui->options.tool_options.show_tool_metrics = 0;
        gui->options.tool_options.show_tool_style_editor = 0;
    }

    return 0;
}

int imgui_cleanup()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

int imgui_start_frame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    return 0;
}

int imgui_end_frame()
{
    ImGuiIO &io = ImGui::GetIO();
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    return 0;
}

void ShowMainMenu(imgui_main_menu_options_t *menu_options)
{
    if (menu_options->file_options.requesting_close)
    {
        ShowClosePopUp(&(menu_options->file_options));
    }
    if (menu_options->tool_options.show_tool_metrics)
        ImGui::ShowMetricsWindow((bool *)&(menu_options->tool_options.show_tool_metrics));
    if (menu_options->tool_options.show_tool_debug_log)
        ImGui::ShowDebugLogWindow((bool *)&(menu_options->tool_options.show_tool_debug_log));
    if (menu_options->tool_options.show_tool_id_stack_tool)
        ImGui::ShowIDStackToolWindow((bool *)&(menu_options->tool_options.show_tool_id_stack_tool));
    if (menu_options->tool_options.show_tool_style_editor)
    {
        ImGui::Begin("Dear ImGui Style Editor", (bool *)&(menu_options->tool_options.show_tool_style_editor));
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (menu_options->tool_options.show_tool_about)
        ImGui::ShowAboutWindow((bool *)&(menu_options->tool_options.show_tool_about));

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowFileMenu(&(menu_options->file_options));
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
            } // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
            }
            ImGui::EndMenu();
        }

        ShowTools(&(menu_options->tool_options));
        ImGui::EndMainMenuBar();
    }
}

void ShowFileMenu(imgui_file_options_t *file_options)
{
    if (ImGui::MenuItem("New"))
    {
    }
    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
    }
    if (ImGui::MenuItem("Save As.."))
    {
    }

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        ImGui::MenuItem("Enabled", "", (bool *)&(file_options->options_enabled));
        ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Border);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true))
    {
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4"))
    {
        file_options->requesting_close = true;
    }
}

void ShowTools(imgui_tool_options_t *tool_options)
{
    if (ImGui::BeginMenu("Tools"))
    {
        const bool has_debug_tools = true;
        ImGui::MenuItem("Metrics/Debugger", NULL, (bool *)&(tool_options->show_tool_metrics), has_debug_tools);
        ImGui::MenuItem("Debug Log", NULL, (bool *)&(tool_options->show_tool_debug_log), has_debug_tools);
        ImGui::MenuItem("ID Stack Tool", NULL, (bool *)&(tool_options->show_tool_id_stack_tool), has_debug_tools);
        ImGui::MenuItem("Style Editor", NULL, (bool *)&(tool_options->show_tool_style_editor));
        ImGui::MenuItem("About Dear ImGui", NULL, (bool *)&(tool_options->show_tool_about));
        ImGui::EndMenu();
    }
}

void ShowClosePopUp(imgui_file_options_t *file_options)
{
    if (file_options->unsaved_changes)
    {
        ImGui::OpenPopup("Save before exiting?");
        if (ImGui::BeginPopupModal("Save before exiting?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("So you want to save before exiting?\nThis operation cannot be undone!");
            ImGui::Separator();

            // ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Save and Exit", ImVec2(120, 0)))
            {
                // TODO: Save
                file_options->save_changes = true;
                file_options->should_close = true;
                file_options->requesting_close = false;

                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Exit Without Saving", ImVec2(120, 0)))
            {
                file_options->save_changes = false;
                file_options->should_close = true;
                file_options->requesting_close = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                file_options->save_changes = false;
                file_options->should_close = false;
                file_options->requesting_close = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    else
    {
        file_options->save_changes = false;
        file_options->should_close = true;
        file_options->requesting_close = false;
    }
}

int imgui_draw(nonstd_imgui_t *gui)
{
    imgui_start_frame();

    // imgui draw calls
    {
        ShowMainMenu(&gui->options);
    }

    imgui_end_frame();
    return 0;
}
