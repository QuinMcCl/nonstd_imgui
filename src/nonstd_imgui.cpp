#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <nonstd.h>
#include <nonstd_glfw_opengl.h>
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
        gui->paused = 1;
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

void ShowMat4(const char *name, mat4 matrix)
{
    ImGui::Text(name);
    ImGui::Text("[%.3f,%.3f,%.3f,%.3f]", matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0]);
    ImGui::Text("[%.3f,%.3f,%.3f,%.3f]", matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1]);
    ImGui::Text("[%.3f,%.3f,%.3f,%.3f]", matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]);
    ImGui::Text("[%.3f,%.3f,%.3f,%.3f]", matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);
}

void ShowCamera(camera_t *camera)
{
    static ImGuiSliderFlags flags = ImGuiSliderFlags_None;

    if (ImGui::CollapsingHeader("Camera"))
    {

        ImGui::DragFloat("Sensitivity", &(camera->mMouseSensitivity), 0.005f, -FLT_MAX, FLT_MAX, "%f", flags);
        ImGui::Separator();
        ImGui::DragFloat("Pos X", &(camera->mPosition[0]), 0.005f, -FLT_MAX, FLT_MAX, "%.3f", flags);
        ImGui::DragFloat("Pos Y", &(camera->mPosition[1]), 0.005f, -FLT_MAX, FLT_MAX, "%.3f", flags);
        ImGui::DragFloat("Pos Z", &(camera->mPosition[2]), 0.005f, -FLT_MAX, FLT_MAX, "%.3f", flags);

        ImGui::DragFloat("Pitch X", &(camera->mPitch), 0.05f, -180.0f, 180.0f, "%.3f", flags);
        ImGui::DragFloat("Roll X", &(camera->mRoll), 0.05f, -180.0f, 180.0f, "%.3f", flags);
        ImGui::DragFloat("Yaw X", &(camera->mYaw), 0.05f, -180.0f, 180.0f, "%.3f", flags);

        ImGui::DragFloat("FOV", &(camera->mFOV), 0.05f, 0.0f, 180.0f, "%.3f", flags);
        ImGui::Separator();

        ImGui::Text("Front:  [%.3f,%.3f,%.3f]", camera->front[0], camera->front[1], camera->front[2]);
        ImGui::Text("Woldup: [%.3f,%.3f,%.3f]", camera->mWorldUp[0], camera->mWorldUp[1], camera->mWorldUp[2]);
        ImGui::Text("up:     [%.3f,%.3f,%.3f]", camera->up[0], camera->up[1], camera->up[2]);
        ImGui::Separator();
        ShowMat4("View:", camera->mView);
        ImGui::Separator();
        ShowMat4("projection:", camera->mProjection);
    }
}

void ShowCameraToolWindow(bool *p_open, unsigned int numCameras, camera_t *camera)
{
    if (!ImGui::Begin("Camera Tool Window", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Camera Tool Window");
    ImGui::Separator();
    for (unsigned int index = 0; index < numCameras; index++)
    {
        ShowCamera(&(camera[index]));
    }

    ImGui::End();
}

void ShowMaterial(material_t *material)
{
    for (unsigned int type = 0; type < AI_TEXTURE_TYPE_MAX + 1; type++)
    {

        if (material->mTextureCount[type] > 0 && ImGui::TreeNode((void *)(intptr_t)type, "Texture %d", type))
        {

            // ImGui::Text("mTextureCount[%d]: %d", type, material->mTextureCount[type]);
            for (unsigned int index = 0; index < material->mTextureCount[type]; index++)
            {
                ImGui::Text("Material[%d][%d]:%lu", type, index, material->mTextureIndex[type][index]);
            }

            ImGui::TreePop();
        }
    }

    // ImGui::Text("mNumMeshes: %d", material->mNumMeshes);
    for (unsigned int index = 0; index < material->mNumMeshes; index++)
    {
        ImGui::Text("Material Mesh:%d", material->mMeshes[index]);
    }
}

void ShowMesh(mesh_t *mesh)
{
    // ImGui::Text("NumInstances: %d", mesh->mNumInstances);
    for (unsigned int index = 0; index < mesh->mNumInstances; index++)
    {
        if (ImGui::TreeNode((void *)(intptr_t)index, "Instance %d", index))
        {
            ShowMat4("Transform:", mesh->mTransformation[index]);
            ImGui::TreePop();
        }
    }
}

void ShowShader(const shader_t *shader)
{
}

void ShowNode(model_node_t *node, const char *parentstr)
{
    char strbuffer[1024];
    ShowMat4("Transform:", node->mTransformation);

    ImGui::Text("mNumMeshes: %d", node->mNumMeshes);
    for (unsigned int index = 0; index < node->mNumMeshes; index++)
    {
        if (ImGui::TreeNode((void *)(intptr_t)index, "Mesh %d", index))
        {
            ImGui::Text("mMeshInstance: %d", node->mMeshData[index].mMeshInstance);
            ImGui::Text("mMeshIndex: %d", node->mMeshData[index].mMeshIndex);
            ImGui::TreePop();
        }
    }

    // ImGui::Text("mNumChildren: %d", node->mNumChildren);
    for (unsigned int index = 0; index < node->mNumChildren; index++)
    {
        snprintf(strbuffer, 1024, "%s %d", parentstr, index);
        if (ImGui::TreeNode((void *)(intptr_t)index, strbuffer))
        {
            ShowNode(&(node->mChildren[index]), strbuffer);
            ImGui::TreePop();
        }
    }
}

void ShowModel(model_t *model)
{

    if (model->mNumMaterials > 0 && ImGui::TreeNode("Materials"))
    { // ImGui::Text("mNumMaterials: %d", model->mNumMaterials);
        for (unsigned int materialIndex = 0; materialIndex < model->mNumMaterials; materialIndex++)
        {
            if (ImGui::TreeNode((void *)(intptr_t)materialIndex, "Material %d", materialIndex))
            {
                ShowMaterial(&(model->mMaterialList[materialIndex]));
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (model->mNumMeshes > 0 && ImGui::TreeNode("Meshes"))
    { // ImGui::Text("mNumMeshes: %d", model->mNumMeshes);
        for (unsigned int meshIndex = 0; meshIndex < model->mNumMeshes; meshIndex++)
        {
            if (ImGui::TreeNode((void *)(intptr_t)meshIndex, "Mesh %d", meshIndex))
            {
                ShowMesh(&(model->mMeshList[meshIndex]));
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    // ShowShader(model->mShader);

    if (ImGui::TreeNode("RootNode"))
    {
        ShowNode(&(model->mRootNode), "Node");
        ImGui::TreePop();
    }
}

void ShowModelToolWindow(bool *p_open, unsigned int num_models, model_t *model)
{
    if (!ImGui::Begin("Model Tool Window", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Model Tool Window");
    ImGui::Separator();
    if (ImGui::TreeNode("Models"))
    {
        // ImGui::Text("num_models: %d", num_models);
        for (unsigned int index = 0; index < num_models; index++)
        {
            if (ImGui::TreeNode((void *)(intptr_t)index, "Model %d", index))
            {
                ShowModel(&(model[index]));
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void ShowMainMenu(imgui_main_menu_options_t *menu_options)
{
    if (menu_options->file_options.requesting_close)
        ShowClosePopUp(&(menu_options->file_options));
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

        ImGui::Separator();

        ImGui::MenuItem("Model_tool", NULL, (bool *)&(tool_options->show_model_tool), has_debug_tools);
        ImGui::MenuItem("Camera_tool", NULL, (bool *)&(tool_options->show_camera_tool), has_debug_tools);

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

int imgui_draw(nonstd_imgui_t *gui, unsigned int numCameras, camera_t *cameraList, unsigned int numModels, model_t *modelList)
{
    imgui_start_frame();

    // imgui draw calls
    if(gui->paused)
    {
        ShowMainMenu(&gui->options);
    }

    if (gui->options.tool_options.show_camera_tool)
        ShowCameraToolWindow((bool *)&(gui->options.tool_options.show_camera_tool), numCameras, cameraList);
    if (gui->options.tool_options.show_model_tool)
        ShowModelToolWindow((bool *)&(gui->options.tool_options.show_model_tool), numModels, modelList);

    imgui_end_frame();
    return 0;
}

int imgui_capture_key()
{
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

int imgui_capture_mouse()
{
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureMouse;
}