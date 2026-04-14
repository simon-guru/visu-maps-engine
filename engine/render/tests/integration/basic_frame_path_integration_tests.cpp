#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"
#include "engine/render/basic_frame_renderer.hpp"

namespace {

void test_runs_basic_frame_path_with_gfx_stubs() {
    auto instance = vme::engine::gfx::contracts::create_gfx_instance_stub();
    assert(instance);

    const auto device_result = instance->create_device(0);
    assert(device_result.ok());

    const auto queue_result =
        device_result.device->create_queue(vme::engine::gfx::contracts::QueueType::kGraphics);
    assert(queue_result.ok());

    const auto swapchain_result =
        device_result.device->create_swapchain(vme::engine::gfx::contracts::SwapchainDesc {.image_count = 3});
    assert(swapchain_result.ok());

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder);

    const auto result = vme::engine::render::run_basic_frame(
        *instance, *device_result.device, *queue_result.queue, *swapchain_result.swapchain, *encoder);

    assert(result.ok);
    assert(queue_result.queue->completed_submission_count() == 1);
}

}  // namespace

int main() {
    test_runs_basic_frame_path_with_gfx_stubs();
    return 0;
}
