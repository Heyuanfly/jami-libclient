/****************************************************************************
 *    Copyright (C) 2016-2022 Savoir-faire Linux Inc.                       *
 *   Author: Olivier Grégoire <olivier.gregoire@savoirfairelinux.com>       *
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

#include "smartinfohub.h"
#include "private/smartInfoHub_p.h"

#include <dbus/videomanager.h>
#include <dbus/callmanager.h>
#include <dbus/callmanager.h>

SmartInfoHub::SmartInfoHub()
{
    d_ptr = new SmartInfoHubPrivate;
    connect(&CallManager::instance(),
            &CallManagerInterface::SmartInfo,
            this,
            &SmartInfoHub::slotSmartInfo,
            Qt::QueuedConnection);
}

SmartInfoHub::~SmartInfoHub() {}

void
SmartInfoHub::start()
{
    CallManager::instance().startSmartInfo(d_ptr->m_refreshTimeInformationMS);
}

void
SmartInfoHub::stop()
{
    CallManager::instance().stopSmartInfo();
}

SmartInfoHub&
SmartInfoHub::instance()
{
    // Singleton
    static SmartInfoHub instance_;
    return instance_;
}

void
SmartInfoHub::setRefreshTime(uint32_t timeMS)
{
    d_ptr->m_refreshTimeInformationMS = timeMS;
}

// Retrieve information from the map and implement all the variables
void
SmartInfoHub::slotSmartInfo(const MapStringString& map)
{
    for (int i = 0; i < map.size(); i++)
        d_ptr->m_information[map.keys().at(i)] = map[map.keys().at(i)];

    emit changed();
}
// Getter

bool
SmartInfoHub::isConference() const
{
    return (d_ptr->m_information["type"] == "conference");
}

float
SmartInfoHub::localFps() const
{
    if (!d_ptr->m_information[LOCAL_FPS].isEmpty())
        return d_ptr->m_information[LOCAL_FPS].toFloat();

    return 0.0;
}

float
SmartInfoHub::remoteFps() const
{
    if (!d_ptr->m_information[REMOTE_FPS].isEmpty())
        return d_ptr->m_information[REMOTE_FPS].toFloat();

    return 0.0;
}

int
SmartInfoHub::remoteWidth() const
{
    if (!d_ptr->m_information[REMOTE_WIDTH].isEmpty())
        return d_ptr->m_information[REMOTE_WIDTH].toInt();
    else
        return 0;
}

int
SmartInfoHub::remoteHeight() const
{
    if (!d_ptr->m_information[REMOTE_HEIGHT].isEmpty())
        return d_ptr->m_information[REMOTE_HEIGHT].toInt();
    else
        return 0;
}

int
SmartInfoHub::localWidth() const
{
    if (!d_ptr->m_information[LOCAL_WIDTH].isEmpty())
        return d_ptr->m_information[LOCAL_WIDTH].toInt();
    else
        return 0;
}

int
SmartInfoHub::localHeight() const
{
    if (!d_ptr->m_information[LOCAL_HEIGHT].isEmpty())
        return d_ptr->m_information[LOCAL_HEIGHT].toInt();
    else
        return 0;
}

QString
SmartInfoHub::callID() const
{
    if (!d_ptr->m_information[CALL_ID].isEmpty())
        return d_ptr->m_information[CALL_ID];
    else
        return SmartInfoHubPrivate::DEFAULT_RETURN_VALUE_QSTRING;
}

QString
SmartInfoHub::localVideoCodec() const
{
    if (!d_ptr->m_information[LOCAL_VIDEO_CODEC].isEmpty())
        return d_ptr->m_information[LOCAL_VIDEO_CODEC];
    else
        return SmartInfoHubPrivate::DEFAULT_RETURN_VALUE_QSTRING;
}

QString
SmartInfoHub::localAudioCodec() const
{
    if (!d_ptr->m_information[LOCAL_AUDIO_CODEC].isEmpty())
        return d_ptr->m_information[LOCAL_AUDIO_CODEC];
    else
        return SmartInfoHubPrivate::DEFAULT_RETURN_VALUE_QSTRING;
}

QString
SmartInfoHub::remoteVideoCodec() const
{
    if (!d_ptr->m_information[REMOTE_VIDEO_CODEC].isEmpty())
        return d_ptr->m_information[REMOTE_VIDEO_CODEC];
    else
        return SmartInfoHubPrivate::DEFAULT_RETURN_VALUE_QSTRING;
}

QString
SmartInfoHub::remoteAudioCodec() const
{
    if (!d_ptr->m_information[REMOTE_AUDIO_CODEC].isEmpty())
        return d_ptr->m_information[REMOTE_AUDIO_CODEC];
    else
        return SmartInfoHubPrivate::DEFAULT_RETURN_VALUE_QSTRING;
}
