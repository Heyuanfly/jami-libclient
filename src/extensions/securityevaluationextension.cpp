/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
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
#include "securityevaluationextension.h"
#include "collectioninterface.h"
#include "contactmethod.h"
#include "certificate.h"
#include "person.h"
#include "account.h"
#include "call.h"
#include "presencestatusmodel.h"
#include "collectionextensionmodel.h"
#include <delegates/pixmapmanipulationdelegate.h>
#include <private/securityevaluationmodel_p.h>

//Qt
#include <QtCore/QMetaObject>

//LibStdC++
#include <utility>

DECLARE_COLLECTION_EXTENSION(SecurityEvaluationExtension)

class SecurityEvaluationExtensionPrivate
{
public:
   //Helpers
   SecurityEvaluationModel::SecurityLevel checkCertificate(ItemBase* i);
};

SecurityEvaluationExtension::SecurityEvaluationExtension(QObject* parent) :
   CollectionExtensionInterface(parent), d_ptr(new SecurityEvaluationExtensionPrivate())
{

}

SecurityEvaluationExtension::~SecurityEvaluationExtension()
{
   delete d_ptr;
}

QVariant SecurityEvaluationExtension::data(int role) const
{
   Q_UNUSED(role)

   if (role == Qt::DisplayRole) {
      return QObject::tr("Security evaluation");
   }

   return QVariant();
}

QVariant SecurityEvaluationExtension::securityLevelIcon(ItemBase* item) const
{
   const SecurityEvaluationModel::SecurityLevel sl = securityLevel(item);

   return PixmapManipulationDelegate::instance()->securityLevelIcon(sl);
}

SecurityEvaluationModel::SecurityLevel SecurityEvaluationExtension::securityLevel(ItemBase* item) const
{
   enum Types {
      OTHER         ,
      ACCOUNT       ,
      CALL          ,
      CERTIFICATE   ,
      CONTACT_METHOD,
      PERSON        ,
   };

   static QHash<const QMetaObject*, Types> types {
      { &Account       :: staticMetaObject , ACCOUNT        },
      { &Call          :: staticMetaObject , CALL           },
      { &Certificate   :: staticMetaObject , CERTIFICATE    },
      { &ContactMethod :: staticMetaObject , CONTACT_METHOD },
      { &Person        :: staticMetaObject , PERSON         },
   };

   switch(types[item->metaObject()]) {
      case Types::ACCOUNT       :
         break;
      case Types::CALL          :
         break;
      case Types::CERTIFICATE   :
         return d_ptr->checkCertificate(item);
      case Types::CONTACT_METHOD:
         break;
      case Types::PERSON        :
         break;
      case Types::OTHER         :
      default:
         return SecurityEvaluationModel::SecurityLevel::NONE;
   }

   return SecurityEvaluationModel::SecurityLevel::NONE;
}

SecurityEvaluationModel::SecurityLevel SecurityEvaluationExtensionPrivate::checkCertificate(ItemBase* i)
{
   Certificate* c = qobject_cast<Certificate*>(i);

   if (!c)
      return SecurityEvaluationModel::SecurityLevel::NONE;

   const bool reqPriv = c->requirePrivateKey();

   const SecurityEvaluationModel::SecurityLevel l = SecurityEvaluationModelPrivate::certificateSecurityLevel(c,true);


   return l;
}
