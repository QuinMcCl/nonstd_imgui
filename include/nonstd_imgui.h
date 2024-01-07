#ifndef NONSTD_IMGUI_H
#define NONSTD_IMGUI_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct imgui_file_options_s
    {
        int options_enabled;
        int unsaved_changes;
        int save_changes;
        int requesting_close;
        int should_close;
    } imgui_file_options_t;

    typedef struct imgui_tool_options_s
    {
        int show_tool_metrics;
        int show_tool_debug_log;
        int show_tool_id_stack_tool;
        int show_tool_style_editor;
        int show_tool_about;
    } imgui_tool_options_t;

    typedef struct imgui_main_menu_options_s
    {
        imgui_file_options_t file_options;
        imgui_tool_options_t tool_options;
    } imgui_main_menu_options_t;

    typedef struct nonstd_imgui_s
    {
        imgui_main_menu_options_t options;
    } nonstd_imgui_t;

    int imgui_init(nonstd_imgui_t *gui, GLFWwindow *window);
    int imgui_cleanup();
    int imgui_start_frame();
    int imgui_end_frame();

    void ShowMainMenu(imgui_main_menu_options_t *menu_options);
    void ShowFileMenu(imgui_file_options_t *file_options);
    void ShowTools(imgui_tool_options_t *tool_options);
    void ShowClosePopUp(imgui_file_options_t *file_options);

    int imgui_draw(nonstd_imgui_t *gui);

#ifdef __cplusplus
}
#endif

#endif /* ANIMAL_H */
