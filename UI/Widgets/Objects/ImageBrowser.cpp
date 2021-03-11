//
//  Rekord Buddy - The future proof music collection tool made by DJs for DJs.
//  Copyright (C) 2020-2021 Didier Malenfant (didier@rekordbuddy.org)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <QFileDialog>
#include <Widgets/ImageBrowser.hpp>

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QStandardPaths>
#include "ui_ImageBrowser.h"
#include "Widgets/moc_ImageBrowser.cpp"

using namespace NxA;
using namespace RekordBuddy;

ImageBrowser::ImageBrowser(QWidget* parent) : QWidget{parent}, ui{new Ui::ImageBrowser}, pixmapImage{nullptr}
{
    this->ui->setupUi(this);
    connect(this->ui->browseButton, &QAbstractButton::clicked, this, &ImageBrowser::onBrowse);
}

QImage ImageBrowser::image() const
{
    return this->currentImage;
}

void ImageBrowser::onBrowse()
{
    QString fileToLoad;
    QStringList dialogLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QString dialogLocation = dialogLocations.isEmpty() ? QStringLiteral("") : dialogLocations.first();
    QFileDialog dialog{this, QStringLiteral(""), dialogLocation};
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec() == 0) {
        return;
    }
    auto dialogFiles = dialog.selectedFiles();
    fileToLoad = dialogFiles.first();
    if (QImage imageToUse{fileToLoad}; !imageToUse.isNull()) {
        this->setImage(imageToUse);
    }
}

void ImageBrowser::setImage(const QImage& value)
{
    this->currentImage = value;
    delete this->pixmapImage;
    this->pixmapImage = new QGraphicsPixmapItem{QPixmap::fromImage(this->currentImage)};

    auto* scene = new QGraphicsScene;
    scene->addItem(this->pixmapImage);

    this->ui->graphicsView->setScene(scene);
    this->ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
