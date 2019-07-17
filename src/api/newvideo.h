/****************************************************************************
 *    Copyright (C) 2018-2019 Savoir-faire Linux Inc.                                  *
 *   Author: Sébastien Blin <sebastien.blin@savoirfairelinux.com>           *
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

// Std
#include <map>
#include <memory>
#include <string>
#include <vector>

// Qt
#include <qobject.h>

// Lrc
#include "typedefs.h"

// Qt
#include <QObject>
#include <QThread>

struct AVFrame;

namespace lrc
{

class RendererPimpl;

namespace api
{

namespace video
{

constexpr static const char PREVIEW_RENDERER_ID[] = "local";

using Channel = std::string;
using Resolution = std::string;
using Framerate = float;
using FrameratesList = std::vector<Framerate>;
using ResRateList = std::vector<std::pair<Resolution, FrameratesList>>;
using Capabilities = std::map<Channel, ResRateList>;

/**
 * This class is used by Renderer class to expose video data frame
 * that could be owned by instances of this class or shared.
 * If an instance carries data, "storage.size()" is greater than 0
 * and equals to "size", "ptr" is equals to "storage.data()".
 * If shared data is carried, only "ptr" and "size" are set.
 */
struct Frame {
   uint8_t*             ptr     { nullptr };
   std::size_t          size    { 0       };
   std::vector<uint8_t> storage {         };
   // Next variables are currently used with DirectRenderer only
   unsigned int         height  { 0       };
   unsigned int         width   { 0       };
};

enum DeviceType
{
    CAMERA,
    DISPLAY,
    FILE,
    INVALID
};

/**
 * This class describes the current rendered device
 */
struct RenderedDevice
{
    std::string name;
    DeviceType type = INVALID;
};

/**
 * This class describes current video settings
 */
struct Settings
{
    Channel channel = "";
    std::string name = "";
    Framerate rate = 0;
    Resolution size = "";
};

class LIB_EXPORT Renderer : public QObject {
    Q_OBJECT
public:
    Renderer(const std::string& id, Settings videoSettings,
        const std::string& shmPath = "", const bool useAVFrame = false);
    ~Renderer();

    /**
     * Start the renderer in its own thread
     */
    void initThread();
    /**
     * Update size and shmPath of a renderer
     * @param res new resolution "wxh"
     * @param shmPath new shmPath
     */
    void update(const std::string& res, const std::string& shmPath);
    /**
     * Stop renderer and thread
     */
    void quit();

    // Getters
    /**
     * @return if renderer is rendering
     */
    bool isRendering() const;
    /**
     * @return renderer's id
     */
    std::string getId() const;
    /**
     * @return current rendered frame
     */
    Frame currentFrame() const;

#if defined(ENABLE_LIBWRAP) || (defined __APPLE__)
    /**
     * @return current avframe
     */
    std::unique_ptr<AVFrame, void(*)(AVFrame*)> currentAVFrame() const;
#endif

    /**
     * @return current size
     */
    QSize size() const; // TODO convert into std format!

    // Utils
    /**
     * Start rendering
     */
    void startRendering();
    /**
     * Stop rendering
     */
    void stopRendering();
    /**
     * set to true to receive AVFrames from render
     */
    void useAVFrame(bool useAVFrame);

Q_SIGNALS:
    /**
     * Emitted when a new frame is ready
     * @param id
     */
    void frameUpdated(const std::string& id);

private:
    std::unique_ptr<RendererPimpl> pimpl_;
};

} // namespace video
} // namespace api
} // namespace lrc
