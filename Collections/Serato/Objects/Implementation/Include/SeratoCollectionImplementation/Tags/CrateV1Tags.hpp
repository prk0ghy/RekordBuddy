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

// -- Known crate tag identifiers.
constexpr uinteger32 crateVersionTagIdentifier                      = 'vrsn';
constexpr uinteger32 trackEntryTagIdentifier                        = 'otrk';
constexpr uinteger32 trackEntryPathTagIdentifier                    = 'ptrk';
constexpr uinteger32 crateRemoveTrackActionTagIdentifier            = 'ortk';
constexpr uinteger32 crateColumnSortingTagIdentifier                = 'osrt';
constexpr uinteger32 crateColumnNameTag                             = 'tvcn';
constexpr uinteger32 crateColumnSortDirectionTagIdentifier          = 'brev';
constexpr uinteger32 cateColumnVisibilityTagIdentifier              = 'ovct';
constexpr uinteger32 crateColumnWidthTagIdentifier                  = 'tvcw';
constexpr uinteger32 crateRemoveColumnVisibilityActionTagIdentifier = 'orvc';

} } }
