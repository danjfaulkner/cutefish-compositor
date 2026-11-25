// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "accelerators/accelerator_delegate.h"

#include "base/dbus.h"

namespace properdesktop::accelerators {

namespace {

template<typename... Args>
void call(const char *service, const char *path, const char *method, const Args &...arguments)
{
    if (base::dbus::Connection *bus = base::dbus::Connection::session())
        base::dbus::Proxy(bus, service, path, service).send(method, arguments...);
}

template<typename... Args>
void callScreenshot(const char *method, const Args &...arguments)
{
    call("org.properdesktop.Screenshot", "/Screenshot", method, arguments...);
}

void callOsd(const char *method)
{
    call("org.properdesktop.Osd", "/Osd", method);
}

} // namespace

void DBusAcceleratorDelegate::toggleAppList()
{
    if (launchpadToggle) {
        launchpadToggle();
        return;
    }
    call("org.properdesktop.Launchpad", "/Launchpad", "toggle");
}

void DBusAcceleratorDelegate::takeScreenshot(bool toClipboard)
{
    callScreenshot("takeScreenshot", toClipboard);
}

void DBusAcceleratorDelegate::takePartialScreenshot(bool toClipboard)
{
    callScreenshot("takePartialScreenshot", toClipboard);
}

void DBusAcceleratorDelegate::startCaptureMode()
{
    callScreenshot("startCaptureMode");
}

void DBusAcceleratorDelegate::stopScreenRecording()
{
    callScreenshot("stopRecording");
}

void DBusAcceleratorDelegate::toggleClipboardHistory()
{
    call("org.properdesktop.ClipboardHistory", "/ClipboardHistory", "toggle");
}

void DBusAcceleratorDelegate::mediaPlayPause()
{
    call("org.properdesktop.MenuBar", "/MenuBar", "mediaPlayPause");
}

void DBusAcceleratorDelegate::mediaPause()
{
    call("org.properdesktop.MenuBar", "/MenuBar", "mediaPause");
}

void DBusAcceleratorDelegate::mediaPrevious()
{
    call("org.properdesktop.MenuBar", "/MenuBar", "mediaPrevious");
}

void DBusAcceleratorDelegate::mediaNext()
{
    call("org.properdesktop.MenuBar", "/MenuBar", "mediaNext");
}

void DBusAcceleratorDelegate::mediaStop()
{
    call("org.properdesktop.MenuBar", "/MenuBar", "mediaStop");
}

void DBusAcceleratorDelegate::volumeDown()
{
    callOsd("volumeDown");
}

void DBusAcceleratorDelegate::volumeUp()
{
    callOsd("volumeUp");
}

void DBusAcceleratorDelegate::toggleMute()
{
    callOsd("toggleMute");
}

void DBusAcceleratorDelegate::toggleMicrophoneMute()
{
    callOsd("toggleMicrophoneMute");
}

void DBusAcceleratorDelegate::brightnessDown()
{
    callOsd("brightnessDown");
}

void DBusAcceleratorDelegate::brightnessUp()
{
    callOsd("brightnessUp");
}

void DBusAcceleratorDelegate::keyboardBrightnessDown()
{
    callOsd("keyboardBrightnessDown");
}

void DBusAcceleratorDelegate::keyboardBrightnessUp()
{
    callOsd("keyboardBrightnessUp");
}

} // namespace properdesktop::accelerators
