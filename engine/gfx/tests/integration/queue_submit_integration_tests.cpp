#include <cassert>

#include "engine/gfx/commands/factory.hpp"
#include "engine/gfx/contracts/factory.hpp"

namespace {

using vme::engine::gfx::commands::CommandBufferDesc;
using vme::engine::gfx::commands::DrawCommand;
using vme::engine::gfx::contracts::FenceState;
using vme::engine::gfx::contracts::FenceValue;
using vme::engine::gfx::contracts::QueueSubmitErrorCode;
using vme::engine::gfx::contracts::QueueTimeline;
using vme::engine::gfx::contracts::QueueType;
using vme::engine::gfx::contracts::SubmitBatch;
using vme::engine::gfx::contracts::SubmitInfo;
using vme::engine::gfx::contracts::TimelineSignalInfo;
using vme::engine::gfx::contracts::TimelineWaitInfo;

void test_submit_batch_preserves_order_and_signals() {
    auto encoder_a = vme::engine::gfx::commands::create_command_encoder_stub();
    auto encoder_b = vme::engine::gfx::commands::create_command_encoder_stub();

    assert(encoder_a->draw(DrawCommand {.vertex_count = 3}).ok());
    assert(encoder_b->draw(DrawCommand {.vertex_count = 6}).ok());

    const auto finish_a = encoder_a->finish(CommandBufferDesc {.label = "A"});
    const auto finish_b = encoder_b->finish(CommandBufferDesc {.label = "B"});
    assert(finish_a.ok());
    assert(finish_b.ok());

    QueueTimeline timeline{};
    FenceState fence_a{};
    FenceState fence_b{};

    SubmitInfo submit_a{};
    submit_a.command_buffer = finish_a.command_buffer.get();
    submit_a.fence_signal = {.fence = &fence_a, .value = FenceValue {3}};
    submit_a.signals.push_back(TimelineSignalInfo {.timeline = &timeline, .value = 2});

    SubmitInfo submit_b{};
    submit_b.command_buffer = finish_b.command_buffer.get();
    submit_b.fence_signal = {.fence = &fence_b, .value = FenceValue {4}};
    submit_b.waits.push_back(TimelineWaitInfo {.timeline = &timeline, .min_value = 2});

    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();

    const auto submit_first = queue->submit(submit_a);
    assert(submit_first.ok());

    const auto blocked_submit = queue->submit(submit_b);
    assert(blocked_submit.code == QueueSubmitErrorCode::kSyncUnresolved);

    assert(queue->pending_submission_count() == 1);
    assert(queue->process_next_submission().ok());
    assert(fence_a.completed_value.value == 3);
    assert(timeline.value == 2);
    assert(queue->completed_submission_count() == 1);

    const auto submit_second = queue->submit(submit_b);
    assert(submit_second.ok());
    assert(queue->pending_submission_count() == 1);
    assert(queue->process_next_submission().ok());
    assert(fence_b.completed_value.value == 4);
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

void test_process_next_submission_fails_when_queue_is_empty() {
    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();

    const auto process_result = queue->process_next_submission();
    assert(process_result.code == QueueSubmitErrorCode::kInvalidArgument);
}

void test_single_submit_can_signal_multiple_timelines() {
    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());

    const auto finish = encoder->finish(CommandBufferDesc {.label = "multi-signal"});
    assert(finish.ok());

    QueueTimeline graphics_timeline{};
    QueueTimeline transfer_timeline{};

    SubmitInfo submit{};
    submit.command_buffer = finish.command_buffer.get();
    submit.signals.push_back(TimelineSignalInfo {.timeline = &graphics_timeline, .value = 7});
    submit.signals.push_back(TimelineSignalInfo {.timeline = &transfer_timeline, .value = 5});

    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();
    assert(queue->submit(submit).ok());
    assert(queue->process_next_submission().ok());

    assert(graphics_timeline.value == 7);
    assert(transfer_timeline.value == 5);
}

void test_rejects_timeline_and_fence_regression() {
    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());
    const auto finish = encoder->finish(CommandBufferDesc {.label = "regression"});
    assert(finish.ok());

    QueueTimeline timeline {.value = 9};
    FenceState fence {.completed_value = FenceValue {6}};

    SubmitInfo submit{};
    submit.command_buffer = finish.command_buffer.get();
    submit.signals.push_back(TimelineSignalInfo {.timeline = &timeline, .value = 8});
    submit.fence_signal = {.fence = &fence, .value = FenceValue {5}};

    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();
    const auto submit_result = queue->submit(submit);
    assert(submit_result.code == QueueSubmitErrorCode::kInvalidArgument);
}

void test_submit_batch_supports_chained_dependencies_within_batch() {
    auto encoder_a = vme::engine::gfx::commands::create_command_encoder_stub();
    auto encoder_b = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder_a->draw(DrawCommand {.vertex_count = 3}).ok());
    assert(encoder_b->draw(DrawCommand {.vertex_count = 3}).ok());

    const auto finish_a = encoder_a->finish(CommandBufferDesc {.label = "chain-a"});
    const auto finish_b = encoder_b->finish(CommandBufferDesc {.label = "chain-b"});
    assert(finish_a.ok());
    assert(finish_b.ok());

    QueueTimeline timeline{};

    SubmitInfo first{};
    first.command_buffer = finish_a.command_buffer.get();
    first.signals.push_back(TimelineSignalInfo {.timeline = &timeline, .value = 2});

    SubmitInfo second{};
    second.command_buffer = finish_b.command_buffer.get();
    second.waits.push_back(TimelineWaitInfo {.timeline = &timeline, .min_value = 2});
    second.signals.push_back(TimelineSignalInfo {.timeline = &timeline, .value = 3});

    SubmitBatch batch{};
    batch.submissions.push_back(first);
    batch.submissions.push_back(second);

    auto queue = vme::engine::gfx::contracts::create_gfx_queue_stub();
    const auto batch_submit = queue->submit_batch(batch);
    assert(batch_submit.ok());
    assert(queue->pending_submission_count() == 2);

    assert(queue->process_next_submission().ok());
    assert(queue->process_next_submission().ok());
    assert(timeline.value == 3);
}

void test_device_create_queue_enables_end_to_end_submit() {
    auto device = vme::engine::gfx::contracts::create_gfx_device_stub();
    assert(device);

    const auto queue_result = device->create_queue(QueueType::kGraphics);
    assert(queue_result.ok());

    auto encoder = vme::engine::gfx::commands::create_command_encoder_stub();
    assert(encoder->draw(DrawCommand {.vertex_count = 3}).ok());
    const auto finish = encoder->finish(CommandBufferDesc {.label = "device-path"});
    assert(finish.ok());

    SubmitInfo submit{};
    submit.command_buffer = finish.command_buffer.get();

    assert(queue_result.queue->submit(submit).ok());
    assert(queue_result.queue->process_next_submission().ok());
    assert(queue_result.queue->completed_submission_count() == 1);
}

}  // namespace

int main() {
    test_submit_batch_preserves_order_and_signals();
    test_submit_batch_validates_all_entries_before_enqueue();
    test_process_next_submission_fails_when_queue_is_empty();
    test_single_submit_can_signal_multiple_timelines();
    test_rejects_timeline_and_fence_regression();
    test_submit_batch_supports_chained_dependencies_within_batch();
    test_device_create_queue_enables_end_to_end_submit();
    return 0;
}
