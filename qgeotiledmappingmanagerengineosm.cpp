/****************************************************************************
**
** Copyright (C) 2013 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotiledmappingmanagerengineosm.h"
#include "qgeotilefetcherosm.h"

#include <osmarender/RenderTheme.h>
#include <osmarender/RenderThemeHandler.h>

#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmapdata_p.h>

#include <QFile>
#include <QFileInfo>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngineOsm::QGeoTiledMappingManagerEngineOsm(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) :
    QGeoTiledMappingManagerEngine()
{
    setCacheHint(MemoryCache);

    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(20.0);
    setCameraCapabilities(cameraCaps);

    setTileSize(QSize(256, 256));

    QList<QGeoMapType> mapTypes;
    mapTypes << QGeoMapType(QGeoMapType::StreetMap, tr("Street Map"), tr("OpenStreetMap street map"), false, false, 1);
    setSupportedMapTypes(mapTypes);

    if (!parameters.contains(QStringLiteral("map"))) {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = QStringLiteral("no \"map\" parameter specified");

        return;
    }

    QStringList stylePaths;
    if (parameters.contains(QStringLiteral("style"))) {
        const QString stylePath = parameters.value(QStringLiteral("style")).toString();
        stylePaths << stylePath;
    }
    stylePaths << QStringLiteral(":/osmarender/style.xml");

    RenderTheme *renderTheme = 0;
    foreach (const QString &stylePath, stylePaths) {
        QFile styleFile(stylePath);
        if (!styleFile.open(QFile::ReadOnly)) {
            qWarning() << "could not open style file" << styleFile.fileName();
            continue;
        }

        const QString canonicalStylePath = QFileInfo(stylePath).canonicalPath();
        RenderThemeHandler renderThemeHandler(canonicalStylePath);
        QXmlInputSource source(&styleFile);
        QXmlSimpleReader xmlReader;
        xmlReader.setContentHandler(&renderThemeHandler);
        xmlReader.setErrorHandler(&renderThemeHandler);
        xmlReader.parse(&source);
        renderTheme = renderThemeHandler.releaseRenderTheme();

        if (renderTheme != 0)
            break;
    }

    if (renderTheme == 0) {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = QStringLiteral("no render theme found, not even the built-in one");
        return;
    }

    const QString mapFilePath = parameters.value(QStringLiteral("map")).toString();
    QFile *mapDatabase = new QFile(mapFilePath);
    if (!mapDatabase->open(QFile::ReadOnly)) {
        *error = QGeoServiceProvider::MissingRequiredParameterError;
        *errorString = "Unable to open " + mapFilePath + " for reading.";
        delete renderTheme;
        delete mapDatabase;
        return;
    }

    QGeoTileFetcherOsm *tileFetcher = new QGeoTileFetcherOsm(mapDatabase, renderTheme);

    setTileFetcher(tileFetcher);

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QGeoTiledMappingManagerEngineOsm::~QGeoTiledMappingManagerEngineOsm()
{
}

QGeoMapData *QGeoTiledMappingManagerEngineOsm::createMapData()
{
    return new QGeoTiledMapData(this, 0);
}

QT_END_NAMESPACE
