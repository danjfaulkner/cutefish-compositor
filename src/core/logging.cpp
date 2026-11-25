// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/logging.h"

namespace properdesktop::core {

void setLoggingRules(std::string_view rules)
{
    base::logging::setRules(rules);
}

} // namespace properdesktop::core

const properdesktop::core::LogCategory cfWayland("properdesktop.wayland");
const properdesktop::core::LogCategory cfSession("properdesktop.session");
const properdesktop::core::LogCategory cfDrm("properdesktop.drm");
const properdesktop::core::LogCategory cfRender("properdesktop.render");
const properdesktop::core::LogCategory cfScene("properdesktop.scene");
const properdesktop::core::LogCategory cfCompositor("properdesktop.compositor");
const properdesktop::core::LogCategory cfInput("properdesktop.input");
const properdesktop::core::LogCategory cfInputTransitions("properdesktop.input.transitions");
const properdesktop::core::LogCategory cfCursorTransitions("properdesktop.cursor.transitions");
