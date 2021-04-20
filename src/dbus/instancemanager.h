/****************************************************************************
 *    Copyright (C) 2009-2021 Savoir-faire Linux Inc.                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#pragma once

#ifdef ENABLE_TEST
#include "../../test/mocks/instancemanager_mock.h"
#else
#ifdef ENABLE_LIBWRAP
#include "../qtwrapper/instancemanager_wrap.h"
#else
#include "instance_dbus_interface.h"
#include <QDBusPendingReply>
#endif
#endif
#include <typedefs.h>

namespace InstanceManager {

LIB_EXPORT InstanceManagerInterface& instance(bool muteDring = false);

}
