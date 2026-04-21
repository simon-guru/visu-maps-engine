/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <type_traits>

namespace vme::engine::gfx::resources {

/**
 * @brief Categoria de erro estável para contratos de recursos.
 *
 * Decisão de design:
 * - manter enum fechado evita expor códigos específicos de API (VkResult/MTL errors/etc.).
 * - facilita telemetria e tratamento uniforme de erro entre backends.
 */
enum class ResourceErrorCode : std::uint16_t {
    Ok = 0,
    InvalidArgument,
    IncompatibleUsage,
    OutOfRange,
    CapabilityNotSupported,
};

template <typename Enum>
concept EnumClass = std::is_enum_v<Enum>;

template <EnumClass Enum>
struct EnableBitMaskOps : std::false_type {};

/**
 * @brief OR bit-a-bit para enums de flags.
 *
 * Decisão de design:
 * - flags abstratas reduzem acoplamento com semântica de uma API gráfica específica.
 * - o static_assert força habilitação explícita por enum, evitando uso acidental.
 */
template <EnumClass Enum>
[[nodiscard]] constexpr Enum operator|(Enum lhs, Enum rhs) noexcept {
    static_assert(EnableBitMaskOps<Enum>::value, "bitmask operations are not enabled for this enum");
    using U = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

template <EnumClass Enum>
[[nodiscard]] constexpr Enum operator&(Enum lhs, Enum rhs) noexcept {
    static_assert(EnableBitMaskOps<Enum>::value, "bitmask operations are not enabled for this enum");
    using U = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

template <EnumClass Enum>
constexpr Enum& operator|=(Enum& lhs, Enum rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

/**
 * @brief Verifica se todas as flags solicitadas estão presentes.
 *
 * Decisão de design:
 * - padroniza checagens de permissão (usage) na camada de validação.
 * - evita duplicação de casts para tipo subjacente em vários arquivos.
 */
template <EnumClass Enum>
[[nodiscard]] constexpr bool has_flag(Enum value, Enum flag) noexcept {
    static_assert(EnableBitMaskOps<Enum>::value, "bitmask operations are not enabled for this enum");
    using U = std::underlying_type_t<Enum>;
    return (static_cast<U>(value) & static_cast<U>(flag)) == static_cast<U>(flag);
}

}  // namespace vme::engine::gfx::resources
