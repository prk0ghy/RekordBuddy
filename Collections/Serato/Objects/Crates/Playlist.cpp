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

#include <SeratoCollection/Crates/Playlist.hpp>
#include <SeratoCollection/Crates/Folder.hpp>
#include <SeratoCollection/Collection.hpp>

#include <SeratoCollectionImplementation/Tags/TagFactory.hpp>
#include <SeratoCollectionImplementation/Tags/CrateV1Tags.hpp>
#include <SeratoCollectionImplementation/Tags/VersionTag.hpp>

using namespace NxA;
using namespace NxA::Serato;

// -- Constructors & Destructors

MutablePlaylist::MutablePlaylist(CollectionImplementation::Crate&& from,
                                 Pointer<MutableCollection> inCollection,
                                 NotNull<MutableFolder*> withParentFolder,
                                 const Protected&) : p_collection{ inCollection },
                                                     p_parentFolder{ withParentFolder },
                                                     p_seratoPlaylist{ std::move(from) },
                                                     p_lastModificationTime{ this->p_seratoPlaylist.maybeLastModificationTimeInSeratoFolder((*this->p_collection).seratoDatabase().seratoFolderPath()).valueOr(Time::distantPast()) }
{

}

// -- Instance Methods

void MutablePlaylist::p_markAsModifiedNow()
{
    this->p_lastModificationTime = Time::currentTime();
    this->p_seratoPlaylist.p_markCrateAsModified();
    this->p_collection.asReference().markAsModifiedNow();
}

void MutablePlaylist::p_readPlaylistFromFolder(const Directory& seratoFolderPath)
{
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("           Read from crate '%s' in '%s'.", this->p_seratoPlaylist.fullCrateName().asUTF8(),
                         seratoFolderPath.asFilePath().asEncodedString().asUTF8());
#endif

    auto filePath = CollectionImplementation::Crate::p_crateFilePathForFullCrateNameInSeratoFolder(this->p_seratoPlaylist.fullCrateName(),
                                                                                                   seratoFolderPath);
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("             Read '%s'.", filePath.asEncodedString().asUTF8());
#endif
    auto maybeCrateFileData = File::maybeContentOfFileAt(filePath);
    if (!maybeCrateFileData.isValid()) {
        return;
    }

    auto tags = CollectionImplementation::TagFactory::parseTagsAt(maybeCrateFileData->data(), maybeCrateFileData->size());
    if (tags.length() == 0u) {
        return;
    }

    for (auto&& tag : tags) {
        switch (tag.identifier()) {
            case CollectionImplementation::crateVersionTagIdentifier: {
                auto& versionTag = static_cast<CollectionImplementation::VersionTag&>(tag);
                if (versionTag.value() != String{ CollectionImplementation::Crate::crateFileCurrentVersionString }) {
                    NXA_ALOG_WITH_FORMAT("Illegal crate file version for file '%s'.", filePath.asEncodedString().asUTF8());
                }
                break;
            }
            case CollectionImplementation::trackEntryTagIdentifier: {
                auto maybeFilePath = CollectionImplementation::TrackEntry::maybeRelativePathForEntryWithTag(static_cast<CollectionImplementation::ObjectTag&>(tag));
                if (maybeFilePath.isValid()) {
                    auto maybeTrack = (*this->p_collection).seratoDatabase().maybeExistingTrackForRelativeFilePath(*maybeFilePath);
                    if (maybeTrack.isValid()) {
                        this->p_seratoPlaylist.p_tracks.append(*maybeTrack);
                        break;
                    }
#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
                    else {
                        NXA_DLOG_WITH_FORMAT("           Found invalid track '%s'.", maybeFilePath->asEncodedString().asUTF8());
                    }
#endif
                }

                // -- Fall thru to default case is intentional here.
            }
            [[fallthrough]]; default: {
                this->p_seratoPlaylist.p_otherTags.append(tag);
                break;
            }
        }
    }

#if defined(NXA_PRINT_SERATO_TAG_DEBUG_INFO)
    NXA_DLOG_WITH_FORMAT("           Found %llu track entries.", this->p_seratoPlaylist.p_tracks.length());
#endif
}

NotNull<Common::MutableCollection*> MutablePlaylist::collection()
{
    return this->p_collection.asNotNull();
}

NotNull<const Common::Collection*> MutablePlaylist::collection() const
{
    return this->p_collection.asNotNull();
}

Optional<NotNull<const Common::Folder*>> MutablePlaylist::maybeParentFolder() const
{
    return { this->p_parentFolder.as<const Common::Folder*>() };
}

Optional<NotNull<Common::MutableFolder*>> MutablePlaylist::maybeParentFolder()
{
    return { this->p_parentFolder.as<Common::MutableFolder*>() };
}

Time MutablePlaylist::lastModificationTime() const
{
    return this->p_lastModificationTime;
}

void MutablePlaylist::setName(const String& name)
{
    this->p_seratoPlaylist.setName(this->p_parentFolder->nextAvailableNameForPlaylistNamed(name));

    this->p_markAsModifiedNow();
}

void MutablePlaylist::removeTrackAtIndex(count index)
{
    this->p_seratoPlaylist.removeTrackAtIndex(index);

    this->p_markAsModifiedNow();
}
