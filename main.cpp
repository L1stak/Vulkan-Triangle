#include "Window.h"
#include "Shader.h"
#include <vector>
//#include <vulkan/vulkan.h>
Window* win = new Window("vulkan test");
int main() {
   
    GLFWwindow* currentWindow = win->GetCurrentWindow();
    
    uint32_t frame = 0;
    uint32_t imageIndex;
    VkDevice device = win->GetLogicalDevice();
    VkSwapchainKHR swapChain = win->GetSwapChain();
    std::vector<VkSemaphore> ImageAvalibleSemaphoress = win->GetImageAvalibleSemaphores();
    std::vector<VkCommandBuffer> commandBuffer = win->GetCommandBuffer();
    while (!glfwWindowShouldClose(currentWindow)) {
        glfwPollEvents();


        VkAcquireNextImageInfoKHR acquireInfo{};
        acquireInfo.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
        acquireInfo.swapchain = swapChain;
        acquireInfo.timeout = UINT64_MAX;
        acquireInfo.semaphore = ImageAvalibleSemaphoress[frame];
        acquireInfo.fence = VK_NULL_HANDLE;

        vkAcquireNextImage2KHR(device, &acquireInfo,&imageIndex);
      
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore waitSemaphores[] = { ImageAvalibleSemaphoress[frame]};
        VkSubmitInfo SubmitInfo{};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &commandBuffer[frame];
        SubmitInfo.pWaitDstStageMask = waitStages;
        SubmitInfo.waitSemaphoreCount = 1;
        SubmitInfo.pWaitSemaphores = waitSemaphores;

        vkQueueSubmit(win->GetGraphicsQueue(),1,&SubmitInfo,win->GetinFlightFences()[frame]);
        frame = (frame + 1) % 2; //MAX_FRAMES_IN_FLIGHT
    }



    return 0;
}