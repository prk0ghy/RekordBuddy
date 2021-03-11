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

#pragma once

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <Widgets/TagWidget.hpp>
#include <Widgets/ColorEditor.hpp>
#include <Widgets/RatingEditor.hpp>

#include <UiMainWindow/MultiValueEditor.hpp>

#include <Base/Array.hpp>
#include <Base/NotNull.hpp>
#include <Base/Threading.hpp>
#include <Base/Variant.hpp>

#include <QDialog>
#include <QList>
#include <QModelIndex>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateTimeEdit>

#include <memory>

namespace Ui {

class TrackEditDialog;

}

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class AbstractTrackItem;
class MultiValueEditor;
class WorkQueue;

// -- Public Interface
class TrackEditDialog : public QDialog
{
    Q_OBJECT

    // -- Private Instance Variables
    Ui::TrackEditDialog* p_ui{ nullptr };
    NotNull<WorkQueue*> p_queue;
    boolean p_canEdit{ false };
    boolean p_editingMultiple{ false };
    Array<MultiValueEditor*> p_columns{ };

public:
    // -- Constructors & Destructors
    explicit TrackEditDialog(TrackEditorList& list, boolean canEdit, NotNull<WorkQueue*> withQueue, QWidget* parent);
    ~TrackEditDialog() override;

    // -- Instance Methods
    void loadFromTracks();
    void saveToTracksIfModified();

public slots:
    int exec() override
    {
        NXA_ASSERT_TRUE(Threading::isOnMainThread());
        return this->QDialog::exec();
    }
    void accept() override;

signals:
    // -- Signals
    void tracksChanged();
};

} }
