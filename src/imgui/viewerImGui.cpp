#include "src/imgui/imgui.h"
#include "src/imgui/imgui_impl_glfw.h"
#include "src/imgui/imgui_impl_opengl3.h"
#include "src/imgui/ImGuiFileBrowser.h"
#include "src/imgui/viewerImGUi.h"
#include "src/read_abaqus.h"

bool viewerImGui::pid_window(bool& open,Mesh& mesh){
    if (*open == true && mesh.get_pid_counter() > 0)
    {
        // 
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.4f, io.DisplaySize.y * 0.25f));
        ImGui::Begin("PID\'s",open,ImGuiWindowFlags_NoResize);
        ImGui::Text("aaa");
        static int selected = 0;
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < mesh.get_pid_counter()-1; i++)
        {
            Pid* temp_pid_pointer = mesh.get_pid(i);
            if (ImGui::Selectable(temp_pid_pointer->name.c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
        ImGui::SameLine();
        // right    
        ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            Pid* temp_pid_pointer = mesh.get_pid(selected);
            // TODO: write 
            // Mid* mid = mesh.get_mid(temp_pid_pointer->mid);
            // std::cout << mid->name << "\n";
            ImGui::Separator();            
            ImGui::Text("MID = ");
            ImGui::SameLine();
            // ImGui::Text((char*)mid->name.c_str());
            ImGui::EndChild();
            if (ImGui::Button("Highlight selected")) {}
        ImGui::EndGroup();       

        
        // sista end?
        ImGui::End();
    }
    
}