#include "Window.h"
#include "Constants.h"
#include <iostream>
#include <set>
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

    
    glfwCreateWindowSurface(instance, window,nullptr,&surface);
    // получение кол-ва девайсов
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    // получение текущей видеокарты
    VkPhysicalDevice physicalDevice = devices.data()[0]; // текущая видеокарта
    
    vkGetPhysicalDeviceProperties(physicalDevice, &GPU_info); // информация о видеокарте

    // кол-во потоков и инфа о них
    uint32_t graphicsQueueFamilyIndex = 0;
    uint32_t presentQueueFamilyIndex = 0;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,nullptr);

    std::vector<VkQueueFamilyProperties>families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, families.data());
    VkBool32 presentSupport;
    // определение функции потоков
    for (size_t i = 0; i < families.size(); i++)
    {

        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport) {

            presentQueueFamilyIndex = i;
        }


        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
            break;
        }
      
        
        
    }

    // поток графики (урок 6)

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> qniqueQueue = {
        graphicsQueueFamilyIndex,
        presentQueueFamilyIndex
    
    };

   


    float queuePriority = 1.0f;  // приоритет выполнения задач, отрисовка или отображение, 1 - нейтрально
    for (const uint32_t queueFamily : qniqueQueue)
    {

        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueCount = 1;
        queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        queueInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueInfo);
    }


   
    VkPhysicalDeviceFeatures deviceFeatures = {};
    VkDeviceQueueCreateInfo queueInfo = {};
    

    
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



    VkSwapchainKHR swapChain; // буфер
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageArrayLayers = 1; // кол-во возможных слоёв для отрисовки
    


    uint32_t queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
         uint32_t queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

         swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
         swapChainCreateInfo.queueFamilyIndexCount = (uint32_t)size_t(queueFamilyIndices);
         swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;

    }
    else {

        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    for(VkPresentModeKHR avalibleMode : presentModes)
    {
        if (avalibleMode == VK_PRESENT_MODE_MAILBOX_KHR) {

            presentMode = avalibleMode;
            break;
        }
    }

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

    swapChainCreateInfo.presentMode = presentMode;
    vkCreateSwapchainKHR(logicalDevice,&swapChainCreateInfo,nullptr,&swapChain);



}



Window::~Window() {


	glfwDestroyWindow(window);

	glfwTerminate();
}

GLFWwindow* Window::GetCurrentWindow() {

    return window;
    
}