#pragma once

#include <unordered_map> 
#include <memory> 

#include "../platform/Window.hpp"

#include "Camera.hpp"
#include "Vulkan.hpp"
#include "ImageManager.hpp"
#include "Image.hpp"

#include "api_abstration/Device.hpp"
#include "api_abstration/RenderWindow.hpp"

namespace daxa {

	struct PersistentRessources {
		std::unordered_map<std::string_view, gpu::GraphicsPipelineHandle> pipelines;
		std::unordered_map<std::string_view, gpu::ImageHandle> images;
		std::unordered_map<std::string_view, gpu::BufferHandle> buffers;
	};

	struct PerFrameRessources {
		std::unordered_map<std::string_view, gpu::ImageHandle> images;
		std::unordered_map<std::string_view, gpu::BufferHandle> buffers;
		std::unordered_map<std::string_view, vk::UniqueSemaphore> semaphores;
	};

	class Renderer {
	public:
		Renderer(std::shared_ptr<Window> win);

		void init() {
			for (int i = 0; i < 3; i++) {
				frameResc[i].semaphores["render"] = device->getVkDevice().createSemaphoreUnique({});
			}
		}

		void draw() {
			auto& frame = frameResc.front();

			auto swapchainImage = renderWindow.getNextImage();

			auto cmdList = device->getEmptyCommandList();

			cmdList.begin();



			cmdList.end();

			device->submit(std::move(cmdList), {
				.waitOnSemaphores = {swapchainImage.getVkSemaphore()},
				.signalSemaphores = {*frame.semaphores["render"]}
			});

			device->present(swapchainImage, { *frame.semaphores["render"] });

			nextFrameContext();
		}

		void deinit() {}

		FPSCamera camera;
		std::shared_ptr<gpu::Device> device;
		PersistentRessources persResc;
		std::deque<PerFrameRessources> frameResc;
	private:
		std::shared_ptr<Window> window{ nullptr };
		gpu::RenderWindow renderWindow;
		void nextFrameContext();
	};
}
