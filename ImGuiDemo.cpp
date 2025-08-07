#include "RegionManager.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////// MyButtonGroup
#include "MyButtonGroup.h"
#include "MessageManager.h"
#include <imgui.h>
#include <random>

// 测试用例
void drawMyButtonGroups() {

    // 创建回调函数
    static auto callbackA = [] { ImGui::Text("Callback: Group2-A"); };
    static auto callbackB = [] { ImGui::Text("Callback: Group2-B"); };
    static auto callbackC = [] { ImGui::Text("Callback: Group2-C"); };

    static auto callbackX = [] { ImGui::Text("Callback: Group3-X"); };
    static auto callbackY = [] { ImGui::Text("Callback: Group3-Y"); };

    // 创建按钮组 (使用新的构造函数)
    static MyButtonGroup group1("Group1", {
            {
                "Button1", 0.3f, [] { 
                MyButtonManager::deferUIUpdate([]{ 
                    MessageManager::addMessage("Callback: Group1-Button1");
                });
            }},
            {
                "Button2", 0.5f, [] {
                MyButtonManager::deferUIUpdate([] {
                    MessageManager::addMessage("Callback: Group1-Button2 - Changing other groups");
                });
            }},
            {
                "Button3", 0.2f, [] {
                MyButtonManager::deferUIUpdate([] {
                    MessageManager::addMessage("Callback: Group1-Button3 - Changing other groups");
                });
                MyButtonManager::setHighlight("Group2", "B");
                MyButtonManager::clickButton("Group3", "X");
            }}
        });

    static MyButtonGroup group2("Group2", {
        {"A", 0.4f, []{}},
        {"B", 0.3f, []{}},
        {"C", 0.3f, []{}}
        });

    static MyButtonGroup group3("Group3", {
        {"X", 0.6f, []{}},
        {"Y", 0.4f, []{}}
        });

    static bool s_bInit{ true };
    if (s_bInit) {
        s_bInit = false;

        // 注册到管理器
        MyButtonManager::addGroup(&group1);
        MyButtonManager::addGroup(&group2);
        MyButtonManager::addGroup(&group3);

        // 设置初始高亮
        MyButtonManager::setHighlight("Group1", "Button1");
        MyButtonManager::setHighlight("Group2", "A");
        MyButtonManager::setHighlight("Group3", "Y");
    }


    // 渲染函数 (需要在每帧调用)
    static auto renderGroups = [&] {
        group1.render();
        group2.render();
        group3.render();

        // 显示调试信息
        ImGui::Separator();
        ImGui::Text("Interaction Example:");
        if (ImGui::Button("Programmatically click Group1-Button2")) {
            MyButtonManager::clickButton("Group1", "Button2");
        }
        ImGui::SameLine();
        if (ImGui::Button("Highlight Group2-C")) {
            MyButtonManager::setHighlight("Group2", "C");
        }
    };

    renderGroups();

    // 在ImGui渲染之后，交换缓冲区之前
    MyButtonManager::processDeferredUpdates();

    // 渲染所有消息
    ImGui::Separator();
    ImGui::Text("Messages:");
    MessageManager::renderMessages();
}


//////////////////////////////////////////////////////////// RegionManager
// 全局实例
RegionManager regionManager;


void drawRegionUI() {
    if (ImGui::Button("Reload Config")) {
        regionManager.ReloadConfig();
    }
    ImGui::SameLine();
    ImGui::Text("Press F5 to reload layout from file");
    regionManager.DrawUI();
}

// 主UI函数
void DrawMainUI() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(displaySize);

    ImGui::Begin("Hierarchical Region Layout", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar);

    //drawRegionUI();

    drawMyButtonGroups();
    

    ImGui::End();
}

// 初始化GLFW窗口
GLFWwindow* CreateGLFWWindow() {
    if (!glfwInit()) return nullptr;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hierarchical Region Layout", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 设置DPI缩放
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    io.FontGlobalScale = xscale;
    ImGui::GetStyle().ScaleAllSizes(xscale);

    // 初始化平台后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 设置样式
    ImGui::StyleColorsLight();

    return window;
}

// 主循环
void MainLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // 获取framebuffer尺寸（处理高DPI）
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // 设置视口
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ImGui新帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawMainUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

// 清理资源
void Cleanup(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

// 主函数
int main() {
    GLFWwindow* window = CreateGLFWWindow();
    if (!window) return 1;

    MainLoop(window);
    Cleanup(window);

    return 0;
}