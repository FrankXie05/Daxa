#include "Renderer.hpp"

#include <chrono>
using namespace std::chrono;

namespace daxa {

	Renderer::Renderer(std::shared_ptr<Window> win)
		: window{ std::move(win) }
		, device{ gpu::Device::createNewDevice() }
		, renderWindow{ device->createRenderWindow(window->getWindowHandleSDL(), window->getSize()[0], window->getSize()[1], VK_PRESENT_MODE_IMMEDIATE_KHR) }
		, stagingBufferPool{ &*device, (size_t)100'000, (VkBufferUsageFlags)VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU }
	{ 
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
			this->frameResc.push_back(PerFrameRessources{
				.renderingFinishedSignal = device->createSignal(),
				.timeline = device->createTimelineSemaphore(),
			});
		}
		
		currentFrame = &frameResc.front();
		swapchainImage = renderWindow.aquireNextImage();
	}

	Renderer::~Renderer() {
		device->waitIdle();
		frameResc.clear();
	}

	void Renderer::init() {
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
			VkSemaphoreCreateInfo semaphoreCreateInfo{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = nullptr,
			}; 
			frameResc[i].renderingFinishedSignal = device->createSignal();
		}

		gpu::ShaderModuleHandle vertexShader = device->tryCreateShderModuleFromFile(
			"daxa/shaders/test.vert", 
			VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT
		).value();

		gpu::ShaderModuleHandle fragmenstShader = device->tryCreateShderModuleFromFile(
			"daxa/shaders/test.frag",
			VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT
		).value();

		VkPipelineVertexInputStateCreateInfo d;
		gpu::GraphicsPipelineBuilder pipelineBuilder;
		pipelineBuilder.addShaderStage(vertexShader);
		pipelineBuilder.addShaderStage(fragmenstShader);
		pipelineBuilder.beginVertexInputAttributeBinding(VK_VERTEX_INPUT_RATE_VERTEX);	// add vertex attributes:
		pipelineBuilder.addVertexInputAttribute(VK_FORMAT_R32G32B32_SFLOAT);			// positions
		pipelineBuilder.addVertexInputAttribute(VK_FORMAT_R32G32B32A32_SFLOAT);			// colors
		pipelineBuilder.addColorAttachment(renderWindow.getVkFormat());
		pipelines["triangle"] = device->createGraphicsPipeline(pipelineBuilder);

		constexpr size_t vertexBufferSize = sizeof(float) * 3 * 3 /* positions */ + sizeof(float) * 4 * 3 /* colors */;
		gpu::BufferCreateInfo bufferCI{
			.size = vertexBufferSize,
			.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU,
		};
		buffers["vertex"] = device->createBuffer(bufferCI);
		std::array<float, vertexBufferSize> vertecies = {
			 1.f, 1.f, 0.0f,		1.f, 0.f, 0.f, 1.f,
			-1.f, 1.f, 0.0f,		0.f, 1.f, 0.f, 1.f,
			 0.f,-1.f, 0.0f,		0.f, 0.f, 1.f, 1.f,
		};

		gpu::uploadToStagingBuffer({(u8*)vertecies.data(), vertecies.size()}, buffers["vertex"], 0);
	}

	void Renderer::nextFrameContext() {
		auto frameContext = std::move(frameResc.back());
		frameResc.pop_back();
		frameResc.push_front(std::move(frameContext));
		currentFrame = &frameResc.front();
		device->recycle();
	}
	
	void Renderer::draw(float deltaTime) {
		if (window->getSize()[0] != renderWindow.getSize().width || window->getSize()[1] != renderWindow.getSize().height) {
			device->waitIdle();
			renderWindow.resize(VkExtent2D{ .width = window->getSize()[0], .height = window->getSize()[1] });
		}
		swapchainImage = renderWindow.aquireNextImage();

		auto cmdList = device->getEmptyCommandList();

		cmdList.begin();

		cmdList.changeImageLayout(swapchainImage.getImageHandle(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		double intpart;
		totalElapsedTime += deltaTime;
		float r = std::cos(totalElapsedTime * 0.21313) * 0.3f + 0.5f;
		float g = std::cos(totalElapsedTime * 0.75454634) * 0.3f + 0.5f;
		float b = std::cos(totalElapsedTime) * 0.3f + 0.5f;

		VkClearValue clear{ .color = VkClearColorValue{.float32 = { r, g, b, 1.0f } } };

		std::array colorAttachments{
			gpu::RenderAttachmentInfo{
				.image = swapchainImage.getImageHandle(),
				.clearValue = clear,
			}
		};
		cmdList.beginRendering(gpu::BeginRenderingInfo{
			.colorAttachments = colorAttachments,
		});

		cmdList.bindPipeline(pipelines["triangle"]);

		VkViewport viewport{
			.x = 0,
			.y = 0,
			.width = (f32)swapchainImage.getImageHandle()->getExtent().width,
			.height = (f32)swapchainImage.getImageHandle()->getExtent().height,
			.minDepth = 0,
			.maxDepth = 1,
		};
		cmdList.setViewport(viewport);

		VkRect2D scissor{
			.offset = { 0, 0 },
			.extent = { (u32)viewport.width, (u32)viewport.height },
		};
		cmdList.setScissor(scissor);

		float triangleAlpha = (std::sin(totalElapsedTime) + 1.0f) * 0.5f;
		cmdList.pushConstant(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, &triangleAlpha);

		cmdList.bindVertexBuffer(0, buffers["vertex"]);

		cmdList.draw(3, 1, 0, 0);

		cmdList.endRendering(); 

		cmdList.changeImageLayout(swapchainImage.getImageHandle(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		cmdList.end();

		// "++currentFrame->finishCounter " is the value that will be set to the timeline when the execution is finished, basicly incrementing it 
		// the timeline is the counter we use to see if the frame is finished executing on the gpu later.
		std::array signalTimelines = { std::tuple{ &currentFrame->timeline, ++currentFrame->finishCounter }};

		gpu::Device::SubmitInfo submitInfo;
		submitInfo.commandLists.push_back(std::move(cmdList));
		submitInfo.signalOnCompletion = { &currentFrame->renderingFinishedSignal, 1 };
		submitInfo.signalTimelines = signalTimelines;
		device->submit(std::move(submitInfo));

		renderWindow.present(std::move(swapchainImage), currentFrame->renderingFinishedSignal);

		nextFrameContext();											// we get the next frame context
		// we wait on the gpu to finish executing the frame
		// as we have two frame contexts we are actually waiting on the previous frame to complete.
		// if you only have one frame in flight you can just wait on the frame to finish here too.
		currentFrame->timeline.wait(currentFrame->finishCounter);	
	}

	void Renderer::deinit() {

	}
}
