#include <cassert>
#include <memory>

#include "engine/gfx/commands/factory.hpp"

namespace {

using vme::engine::gfx::commands::BarrierCommand;
using vme::engine::gfx::commands::CommandBufferDesc;
using vme::engine::gfx::commands::CommandErrorCode;
using vme::engine::gfx::commands::CopyBufferCommand;
using vme::engine::gfx::commands::DispatchCommand;
using vme::engine::gfx::commands::DrawCommand;
using vme::engine::gfx::commands::PipelineStage;

void test_records_commands_and_finishes_immutably() {
    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder);

    assert(encoder->copy(CopyBufferCommand {.size_bytes = 256}).ok());
    assert(encoder->dispatch(DispatchCommand {.group_count_x = 2, .group_count_y = 1, .group_count_z = 1}).ok());
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());
    assert(encoder->barrier(BarrierCommand {.src_stage = PipelineStage::kTransfer,
                                            .dst_stage = PipelineStage::kFragment})
               .ok());

    const auto finish_result = encoder->finish(CommandBufferDesc {.label = "frame-main"});
    assert(finish_result.ok());
    assert(finish_result.command_buffer->command_count() == 4);
    assert(finish_result.command_buffer->desc().label == "frame-main");
}

void test_rejects_invalid_arguments() {
    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();

    const auto copy_error = encoder->copy(CopyBufferCommand {.size_bytes = 0});
    assert(copy_error.code == CommandErrorCode::InvalidArgument);

    const auto dispatch_error =
        encoder->dispatch(DispatchCommand {.group_count_x = 0, .group_count_y = 1, .group_count_z = 1});
    assert(dispatch_error.code == CommandErrorCode::InvalidArgument);

    const auto draw_error = encoder->draw(DrawCommand {.vertex_count = 0});
    assert(draw_error.code == CommandErrorCode::InvalidArgument);

    const auto barrier_error = encoder->barrier(BarrierCommand {.src_stage = PipelineStage::kCompute,
                                                                .dst_stage = PipelineStage::kCompute});
    assert(barrier_error.code == CommandErrorCode::InvalidArgument);
}

void test_rejects_recording_after_finish() {
    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();

    const auto finish_result = encoder->finish(CommandBufferDesc {.label = "once"});
    assert(finish_result.ok());

    const auto draw_after_finish = encoder->draw(DrawCommand {.vertex_count = 3});
    assert(draw_after_finish.code == CommandErrorCode::InvalidState);

    const auto second_finish = encoder->finish(CommandBufferDesc {.label = "twice"});
    assert(!second_finish.ok());
    assert(second_finish.error.code == CommandErrorCode::InvalidState);
}

}  // namespace

int main() {
    test_records_commands_and_finishes_immutably();
    test_rejects_invalid_arguments();
    test_rejects_recording_after_finish();
    return 0;
}
