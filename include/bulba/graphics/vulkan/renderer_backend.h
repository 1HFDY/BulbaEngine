#ifndef BULBA_GRAPHICS_VULKAN_RENDERER_BACKEND_H
#define BULBA_GRAPHICS_VULKAN_RENDERER_BACKEND_H

#include "bulba/graphics/renderer.h"
#include "bulba/graphics/vulkan/vulkan.h"

const BLB_RenderBackendOps *BLB_VulkanRendererBackendOps(void);
int BLB_VulkanRendererBackendAttach(BLB_Renderer *renderer, VULKAN *vulkan);

#endif
