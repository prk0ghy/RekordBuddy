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

#include <Base/Base.hpp>

// -- Constants

namespace NxA { namespace Serato { namespace CollectionImplementation {

// -- Known database Tag identifiers.
constexpr uinteger32 databaseVersionTagIdentifier                   = 'vrsn';
constexpr uinteger32 trackObjectTagIdentifier                       = 'otrk';
constexpr uinteger32 trackFilePathTagIdentifier                     = 'pfil';
constexpr uinteger32 databaseDirectoryPathTagIdentifier             = 'pdir';
constexpr uinteger32 databaseVideoFilePathTagIdentifier             = 'pvid';
constexpr uinteger32 trackFileTypeTagIdentifier                     = 'ttyp';
constexpr uinteger32 trackTitleTagIdentifier                        = 'tsng';
constexpr uinteger32 trackArtistTagIdentifier                       = 'tart';
constexpr uinteger32 trackAlbumTagIdentifier                        = 'talb';
constexpr uinteger32 trackGenreTagIdentifier                        = 'tgen';
constexpr uinteger32 trackLengthTagIdentifier                       = 'tlen';
constexpr uinteger32 trackSizeStringTagIdentifier                   = 'tsiz';
constexpr uinteger32 trackBitrateTagIdentifier                      = 'tbit';
constexpr uinteger32 trackSampleRateTagIdentifier                   = 'tsmp';
constexpr uinteger32 trackBpmTagIdentifier                          = 'tbpm';
constexpr uinteger32 trackCommentsTagIdentifier                     = 'tcom';
constexpr uinteger32 trackGroupingTagIdentifier                     = 'tgrp';
constexpr uinteger32 trackRemixerTagIdentifier                      = 'trmx';
constexpr uinteger32 trackLabelTagIdentifier                        = 'tlbl';
constexpr uinteger32 trackComposerTagIdentifier                     = 'tcmp';
constexpr uinteger32 trackYearTagIdentifier                         = 'ttyr';
constexpr uinteger32 trackCrateNameTagIdentifier                    = 'tcrt';
constexpr uinteger32 trackDateAddedStringTagIdentifier              = 'tadd';
constexpr uinteger32 trackIsCorruptedMessageTagIdentifier           = 'tcor';
constexpr uinteger32 trackVideoEffectsTagIdentifier                 = 'tvfx';
constexpr uinteger32 trackKeyTagIdentifier                          = 'tkey';
constexpr uinteger32 trackItunesIdentifierTagIdentifier             = 'tiid';
constexpr uinteger32 trackNumberTagIdentifier                       = 'utkn';
constexpr uinteger32 trackDiscNumberTagIdentifier                   = 'udsc';
constexpr uinteger32 trackLabelColorTagIdentifier                   = 'ulbl';
constexpr uinteger32 trackModificationTimeTagIdentifier             = 'utme';
constexpr uinteger32 trackPlayCount                                 = 'utpc';
constexpr uinteger32 trackDateAddedTagIdentifier                    = 'uadd';
constexpr uinteger32 trackSizeTagIdentifier                         = 'ufsb';
constexpr uinteger32 trackLastAnalyzedVersionTagIdentifier          = 'sbav';
constexpr uinteger32 trackMetadataWasReadTagIdentifier              = 'bhrt';
constexpr uinteger32 trackFileIsMissingTagIdentifier                = 'bmis';
constexpr uinteger32 trackWasRecentlyPlayedTagIdentifier            = 'bply';
constexpr uinteger32 trackLoopingTagIdentifier                      = 'blop';
constexpr uinteger32 trackIsFromItunesTagIdentifier                 = 'bitu';
constexpr uinteger32 trackIsReadOnlyTagIdentifier                   = 'biro';
constexpr uinteger32 trackWasAnalyzedTagIdentifier                  = 'bovc';
constexpr uinteger32 trackIsCorruptedTagIdentifier                  = 'bcrt';
constexpr uinteger32 trackIsWhiteLabelTagIdentifier                 = 'bwlb';
constexpr uinteger32 trackIsAccessControlledWhiteLabelTagIdentifier = 'bwll';
constexpr uinteger32 trackFileTypeIsUnsupportedTagIdentifier        = 'buns';
constexpr uinteger32 trackBeatGridIsLockedTagIdentifier             = 'bbgl';

} } }
