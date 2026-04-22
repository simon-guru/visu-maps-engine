/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 14/04/2026
# This file is part of VISU LLC.
*/

#pragma once

#include <cstdint>
#include <memory>

#include "engine/gfx/contracts/i_gfx_device.hpp"
/**
    * @brief Contrato mínimo de instância gráfica.
    * Decisão de design:
    * - expõe apenas informações de adaptadores e criação de dispositivo;
    * - não expõe ponteiros ou handles nativos para preservar portabilidade.
    * - o resultado da criação do dispositivo inclui um campo de erro para evitar exceções e simplificar o fluxo de controle.
    * - o método de criação do dispositivo retorna um struct que encapsula tanto o dispositivo quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
    * - a estrutura de resultado inclui um método auxiliar `ok()` para facilitar a verificação de sucesso, promovendo um estilo de código mais limpo e expressivo.
    * - a estrutura de resultado também inclui um método estático `ok_result()` para criar facilmente um resultado de sucesso, promovendo consistência e clareza no código.
    * - a estrutura de resultado evita o uso de enums separados para códigos de erro, simplificando a estrutura e reduzindo a complexidade.
    * - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos ou mensagens) sem quebrar o contrato existente.
    * - a estrutura de resultado é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
 */
namespace vme::engine::gfx::contracts {

/**
 * @brief Informações sobre um adaptador gráfico.
 * Decisão de design:
 * - struct simples para encapsular informações básicas do adaptador;
 * - não expõe ponteiros ou handles nativos para preservar portabilidade.
 * - a estrutura `AdapterInfo` é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como tipo de adaptador, capacidades suportadas, etc.) sem quebrar o contrato existente.
 * - a estrutura `AdapterInfo` é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de informações detalhadas sobre o adaptador conforme necessário.
 */
struct AdapterInfo {
    // Identificador do adaptador, pode ser usado para seleção ou log.
    std::uint32_t adapter_id{0};
    // Nome do adaptador, útil para depuração e seleção de adaptadores.
    const char* name{"stub-adapter"};
};

/**
 * @brief Resultado da criação de um dispositivo gráfico.
 * Decisão de design:
 * - struct simples para encapsular ponteiro do dispositivo e código de erro, evitando exceções e simplificando o fluxo de controle.
 * - método `ok()` para facilitar verificações de sucesso.
 * - a estrutura de resultado é projetada para ser facilmente extensível no futuro, permitindo a adição de campos adicionais (como códigos de erro específicos ou mensagens) sem quebrar o contrato existente.
 * - a estrutura de resultado é projetada para ser facilmente integrável com sistemas de log ou monitoramento, permitindo a inclusão de mensagens de erro detalhadas ou códigos de erro específicos conforme necessário.
 */
struct CreateDeviceResult {
    // Em vez de usar um enum separado, podemos usar o ponteiro nulo para indicar falha, simplificando a estrutura.
    std::unique_ptr<IGfxDevice> device{};
    // O campo de erro é inicializado com um valor padrão de sucesso (kOk) para indicar que, por padrão, a criação é considerada bem-sucedida, a menos que o backend retorne um erro específico.
    QueueSubmitResult error{};

    // Método auxiliar para verificar se o resultado indica sucesso.
     [[nodiscard]] inline bool ok() 
     const noexcept { 
        // O resultado é considerado bem-sucedido se o código de erro indicar sucesso (kOk) e o ponteiro do dispositivo for válido (não nulo).
        return error.ok() && static_cast<bool>(device); 
    }
};

/**
 * @brief Contrato mínimo de instância gráfica.
 * Decisão de design:
 * - expõe apenas informações de adaptadores e criação de dispositivo;
 * - não expõe ponteiros ou handles nativos para preservar portabilidade.
 * - o resultado da criação do dispositivo inclui um campo de erro para evitar exceções e simplificar o fluxo de controle.
 * - o método de criação do dispositivo retorna um struct que encapsula tanto o dispositivo quanto o resultado da operação, permitindo uma verificação clara de sucesso/falha.
 */
class IGfxInstance {
public:
    virtual ~IGfxInstance() = default;

    [[nodiscard]] virtual std::uint32_t adapter_count() const noexcept = 0;
    [[nodiscard]] virtual AdapterInfo adapter_info(std::uint32_t adapter_index) const noexcept = 0;
    virtual CreateDeviceResult create_device(std::uint32_t adapter_index) = 0;
};

}  // namespace vme::engine::gfx::contracts
