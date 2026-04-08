/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 08/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// Este include atua como ponto de entrada estável para consumidores externos.
// Motivo da abordagem:
// - evita que aplicações dependam diretamente do subdiretório `contracts/`;
// - preserva compatibilidade de include path caso a organização interna mude;
// - mantém a API pública coesa sob `engine/core/*`.
#include "engine/core/contracts/i_engine_lifecycle.hpp"
