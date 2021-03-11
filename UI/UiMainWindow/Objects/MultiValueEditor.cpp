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

#include <UiMainWindow/MultiValueEditor.hpp>
#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>
#include <qcolor.h>
#include "ui_MultiValueEditor.h"
#include "UiMainWindow/moc_MultiValueEditor.cpp"

using namespace NxA;
using namespace RekordBuddy;

MultiValueEditor::MultiValueEditor(QWidget* parent) : QWidget{parent}, ui{new Ui::MultiValueEditor}
{
    ui->setupUi(this);
    connect(ui->revertButton, &QPushButton::clicked, this, &MultiValueEditor::onRevert);
    connect(ui->editButton, &QPushButton::clicked, this, &MultiValueEditor::onEditMulti);
}

MultiValueEditor::~MultiValueEditor() = default;

void MultiValueEditor::associateColumn(BaseColumn column)
{
    this->columnToEdit = column;
    this->setShowingCountValuesWithEditable(1, true);
}

void MultiValueEditor::onRevert()
{
    if (!this->columnToEdit.isValid()) {
        return;
    }
    this->loadFromTracks();
}

void MultiValueEditor::onEditMulti()
{
    this->setShowingCountValuesWithEditable(1, true);
    this->currentEditor()->setFocus();
    this->overrideMulti = true;
}

EditorType MultiValueEditor::type() const
{
    auto maybeResult =
        this->columnToEdit.maybe([](const auto& column) { return column.apply([](const auto& thing) { return thing.uiType(); }); });
    return *maybeResult;
}

void MultiValueEditor::setShowingCountValuesWithEditable(count number, boolean editable)
{
    // if showing, then we want to hide all inputs and show the multi-value indicator and controls
    // otherwise, we're going to show only the editor control for the current `type()`.
    this->isMultiEditing = number != 1;
    this->ui->lineEdit->setVisible(false);
    this->ui->multiValueDescription->setVisible(false);
    this->ui->dateEdit->setVisible(false);
    this->ui->showTransformedValue->setVisible(false);
    this->ui->tagEdit->setVisible(false);
    this->ui->spinBox->setVisible(false);
    this->ui->ratingEdit->setVisible(false);
    this->ui->showDuration->setVisible(false);
    this->ui->imageEdit->setVisible(false);
    this->ui->colorEdit->setVisible(false);
    if (this->isMultiEditing) {
        this->ui->multiValueDescription->setText(QStringLiteral("%1 different value%2").arg(number).arg(number > 1 ? "s" : ""));
        this->ui->multiValueDescription->setVisible(true);
        this->ui->editButton->setVisible(true);
        this->ui->revertButton->setVisible(false);
    }
    else {
        this->currentEditor()->setVisible(true);
        this->ui->editButton->setVisible(false);
        if (type() == EditorType::QLabel) {
            this->ui->revertButton->setVisible(false);
        }
        else {
            this->ui->revertButton->setVisible(editable);
        }
    }
}

inline bool operator<(const QImage &a1, const QImage &a2)
{
    if (a1 == a2) {
        return false;
    }
    auto a1rect = a1.rect(), a2rect = a2.rect();
    if (a1rect.width() < a2rect.width() || a1rect.height() < a2rect.height()) {
        return true;
    }
    // -- todo: other ways to sort cheaply before doing pixel comparison
    for (int x = 0; x < a2rect.width(); ++x) {
        for (int y = 0; y < a2rect.height(); ++y) {
            if (a1.pixel(x, y) < a2.pixel(x, y)) {
                return true;
            }
        }
    }
    return false;
}

inline bool operator<(const QColor &a1, const QColor &a2)
{
    return a1.rgb() < a2.rgb();
}

void MultiValueEditor::loadFromTracks()
{
    overrideMulti = false;
    columnToEdit->apply([this](auto&& column) {
        boolean anyEditable{false};

        MutableSet<BaseColumnValue> trackValuesForColumn;
        for (auto&& [index, track] : column.trackList()) {
            anyEditable = anyEditable || track->canEdit();
            trackValuesForColumn.add(column.getFromTrack(track.get()));
        }

        this->setShowingCountValuesWithEditable(trackValuesForColumn.length(), anyEditable);

        if (trackValuesForColumn.length() == 1) {
            column.setToUI(this, trackValuesForColumn.anyObject());
        }
    });
}

NotNull<QWidget*> MultiValueEditor::currentEditor()
{
    switch (type()) {
        case EditorType::QImage:
            return this->ui->imageEdit;
        case EditorType::Duration:
            return this->ui->showDuration;
        case EditorType::QLabel:
            return this->ui->showTransformedValue;
        case EditorType::QColor:
            return this->ui->colorEdit;
        case EditorType::TrackRating:
            return this->ui->ratingEdit;
        case EditorType::QDate:
            return this->ui->dateEdit;
        case EditorType::QDateTime:
            return this->ui->dateEdit;
        case EditorType::QString:
            return this->ui->lineEdit;
        case EditorType::QStringList:
            return this->ui->tagEdit;
        case EditorType::count:
            return this->ui->spinBox;
    }
    NXA_ALOG("Failed to pick editor");
}

boolean MultiValueEditor::saveToTracksIfModified()
{
    if (this->isMultiEditing && !this->overrideMulti) {
        return false;
    }

    return this->columnToEdit->apply([this](auto&& column) {
        boolean somethingChanged = false;

        for (auto&& [index, track] : column.trackList()) {
            if (!track->canEdit()) {
                continue;
            }

            if (auto didChange = column.applyToTrackIfChangedInUI(this, track.get()); didChange) {
                track->markAsModifiedNow();
                somethingChanged = true;
            }
        }

        return somethingChanged;
    });
}
