#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"

namespace {

using vme::engine::gfx::commands::CommandBufferDesc;
using vme::engine::gfx::commands::DrawCommand;
using vme::engine::gfx::contracts::QueueType;
using vme::engine::gfx::contracts::SubmitInfo;
using vme::engine::gfx::contracts::SwapchainDesc;
using vme::engine::gfx::contracts::SwapchainErrorCode;

void test_instance_device_queue_swapchain_single_frame() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);
    assert(instance->adapter_count() > 0);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result = device_result.device->create_queue(QueueType::kGraphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    const auto acquire = swapchain_result.swapchain->acquire_next_image();
    assert(acquire.ok());

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());
    const auto finish = encoder->finish(CommandBufferDesc {.label = "frame-0"});
    assert(finish.ok());

    SubmitInfo submit{};
    submit.command_buffer = finish.command_buffer.get();
    assert(queue_result.queue->submit(submit).ok());
    assert(queue_result.queue->process_next_submission().ok());

    const auto present = swapchain_result.swapchain->present(acquire.image_index);
    assert(present.ok());
}

void test_swapchain_rejects_invalid_recreate_and_wrong_present_order() {
    auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
    assert(device);

    const auto swapchain_result = device->create_swapchain(SwapchainDesc {.image_count = 2});
    assert(swapchain_result.ok());

    const auto invalid_recreate = swapchain_result.swapchain->recreate(SwapchainDesc {.image_count = 1});
    assert(invalid_recreate == SwapchainErrorCode::kInvalidArgument);

    const auto wrong_present = swapchain_result.swapchain->present(0);
    assert(wrong_present.code == SwapchainErrorCode::kInvalidState);
}

}  // namespace

int main() {
    test_instance_device_queue_swapchain_single_frame();
    test_swapchain_rejects_invalid_recreate_and_wrong_present_order();
    return 0;
}
