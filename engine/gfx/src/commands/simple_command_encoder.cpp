/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#include "engine/gfx/commands/factory.hpp"

#include <utility>

namespace vme::engine::gfx::commands {
namespace {

class StubCommandBuffer final : public ICommandBuffer {
public:
    StubCommandBuffer(CommandBufferDesc desc, std::uint64_t command_count)
        : desc_(std::move(desc)), command_count_(command_count) {}

    [[nodiscard]] const CommandBufferDesc& desc() const noexcept override { return desc_; }

    [[nodiscard]] std::uint64_t command_count() const noexcept override { return command_count_; }

private:
    CommandBufferDesc desc_{};
    std::uint64_t command_count_{};
};

class StubCommandEncoder final : public ICommandEncoder {
public:
    CommandError copy(const CopyBufferCommand& command) override {
        if (!is_recording_) {
            return {CommandErrorCode::InvalidState, "encoder is already finished"};
        }

        if (command.size_bytes == 0) {
            return {CommandErrorCode::InvalidArgument, "copy size must be greater than zero"};
        }

        ++recorded_commands_;
        return CommandError::ok_result();
    }

    CommandError dispatch(const DispatchCommand& command) override {
        if (!is_recording_) {
            return {CommandErrorCode::InvalidState, "encoder is already finished"};
        }

        if (command.group_count_x == 0 || command.group_count_y == 0 || command.group_count_z == 0) {
            return {CommandErrorCode::InvalidArgument, "dispatch group count must be greater than zero"};
        }

        ++recorded_commands_;
        return CommandError::ok_result();
    }

    CommandError draw(const DrawCommand& command) override {
        if (!is_recording_) {
            return {CommandErrorCode::InvalidState, "encoder is already finished"};
        }

        if (command.vertex_count == 0 || command.instance_count == 0) {
            return {CommandErrorCode::InvalidArgument, "draw counts must be greater than zero"};
        }

        ++recorded_commands_;
        return CommandError::ok_result();
    }

    CommandError barrier(const BarrierCommand& command) override {
        if (!is_recording_) {
            return {CommandErrorCode::InvalidState, "encoder is already finished"};
        }

        if (command.src_stage == command.dst_stage) {
            return {CommandErrorCode::InvalidArgument, "barrier requires distinct src/dst stages"};
        }

        ++recorded_commands_;
        return CommandError::ok_result();
    }

    FinishCommandBufferResult finish(const CommandBufferDesc& desc) override {
        if (!is_recording_) {
            return {nullptr,
                    {CommandErrorCode::InvalidState, "encoder finish cannot be called twice"}};
        }

        is_recording_ = false;

        return {std::make_unique<StubCommandBuffer>(desc, recorded_commands_), CommandError::ok_result()};
    }

private:
    bool is_recording_{true};
    std::uint64_t recorded_commands_{0};
};

}  // namespace

std::unique_ptr<ICommandEncoder> create_command_encoder_stub() {
    return std::make_unique<StubCommandEncoder>();
}

}  // namespace vme::engine::gfx::commands
