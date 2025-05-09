#include "Window.h"
#include "Constants.h"
#include <iostream>
#include <set>
#include "Shader.h"
Shader* shader = new Shader();

int shaderCount = 2;
std::vector<char> shaderCode;
std::vector <VkShaderModule> shadersModules(shaderCount);
void Window::LoadShaders() {

    for (size_t i = 0; i < shaderCount; i++)
    {
        VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
        if (i == 1) {
            shaderCode = shader->ReadShader("shaders/vert.spv");
        }
        else {
            shaderCode = shader->ReadShader("shaders/frag.spv");

        }
        ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ShaderModuleCreateInfo.codeSize = shaderCode.size();
        ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
        VkShaderModule ShaderModule;

        vkCreateShaderModule(logicalDevice, &ShaderModuleCreateInfo, nullptr, &ShaderModule);
        shadersModules.push_back(ShaderModule);
    }

    vertModule = shadersModules[0];
    fragModule = shadersModules[1];

}
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
    // получение видеокарт
    vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
   
    VkPhysicalDevice physicalDevice = devices.data()[0]; // òåêóùàÿ âèäåîêàðòà
    
    vkGetPhysicalDeviceProperties(physicalDevice, &GPU_info); // получение информации о видеокарте

    // id Потоков видеокарты
    uint32_t graphicsQueueFamilyIndex = 0;
    uint32_t presentQueueFamilyIndex = 0;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,nullptr);

    std::vector<VkQueueFamilyProperties>families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, families.data());
    VkBool32 presentSupport;
    // определение принадлежности потоков
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



    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> qniqueQueue = {
        graphicsQueueFamilyIndex,
        presentQueueFamilyIndex
    
    };

   


    float queuePriority = 1.0f;  // приоритет выполнения операций, 1 - нейтральный
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

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME

    };

    // Логический девайс
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledExtensionCount = (uint32_t) deviceExtensions.size();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();



    
    vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice); // создание девайса


    uint32_t formatsCount; // форматы видеокарты

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatsCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, formats.data());


    VkSurfaceFormatKHR surfaceFormat = formats.data()[0];

    for (VkSurfaceFormatKHR avalibleFormat : formats)
    {
        if (avalibleFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && avalibleFormat.format == VK_FORMAT_B8G8R8_SRGB) {

            surfaceFormat = avalibleFormat;
            break;
        }
    }
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    VkExtent2D extent = capabilities.currentExtent;
    uint32_t imageCount = capabilities.minImageCount + 1;



    VkSwapchainKHR swapChain; // буфер
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageArrayLayers = 1; // кол-во слоёв
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.preTransform = capabilities.currentTransform;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

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
    swapChainCreateInfo.presentMode = presentMode;
    vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain);

    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapChainImageCount, nullptr);
    std::vector<VkImage> swapChainImages(swapChainImageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapChainImageCount, swapChainImages.data());
    std::vector<VkImageView> swapChainImageView(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapChainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // 2d/3d графика
        imageViewCreateInfo.format = surfaceFormat.format;
        // определение цветов
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // красный - красный, определение цветов
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY; // 
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // тип изображения (цвет/глубина)

        // mip maping
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &swapChainImageView[i]);


        //render Pass
        VkAttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription.format = surfaceFormat.format;
        colorAttachmentDescription.samples = sampleBits; // сглаживание
        colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentReference  colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription{};

        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;



        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;


        vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);

    }
    // подключение шейдеров , создание графического конвеера
    LoadShaders();
    VkPipelineShaderStageCreateInfo vertPipelineShaderStageCreateInfo{};
    vertPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertPipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertPipelineShaderStageCreateInfo.module = vertModule;
    vertPipelineShaderStageCreateInfo.pName = "main";


    VkPipelineShaderStageCreateInfo fragPipelineShaderStageCreateInfo{};
    fragPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragPipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragPipelineShaderStageCreateInfo.module = fragModule;
    fragPipelineShaderStageCreateInfo.pName = "main";


    VkPipelineShaderStageCreateInfo shaderStages[] = { vertPipelineShaderStageCreateInfo , fragPipelineShaderStageCreateInfo };

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo{};

    PipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    PipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    PipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo{};
    PipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    PipelineViewportStateCreateInfo.viewportCount = 1;
    PipelineViewportStateCreateInfo.scissorCount = 1;
    // Растеризация
    VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo{};
    PipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    PipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    PipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    PipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    PipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    PipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
    PipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    PipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // сглаживание
    VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo{};
    PipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    PipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE; // сглавживание
    PipelineMultisampleStateCreateInfo.rasterizationSamples = sampleBits;
    
    // смешивание цветов
    VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState{};
    PipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
    PipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    

    VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo{};
    PipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    PipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    PipelineColorBlendStateCreateInfo.attachmentCount = 1;
    PipelineColorBlendStateCreateInfo.pAttachments = &PipelineColorBlendAttachmentState;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR

    };
    VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo{};
    PipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    PipelineDynamicStateCreateInfo.dynamicStateCount = (uint32_t) dynamicStates.size();
    PipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();


    VkPipelineLayoutCreateInfo  PipelineLayoutCreateInfo{};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutCreateInfo.setLayoutCount = 0;
    PipelineLayoutCreateInfo.pushConstantRangeCount = 0;


    
    vkCreatePipelineLayout(logicalDevice, &PipelineLayoutCreateInfo, nullptr, &pipelineLayout);

    VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo{};
    GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    GraphicsPipelineCreateInfo.stageCount = 2;
    GraphicsPipelineCreateInfo.pStages = shaderStages;
    GraphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
    GraphicsPipelineCreateInfo.pInputAssemblyState = &PipelineInputAssemblyStateCreateInfo;
    GraphicsPipelineCreateInfo.pViewportState = &PipelineViewportStateCreateInfo;
    GraphicsPipelineCreateInfo.pRasterizationState = &PipelineRasterizationStateCreateInfo;
    GraphicsPipelineCreateInfo.pMultisampleState = &PipelineMultisampleStateCreateInfo;
    GraphicsPipelineCreateInfo.pColorBlendState = &PipelineColorBlendStateCreateInfo;
    GraphicsPipelineCreateInfo.pDynamicState = &PipelineDynamicStateCreateInfo;
    GraphicsPipelineCreateInfo.layout = pipelineLayout;
    GraphicsPipelineCreateInfo.renderPass = renderPass;
    GraphicsPipelineCreateInfo.subpass = 0;
    GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    
    vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo,nullptr,&graphicsPipeline);


    vkDestroyShaderModule(logicalDevice, vertModule, nullptr);
    vkDestroyShaderModule(logicalDevice, fragModule, nullptr);

    std::vector<VkFramebuffer> swapChainFrameBuffers(swapChainImageView.size());

    for (size_t i = 0; i < swapChainImageView.size(); i++)
    {
        VkImageView attachements[] = {swapChainImageView[i]};
        VkFramebufferCreateInfo FramebufferCreateInfo{};
        FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferCreateInfo.renderPass = renderPass;
        FramebufferCreateInfo.attachmentCount = 1;
        FramebufferCreateInfo.pAttachments = attachements;
        FramebufferCreateInfo.width = extent.width;
        FramebufferCreateInfo.height = extent.height;
        FramebufferCreateInfo.layers = 1; // слои
       



        vkCreateFramebuffer(logicalDevice, &FramebufferCreateInfo, nullptr, &swapChainFrameBuffers[i]);

    }

}




Window::~Window() {


	glfwDestroyWindow(window);

	glfwTerminate();
}




GLFWwindow* Window::GetCurrentWindow() {

    return window;
    
}
