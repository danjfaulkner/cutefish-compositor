// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_SECRETS_VPN_SECRET_FIELDS_H_
#define PROPERDESKTOP_SHELL_SECRETS_VPN_SECRET_FIELDS_H_

#include <map>
#include <string>
#include <vector>

namespace properdesktop::shell {

struct VpnSecretField {
    std::string key;
    std::string label;
    bool secret = true;
};

// The hinted secrets when NetworkManager names them, else the plugin's own rules.
std::vector<VpnSecretField> vpnSecretFields(const std::string &serviceType,
                                            const std::map<std::string, std::string> &data,
                                            const std::vector<std::string> &hints);
std::string vpnHintMessage(const std::vector<std::string> &hints);

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_SECRETS_VPN_SECRET_FIELDS_H_
