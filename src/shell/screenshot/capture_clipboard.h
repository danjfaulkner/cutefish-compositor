// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_SCREENSHOT_CAPTURE_CLIPBOARD_H_
#define PROPERDESKTOP_SHELL_SCREENSHOT_CAPTURE_CLIPBOARD_H_

#include <memory>

#include "base/thread_pool.h"
#include "ui/bitmap.h"

namespace properdesktop::clipboard {
class SelectionController;
}

namespace properdesktop::shell {

// The selection is offered at once and encoded on a worker; a reader that asks
// before that waits for it.
class CaptureClipboard
{
public:
    explicit CaptureClipboard(clipboard::SelectionController *selection);
    ~CaptureClipboard();

    // False when there is no clipboard to take, as while the session is locked.
    bool setImage(const ui::Bitmap &image);

private:
    clipboard::SelectionController *m_selection;
    base::Job m_encode;
};

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_SCREENSHOT_CAPTURE_CLIPBOARD_H_
