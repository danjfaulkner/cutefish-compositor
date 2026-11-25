// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_CORE_LOGGING_H_
#define PROPERDESKTOP_CORE_LOGGING_H_

#include <string_view>

#include "base/logging.h"

namespace properdesktop::core {

using LogCategory = base::LogCategory;

// Applies `rules` (base::logging::setRules syntax) to every category.
void setLoggingRules(std::string_view rules);

} // namespace properdesktop::core

extern const properdesktop::core::LogCategory cfWayland;
extern const properdesktop::core::LogCategory cfSession;
extern const properdesktop::core::LogCategory cfDrm;
extern const properdesktop::core::LogCategory cfRender;
extern const properdesktop::core::LogCategory cfScene;
extern const properdesktop::core::LogCategory cfCompositor;
extern const properdesktop::core::LogCategory cfInput;
extern const properdesktop::core::LogCategory cfInputTransitions;
extern const properdesktop::core::LogCategory cfCursorTransitions;

#endif // PROPERDESKTOP_CORE_LOGGING_H_
