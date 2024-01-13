#include "UI.h"
#include"application.h"
void UI::init(Application* application)
{
	this->application = application;
	//1> Config
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//2> Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	//3> Init Vulkan
	ImGui_ImplGlfw_InitForVulkan(application->window.pWindow, true);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = application->engine.instance;
	init_info.PhysicalDevice = application->engine.physicalDevice;
	init_info.Device = application->engine.device;
	init_info.QueueFamily = application->engine.graphicsQueueIndex;
	init_info.Queue = application->engine.graphicsQueue;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = application->engine.descriptorPool_imgui;
	init_info.Subpass = 1;
	init_info.MinImageCount = application->engine.swapChainImages.size();
	init_info.ImageCount = application->engine.swapChainImages.size();
	init_info.MSAASamples = application->engine.msaaSamples;
	//init_info.Allocator = g_Allocator;
	//init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, application->engine.renderPass);

	ImGui_ImplVulkan_CreateFontsTexture();
}

void UI::show() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		static bool first_time = true;
		if (first_time) {
			ImGui::SetNextWindowSize(ImVec2(400, 300));
			first_time = false;
		}
		ImGui::Begin(reinterpret_cast<const char*>(u8"说明"));                          // Create a window called "Hello, world!" and append into it.
		
		ImGui::Text(u8"视角操作模式：%s"u8, Camera::mode_name[application->camera.mode].c_str());
		if (ImGui::Button(u8"切换"u8)) {
			application->camera.setMode((application->camera.mode + 1) % Camera::mode_name.size());
		}
		ImGui::Text(u8"\n操作说明\n"u8);
		switch (application->camera.mode)
		{
		case 0:
			ImGui::Text(u8"环绕模式下按WASD绕模型旋转"u8);
			break;
		case 1:
			ImGui::Text(u8"自由模式下按WASD移动摄像机 按住右键移动鼠标旋转摄像机"u8);
			break;
		default:
			break;
		}
		ImGui::Text(u8"\n备注：\n多光源通过在着色器中声明常量实现\n"u8);
		//if (i >= 0) {
		//	ImGui::Text(u8"星球名称：%s"u8, objects[i].name.c_str());
		//	ImGui::Text(u8"坐标：(%6.2f,%6.2f,%6.2f) AU"u8, objects[i].location[0], objects[i].location[1], objects[i].location[2]);
		//	float r = glm::distance(objects[i].location, objects[0].location);
		//	if (r > 0.001) {
		//		ImGui::Text(u8"与太阳的距离：%6.2f AU"u8, r);
		//		ImGui::Text(u8"公转周期：%6.2f 天"u8, 365.25 * pow(r, 1.5));
		//	}
		//}
		//else ImGui::Text(reinterpret_cast<const char*>(u8"未选中星球"));               // Display some text (you can use a format strings too)
		//ImGui::Text(u8"\n操作说明：\nWASD移动摄像机\n按住右键移动鼠标调整视角\n左键点选星球\nEsc退出程序\n"u8);
		ImGui::End();
	}
}