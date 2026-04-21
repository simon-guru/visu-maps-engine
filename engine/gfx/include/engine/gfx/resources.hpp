/*
# Copyright (c) 2026. VISU LLC. All rights reserved.
# Author: Simão A.Mayunga
# Date: 21/04/2026
# This file is part of VISU LLC.
*/

#pragma once

// Ponto de entrada estável para contratos de recursos GPU.
// Decisão de design:
// - manter include único simplifica adoção incremental pelos módulos consumidores.
#include "engine/gfx/resources/factory.hpp"
#include "engine/gfx/resources/backend_translation.hpp"
#include "engine/gfx/resources/debug_lifecycle.hpp"
#include "engine/gfx/resources/interfaces.hpp"
#include "engine/gfx/resources/resource_common.hpp"
#include "engine/gfx/resources/resource_desc.hpp"
#include "engine/gfx/resources/validation.hpp"
