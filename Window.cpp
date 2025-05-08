#include "Window.h"
#include "Constants.h"
#include <iostream>

Window::Window(const char* title) {

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(WIDTH, HEIGHT, title, nullptr, nullptr);

    uint32_t glfwExtensionCount;
    uint32_t  deviceCount;
    
    const char** glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtension, glfwExtension + glfwExtensionCount);
 
    VkInstanceCreateInfo info = {};
    VkApplicationInfo appInfo = {};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = title;
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,1);
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_4;


    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = (uint32_t) extensions.size();
    info.ppEnabledExtensionNames = extensions.data();
    

    vkCreateInstance(&info, nullptr, &instance);
    // получение кол-ва девайсов
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    // получение текущей видеокарты
    VkPhysicalDevice physicalDevice = devices.data()[0];
    
    vkGetPhysicalDeviceProperties(physicalDevice, &GPU_info); // информация о видеокарте

    // кол-во потоков и инфа о них
    uint32_t graphicsQueueFamilyIndex = 0;
    uint32_t presentQueueFamilyIndex = 0;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,nullptr);

    std::vector<VkQueueFamilyProperties>families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, families.data());

    // определение функции потоков
    for (size_t i = 0; i < families.size(); i++)
    {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
            break;
        }
        
    }

    // поток графики (урок 6)

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float queuePriority = 1.0f;  // приоритет выполнения задач, отрисовка или отображение, 1 - нейтрально
    VkPhysicalDeviceFeatures deviceFeatures = {};
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueCount = 1;
    queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    queueInfo.pQueuePriorities = &queuePriority;

    
    queueCreateInfos.push_back(queueInfo);

    // логический девайс
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledExtensionCount = (uint32_t)0;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    VkDevice logicalDevice;
    vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);


}



Window::~Window() {


	glfwDestroyWindow(window);

	glfwTerminate();
}

GLFWwindow* Window::GetCurrentWindow() {

    return window;
    
}