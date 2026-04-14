#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"

namespace {

using vme::engine::gfx::commands::CommandBufferDesc;
using vme::engine::gfx::commands::CommandErrorCode;
using vme::engine::gfx::commands::DrawCommand;
using vme::engine::gfx::contracts::FenceState;
using vme::engine::gfx::contracts::QueueSubmitErrorCode;
using vme::engine::gfx::contracts::SemaphoreSignalInfo;
using vme::engine::gfx::contracts::SemaphoreState;
using vme::engine::gfx::contracts::SemaphoreWaitInfo;
using vme::engine::gfx::contracts::SubmitBatch;
using vme::engine::gfx::contracts::SubmitInfo;

void test_submit_batch_preserves_order_and_signals() {
    auto encoder_a = vme::engine::gfx::commands::create_command_encoder_stub();
    auto encoder_b = vme::engine::gfx::commands::create_command_encoder_stub();

    assert(encoder_a->draw(DrawCommand {.vertex_count = 3}).ok());
    assert(encoder_b->draw(DrawCommand {.vertex_count = 6}).ok());

    const auto finish_a = encoder_a->finish(CommandBufferDesc {.label = "A"});
    const auto finish_b = encoder_b->finish(CommandBufferDesc {.label = "B"});
    assert(finish_a.ok());
    assert(finish_b.ok());

    SemaphoreState semaphore{};
    FenceState fence_a{};
    FenceState fence_b{};

    SubmitInfo submit_a{};
    submit_a.command_buffer = finish_a.command_buffer.get();
    submit_a.signal_fence = &fence_a;
    submit_a.signal_semaphores.push_back(SemaphoreSignalInfo {.semaphore = &semaphore, .value = 2});

    SubmitInfo submit_b{};
    submit_b.command_buffer = finish_b.command_buffer.get();
    submit_b.signal_fence = &fence_b;
    submit_b.wait_semaphores.push_back(SemaphoreWaitInfo {.semaphore = &semaphore, .min_value = 2});

    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();

    const auto submit_first = queue->submit(submit_a);
    assert(submit_first.ok());

    const auto blocked_submit = queue->submit(submit_b);
    assert(blocked_submit.code == QueueSubmitErrorCode::kSyncUnresolved);

    assert(queue->pending_submission_count() == 1);
    assert(queue->process_next_submission().ok());
    assert(fence_a.signaled);
    assert(semaphore.value == 2);
    assert(queue->completed_submission_count() == 1);

    const auto submit_second = queue->submit(submit_b);
    assert(submit_second.ok());
    assert(queue->pending_submission_count() == 1);
    assert(queue->process_next_submission().ok());
    assert(fence_b.signaled);
    assert(queue->completed_submission_count() == 2);
}

void test_submit_batch_validates_all_entries_before_enqueue() {
    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());

    const auto finish = encoder->finish(CommandBufferDesc {.label = "valid"});
    assert(finish.ok());

    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();

    SubmitBatch batch{};
    batch.submissions.push_back(SubmitInfo {.command_buffer = finish.command_buffer.get()});
    batch.submissions.push_back(SubmitInfo {.command_buffer = nullptr});

    const auto batch_result = queue->submit_batch(batch);
    assert(batch_result.code == QueueSubmitErrorCode::kInvalidArgument);
    assert(queue->pending_submission_count() == 0);
}

}  // namespace

int main() {
    test_submit_batch_preserves_order_and_signals();
    test_submit_batch_validates_all_entries_before_enqueue();
    return 0;
}
