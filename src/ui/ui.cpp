#include "ui.h"

#include "download/downloadControllerEvents.h"
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>


void UI::draw() {
    //draw enabled pop up window
    this->draw_pop_up_window();

    //draw main window
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    //set the next window size to the total viewport area
    const ImGuiViewport* viewport = ImGui::GetMainViewport(); //get the full viewport
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    //draw initial root window
    if (ImGui::Begin("Root", nullptr, flags)) {

        //draw menu bar
        this->draw_menu_bar();
        ImGui::Separator();

        //draw button header
        this->draw_button_header();
        ImGui::Separator();

        ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        ImGui::Spacing();

        const ImVec2 available_region = ImGui::GetContentRegionAvail();

        //draw "left" panel
        ImGui::BeginChild("FilesystemNavigatorPanel",ImVec2(280.0f, 0.0f), ImGuiChildFlags_Borders);
        this->draw_filesystem_nav_panel();
        ImGui::EndChild();

        //draw button as slider
        ImGui::SameLine();
        ImGui::Button("##VerticalSplitter", ImVec2(6.0f, available_region.y));
        ImGui::SameLine();

        //draw "right" panel
        ImGui::BeginChild("DownloadMetadataPanel", ImVec2(0, 0), ImGuiChildFlags_Borders);
        this->draw_download_list_panel();
        ImGui::EndChild();

        ImGui::End(); //end of root
    }
}

void UI::draw_pop_up_window() {
    //draw "add download"
    if ( m_shared_ui_window_data.m_show_add_download_window ) { this->draw_add_download_window(); }
}

void UI::draw_menu_bar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tasks")){
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("File")){
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Downloads")){
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")){
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")){
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void UI::draw_button_header() {
    // create a popup window to "add a download" to the queue
    if (ImGui::Button("Add URL")) {
        m_shared_ui_window_data.m_show_add_download_window = !m_shared_ui_window_data.m_show_add_download_window;
    }; ImGui::SameLine();

    ImGui::Button("Resume"); ImGui::SameLine();
    ImGui::Button("Stop"); ImGui::SameLine();
    ImGui::Button("Delete"); ImGui::SameLine();
    ImGui::Button("Delete Completed"); ImGui::SameLine();
    ImGui::Button("Scheduler"); ImGui::SameLine();
    ImGui::Button("Options"); ImGui::SameLine();
    ImGui::Button("Start Queue"); ImGui::SameLine();
    ImGui::Button("Stop Queue"); ImGui::SameLine();
    ImGui::Button("Grabber");
}

void UI::draw_filesystem_nav_panel() {

    ImGui::Text("Filesystem");
    ImGui::Separator();

    // Placeholder filesystem tree
    if (ImGui::TreeNode("Downloads")) {
        ImGui::Selectable("ubuntu.iso");
        ImGui::Selectable("godot.zip");
        ImGui::Selectable("video.mp4");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Documents")) {
        ImGui::Selectable("notes.txt");
        ImGui::Selectable("resume.pdf");
        ImGui::TreePop();
    }

}

void UI::draw_download_list_panel() {
    ImGui::Text("Download Metadata");
    ImGui::Separator();

    //todo: poll get all downloadids and display snapshot metadata

    ImGui::Text("URL: %s", "https://example.com/file.zip");
    ImGui::Text("File: %s", "file.zip");
    ImGui::Text("Status: %s", "Downloading");
    ImGui::Text("Speed: %s", "12.4 MB/s");
    ImGui::Text("ETA: %s", "00:02:31");

    ImGui::ProgressBar(0.45f, ImVec2(-1.0f, 0.0f), "45%");
}

void UI::draw_add_download_window() {
    //setup popup window properties
    const auto total_size = ImGui::GetMainViewport()->WorkSize;
    constexpr float window_size_x = 640;
    constexpr float window_size_y = 384;
    ImGui::SetNextWindowSize( ImVec2(window_size_x, window_size_y) );
    ImGui::SetNextWindowPos( ImVec2(total_size.x/2 - window_size_x/2,total_size.y/2 - window_size_y/2) );
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoScrollbar;
    if ( !ImGui::Begin("Scan address to download", &m_shared_ui_window_data.m_show_add_download_window, flags) ){
        ImGui::End();
        return;
    }
    //draw "Address:" label
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Address:"); ImGui::SameLine();
    //draw input text line
    ImGui::InputText("##Address", &m_shared_ui_window_data.m_source, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
    //draw "Output:" label
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Output:"); ImGui::SameLine();
    ImGui::InputText("##Output", &m_shared_ui_window_data.m_output, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);  ImGui::SameLine();
    if ( ImGui::Button("Start", ImVec2(-FLT_MIN,0)) ) {
        //submit a DownloadSubmit event
        if (!m_shared_ui_window_data.m_source.empty()) {
            m_event_manager->publish(std::make_shared<DownloadSubmitEvent>(
                std::move(m_shared_ui_window_data.m_source),
                "/Users/abi/Downloads/" + m_shared_ui_window_data.m_output
            ));
            m_shared_ui_window_data.m_source.clear();
        }
    }

    ImGui::End();
}
