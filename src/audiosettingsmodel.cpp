/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#include "audiosettingsmodel.h"
#include "dbus/configurationmanager.h"
#include "dbus/callmanager.h"

AudioSettingsModel* AudioSettingsModel::m_spInstance = nullptr;

///Constructor
AudioSettingsModel::AudioSettingsModel() : QObject(),m_EnableRoomTone(false),
 m_pAlsaPluginModel  (nullptr), m_pInputDeviceModel   (nullptr),
 m_pAudioManagerModel(nullptr), m_pRingtoneDeviceModel(nullptr),
 m_pOutputDeviceModel(nullptr)
{
   m_pRingtoneDeviceModel = new RingtoneDeviceModel (this);
}

///Destructor
AudioSettingsModel::~AudioSettingsModel()
{
   if ( m_pAlsaPluginModel     ) delete m_pAlsaPluginModel    ;
   if ( m_pInputDeviceModel    ) delete m_pInputDeviceModel   ;
   if ( m_pOutputDeviceModel   ) delete m_pOutputDeviceModel  ;
   if ( m_pAudioManagerModel   ) delete m_pAudioManagerModel  ;
   if ( m_pRingtoneDeviceModel ) delete m_pRingtoneDeviceModel;
}

///Singleton
AudioSettingsModel* AudioSettingsModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new AudioSettingsModel();
   return m_spInstance;
}

///Return plugin model (alsa only for the time being)
AlsaPluginModel* AudioSettingsModel::alsaPluginModel()
{
   if (!m_pAlsaPluginModel)
      m_pAlsaPluginModel = new AlsaPluginModel(this);
   return m_pAlsaPluginModel;
}

///Return the input device model
InputDeviceModel* AudioSettingsModel::inputDeviceModel()
{
   if (!m_pInputDeviceModel)
      m_pInputDeviceModel = new InputDeviceModel(this);
   return m_pInputDeviceModel;
}

///Return the output device model
OutputDeviceModel* AudioSettingsModel::outputDeviceModel()
{
   if (!m_pOutputDeviceModel)
      m_pOutputDeviceModel   = new OutputDeviceModel(this);
   return m_pOutputDeviceModel;
}

///Return audio manager
AudioManagerModel* AudioSettingsModel::audioManagerModel()
{
   if (!m_pAudioManagerModel)
      m_pAudioManagerModel = new AudioManagerModel(this);
   return m_pAudioManagerModel;
}

///Return the ringtone device model
RingtoneDeviceModel* AudioSettingsModel::ringtoneDeviceModel()
{
   if (!m_pRingtoneDeviceModel)
      m_pRingtoneDeviceModel = new RingtoneDeviceModel (this);
   return m_pRingtoneDeviceModel;
}

///Is the room tone (globally) enabled
bool AudioSettingsModel::isRoomToneEnabled()
{
   return m_EnableRoomTone;
}

///Reload everything
void AudioSettingsModel::reload()
{
   m_pAlsaPluginModel->reload();
   m_pInputDeviceModel->reload();
   m_pOutputDeviceModel->reload();
//    m_pAudioManagerModel->reload();
   m_pRingtoneDeviceModel->reload();
}

///Play room tone
AudioSettingsModel::ToneType AudioSettingsModel::playRoomTone() const
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   callManager.startTone(true,static_cast<int>(AudioSettingsModel::ToneType::WITHOUT_MESSAGE));
   //TODO support voicemail
   return AudioSettingsModel::ToneType::WITHOUT_MESSAGE;
}

///Stop room tone if it is playing
void AudioSettingsModel::stopRoomTone() const
{
   CallManagerInterface& callManager = DBus::CallManager::instance();
   callManager.startTone(false,0);
}

///Set if the roomtone is (globally) enabled
void AudioSettingsModel::setEnableRoomTone(bool enable)
{
   m_EnableRoomTone = enable;
}

///Enable noise suppress code, may make things worst
void AudioSettingsModel::setNoiseSuppressState(bool enabled)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setNoiseSuppressState(enabled);
}

///Enable noise suppress code, may make things worst
bool AudioSettingsModel::noiseSuppressState() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   return configurationManager.getNoiseSuppressState();
}

/****************************************************************
 *                                                              *
 *                        AlsaPluginModel                       *
 *                                                              *
 ***************************************************************/
///Constructor
AlsaPluginModel::AlsaPluginModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioPluginList();
}

///Destructor
AlsaPluginModel::~AlsaPluginModel()
{
   
}

///Re-implement QAbstractListModel data
QVariant AlsaPluginModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

///Re-implement QAbstractListModel rowCount
int AlsaPluginModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags AlsaPluginModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///Setting data is disabled
bool AlsaPluginModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Return the current index
QModelIndex AlsaPluginModel::currentPlugin() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const int idx = m_lDeviceList.indexOf(configurationManager.getCurrentAudioOutputPlugin());
   qDebug() << "Invalid current audio plugin";
   if (idx == -1)
      return QModelIndex();
   else
      return index(idx,0,QModelIndex());
}

///Set the current index
void AlsaPluginModel::setCurrentPlugin(const QModelIndex& idx)
{
   if (!idx.isValid())
      return;
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   configurationManager.setAudioPlugin(m_lDeviceList[idx.row()]);
}

///Set the current index (qcombobox compatibility shim)
void AlsaPluginModel::setCurrentPlugin(int idx)
{
   setCurrentPlugin(index(idx,0));
}

///Reload to current deamon state
void AlsaPluginModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioPluginList();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(m_lDeviceList.size()-1,0));
}


/****************************************************************
 *                                                              *
 *                       InputDeviceModel                       *
 *                                                              *
 ***************************************************************/

///Constructor
InputDeviceModel::InputDeviceModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioInputDeviceList  ();
}

///Destructor
InputDeviceModel::~InputDeviceModel()
{
   
}

///Re-implement QAbstractListModel data
QVariant InputDeviceModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

///Re-implement QAbstractListModel rowCount
int InputDeviceModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags InputDeviceModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///This model does not support setting data
bool InputDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Return the current input device index
QModelIndex InputDeviceModel::currentDevice() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
   const int idx = currentDevices[AudioSettingsModel::DeviceIndex::INPUT].toInt();
   if (idx >= m_lDeviceList.size())
      return QModelIndex();
   return index(idx,0);
}

///Set the current input device
void InputDeviceModel::setCurrentDevice(const QModelIndex& index)
{
   if (index.isValid()) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setAudioInputDevice(index.row());
   }
}

///QCombobox signals -> QModelIndex shim
void InputDeviceModel::setCurrentDevice(int idx)
{
   setCurrentDevice(index(idx,0));
}

///Reload input device list
void InputDeviceModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioInputDeviceList  ();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(m_lDeviceList.size()-1,0));
}


/****************************************************************
 *                                                              *
 *                       OutputDeviceModel                      *
 *                                                              *
 ***************************************************************/

///Constructor
OutputDeviceModel::OutputDeviceModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
}

///Destructor
OutputDeviceModel::~OutputDeviceModel()
{
   
}

///Re-implement QAbstractListModel data
QVariant OutputDeviceModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

///Re-implement QAbstractListModel rowCount
int OutputDeviceModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags OutputDeviceModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///This model is read only
bool OutputDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Return the current output device
QModelIndex OutputDeviceModel::currentDevice() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
   const int         idx            = currentDevices[AudioSettingsModel::DeviceIndex::OUTPUT].toInt();

   if (idx >= m_lDeviceList.size())
      return QModelIndex();
   return index(idx,0);
}

///Set the current output device
void OutputDeviceModel::setCurrentDevice(const QModelIndex& index)
{
   if (index.isValid()) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setAudioOutputDevice(index.row());
   }
}

///QCombobox index -> QModelIndex shim
void OutputDeviceModel::setCurrentDevice(int idx)
{
   setCurrentDevice(index(idx,0));
}

///reload output devices list
void OutputDeviceModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(m_lDeviceList.size()-1,0));
}

/****************************************************************
 *                                                              *
 *                        AudioManagerModel                      *
 *                                                              *
 ***************************************************************/

///Constructor
AudioManagerModel::AudioManagerModel(QObject* parent) : QAbstractListModel(parent)
{
   m_lDeviceList << "ALSA" << "Pulse Audio";
}

///Destructor
AudioManagerModel::~AudioManagerModel()
{
   m_lDeviceList.clear();
}

///Re-implement QAbstractListModel data
QVariant AudioManagerModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

///Re-implement QAbstractListModel rowCount
int AudioManagerModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags AudioManagerModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///This model is read only
bool AudioManagerModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

/**
 * Return the current audio manager
 * @warning Changes to the current index model will invalid Input/Output/Ringtone devices models
 */
QModelIndex AudioManagerModel::currentManager() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QString manager = configurationManager.getAudioManager();
      if (manager == ManagerName::PULSEAUDIO)
         return index((int)Manager::PULSE,0);
      else if (manager == ManagerName::ALSA)
         return index((int)Manager::ALSA,0);
      return QModelIndex();
}

///Set current audio manager
void AudioManagerModel::setCurrentManager(const QModelIndex& idx)
{
   if (!idx.isValid())
      return;

   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   switch (static_cast<Manager>(idx.row())) {
      case Manager::PULSE:
         configurationManager.setAudioManager(ManagerName::PULSEAUDIO);
         AudioSettingsModel::instance()->reload();
         break;
      case Manager::ALSA:
         configurationManager.setAudioManager(ManagerName::ALSA);
         AudioSettingsModel::instance()->reload();
         break;
   };
}

///QCombobox -> QModelIndex shim
void AudioManagerModel::setCurrentManager(int idx)
{
   setCurrentManager(index(idx,0));
}

/****************************************************************
 *                                                              *
 *                       RingtoneDeviceModel                    *
 *                                                              *
 ***************************************************************/

///Constructor
RingtoneDeviceModel::RingtoneDeviceModel(QObject* parent) : QAbstractListModel(parent)
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
}

///Destructor
RingtoneDeviceModel::~RingtoneDeviceModel()
{
   
}

///Re-implement QAbstractListModel data
QVariant RingtoneDeviceModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   switch(role) {
      case Qt::DisplayRole:
         return m_lDeviceList[index.row()];
   };
   return QVariant();
}

///Re-implement QAbstractListModel rowCount
int RingtoneDeviceModel::rowCount( const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lDeviceList.size();
}

///Re-implement QAbstractListModel flags
Qt::ItemFlags RingtoneDeviceModel::flags( const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///RingtoneDeviceModel is read only
bool RingtoneDeviceModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Return the current ringtone device
QModelIndex RingtoneDeviceModel::currentDevice() const
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   const QStringList currentDevices = configurationManager.getCurrentAudioDevicesIndex();
   const int         idx            = currentDevices[AudioSettingsModel::DeviceIndex::RINGTONE].toInt();
   if (idx >= m_lDeviceList.size())
      return QModelIndex();
   return index(idx,0);
}

///Set the current ringtone device
void RingtoneDeviceModel::setCurrentDevice(const QModelIndex& index)
{
   if (index.isValid()) {
      ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
      configurationManager.setAudioRingtoneDevice(index.row());
   }
}

///QCombobox -> QModelIndex shim
void RingtoneDeviceModel::setCurrentDevice(int idx)
{
   setCurrentDevice(index(idx,0));
}

///Reload ringtone device list
void RingtoneDeviceModel::reload()
{
   ConfigurationManagerInterface& configurationManager = DBus::ConfigurationManager::instance();
   m_lDeviceList = configurationManager.getAudioOutputDeviceList();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(m_lDeviceList.size()-1,0));
}