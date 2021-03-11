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

#include <UiMainWindow/PredicateEditorDialog.hpp>
#include <UiMainWindow/TrackEditDialog.hpp>

#include <AbstractViewCollection/TrackList/AbstractTrackItem.hpp>

#include <RekordBuddyCollection/UserPreferences.hpp>

#include <CommonCollection/Markers/MarkerValidation.hpp>
#include <CommonCollection/Markers/MarkerOffset.hpp>

#include <Widgets/ConfirmationMessageBox.hpp>

#include <WorkQueue/WorkQueue.hpp>

#include <Base/Set.hpp>
#include <Base/Array.hpp>

#include "ui_TrackEditDialog.h"
#include "UiMainWindow/moc_TrackEditDialog.cpp"

#include <QModelIndex>
#include <QModelIndexList>
#include <QDate>

using namespace NxA;
using namespace RekordBuddy;

#define DISPLAY_COLUMN(name) \
    do { \
        mutableColumns.append(this->p_ui->show##name); \
        this->p_ui->show##name->associateColumn(Column<decltype(std::declval<AbstractTrackItem>().get##name()), \
                                                   AbstractTrackItem>{ withTracks, \
                                                                       #name, \
                                                                       &AbstractTrackItem::get##name }); \
    } while (false)
#define DISPLAY_COLUMN_WITH_TRANSFORM(name, transform) \
    do { \
        mutableColumns.append(this->p_ui->show##name); \
        this->p_ui->show##name->associateColumn(Column<decltype(std::declval<AbstractTrackItem>().get##name()), \
                                                   AbstractTrackItem>{ withTracks, \
                                                                       #name, \
                                                                       &AbstractTrackItem::get##name, \
                                                                       std::function<QString(decltype(std::declval<AbstractTrackItem>().get##name()))>(transform) }); \
    } while (false)
#define EDITABLE_COLUMN(name) \
    do { \
        mutableColumns.append(this->p_ui->edit##name); \
        if (this->p_canEdit) { \
            this->p_ui->edit##name->associateColumn(Column<decltype(std::declval<AbstractTrackItem>().get##name()), \
                                 AbstractTrackItem>{ withTracks, \
                                                     #name, \
                                                     &AbstractTrackItem::get##name, \
                                                     &AbstractTrackItem::set##name }); \
        } else { \
            this->p_ui->edit##name->setDisabled(true); \
            this->p_ui->edit##name->associateColumn(Column<decltype(std::declval<AbstractTrackItem>().get##name()), \
                                                       AbstractTrackItem>{ withTracks, \
                                                                           #name, \
                                                                           &AbstractTrackItem::get##name }); \
        } \
    } while (false)

TrackEditDialog::TrackEditDialog(TrackEditorList& withTracks,
                                 boolean withCanEdit,
                                 NotNull<WorkQueue*> withQueue,
                                 QWidget* parent) : QDialog{ parent },
                                                    p_ui{ new Ui::TrackEditDialog },
                                                    p_queue{ withQueue },
                                                    p_canEdit{ withCanEdit }
{
    NXA_ASSERT_TRUE(Threading::isOnMainThread());

    this->p_ui->setupUi(this);
    this->setWindowModality(Qt::WindowModal);
    this->p_editingMultiple = withTracks.length() > 1;

    MutableArray<MultiValueEditor*> mutableColumns;
    DISPLAY_COLUMN_WITH_TRANSFORM(TrackAbsoluteFilePath, [](auto v) { return v; });
    EDITABLE_COLUMN(TrackTitle);
    EDITABLE_COLUMN(TrackBeatsPerMinute);
    EDITABLE_COLUMN(TrackComments);
    EDITABLE_COLUMN(TrackAlbum);
    EDITABLE_COLUMN(TrackRecordLabel);
    EDITABLE_COLUMN(TrackMixName);
    EDITABLE_COLUMN(TrackGrouping);
    EDITABLE_COLUMN(TrackArtists);
    EDITABLE_COLUMN(TrackProducers);
    EDITABLE_COLUMN(TrackRemixers);
    EDITABLE_COLUMN(TrackGenres);
    EDITABLE_COLUMN(TrackTags);
    EDITABLE_COLUMN(TrackKeys);
    EDITABLE_COLUMN(TrackDateReleased);
    EDITABLE_COLUMN(TrackDateAdded);
    EDITABLE_COLUMN(TrackNumber);
    EDITABLE_COLUMN(TrackPlayCount);
    DISPLAY_COLUMN_WITH_TRANSFORM(TrackTimeLastModified, [](QDateTime value) { QLocale locale; return locale.toString(value, QLocale::ShortFormat); });
    DISPLAY_COLUMN(TrackLength);
    EDITABLE_COLUMN(TrackRating);
    EDITABLE_COLUMN(TrackColor);
    EDITABLE_COLUMN(TrackArtwork);

    this->p_columns = std::move(mutableColumns);
    this->loadFromTracks();

    if (this->p_editingMultiple) {
        this->p_ui->markerInfo->setText("Multiple tracks selected.");
    }
    else {
        auto [modelIndex, track] = withTracks.firstObject();
        auto markers = track->getMarkers();

        auto markerInfoString = MutableString::stringWithFormat("Found %llu marker", markers.length());
        auto absoluteFilePath = track->getTrackAbsoluteFilePath();
        auto maybeAsfilePath = FilePath{ fromString(absoluteFilePath) };
        if (Common::MarkerOffset::maybeOffsetToAddInSecondsForFileAtWhenImportingFrom(maybeAsfilePath, Common::Collection::Type::rekordbox).isValid()) {
            markerInfoString.append("s."_String);
        }
        else {
            markerInfoString.append("(s)."_String);
        }

        for (auto&& marker : markers) {
            markerInfoString.append("\n"_String);

            withVariant(marker, [&markerInfoString](auto& marker) {
                markerInfoString.append(marker->description());
            });
        }

        MutableArray<String> reasons;

        auto userPreferences = RekordBuddy::UserPreferences::forVolume(Volume::musicFolderVolume());
        auto maximumNumberOfHotCuesForRekordbox = *userPreferences->maybeIntegerForKey(String{ NXA_MAXIMUM_NUMBER_OF_HOTCUES_TO_EXPORT_FOR_REKORDBOX_PREFERENCES_KEY });

        {
            Common::MarkerValidation validator;
            validator.setMaximumNumberOfHotCues(maximumNumberOfHotCuesForRekordbox);
            validator.maybeCorrectedMarkersWithWhenExportedTo(markers, Common::Collection::Type::rekordbox, &reasons);
            if (reasons.length()) {
                markerInfoString.append("\nMarkers/Grid are not completely compatible with rekordbox:"_String);

                for (auto&& reason : reasons) {
                    markerInfoString.append(String::stringWithFormat("\n   %s", reason.asUTF8()));
                }

                reasons.removeAll();
            }
        }

        {
            Common::MarkerValidation validator;
            validator.setMaximumNumberOfHotCues(8);
            validator.maybeCorrectedMarkersWithWhenExportedTo(markers, Common::Collection::Type::Serato, &reasons);
            if (reasons.length()) {
                markerInfoString.append("\nMarkers/Grid are not completely compatible with Serato:"_String);

                for (auto&& reason : reasons) {
                    markerInfoString.append(String::stringWithFormat("\n   %s", reason.asUTF8()));
                }

                reasons.removeAll();
            }
        }

        {
            Common::MarkerValidation validator;
            validator.maybeCorrectedMarkersWithWhenExportedTo(markers, Common::Collection::Type::Traktor, &reasons);
            if (reasons.length()) {
                markerInfoString.append("\nMarkers/Grid are not completely compatible with Traktor:"_String);

                for (auto&& reason : reasons) {
                    markerInfoString.append(String::stringWithFormat("\n   %s", reason.asUTF8()));
                }

                reasons.removeAll();
            }
        }

        this->p_ui->markerInfo->setText(fromString(markerInfoString));

#if defined(NXA_MARKER_COPY_DEBUG_INFO)
        NXA_BETA_LOG_WITH_FORMAT("Displaying info for track '%s' in %s.\n",
               absoluteFilePath.toUtf8().constData(),
               track->fullNameOfParentCollection().toUtf8().constData());
        NXA_BETA_LOG_WITH_FORMAT("%s\n", markerInfoString.asUTF8());
#endif
    }
}

void TrackEditDialog::accept()
{
    if (!this->p_editingMultiple || ConfirmationMessageBox::promptForConfirmOkCancel(QObject::tr("Editing Multiple; Commit changes to all tracks?"),
                                                                                     QObject::tr("Are you sure you want save your changes to all selected tracks?"),
                                                                                     QObject::tr(""))) {
        this->setEnabled(false);

        this->p_queue->enqueueJob(0, [this](auto job) {
            this->saveToTracksIfModified();

            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
        });
    }
}

void TrackEditDialog::loadFromTracks()
{
    for (auto&& column : this->p_columns) {
        column->loadFromTracks();
    }
}

void TrackEditDialog::saveToTracksIfModified()
{
    boolean somethingWasModified = false;

    for (auto&& column : this->p_columns) {
        if (column->saveToTracksIfModified()) {
            somethingWasModified = true;
        }
    }

    if (somethingWasModified) {
        emit tracksChanged();
    }
}

TrackEditDialog::~TrackEditDialog()
{
    delete this->p_ui;
}
