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

#include <RekordBuddyCollection/Implementation/Tracks/Track.hpp>
#include <RekordBuddyCollection/Implementation/Collection.hpp>
#include <RekordBuddyCollectionImplementationPersistence/Persistence.hpp>

// -- Must be last as it defines macros on some versions of MacOS that interfere with SQLITE
#import "Cocoa/Cocoa.h"

using namespace NxA;
using namespace NxA::RekordBuddy::CollectionImplementation;

// -- Instance Methods

void Track::setRelativeFilePath(const FilePath& path)
{
    auto internal = nxa_internal;
    internal->setFilePathRelativeToCollectionVolume(path.pathForCrossPlatformSerialization());

    auto fullPathAsString = FilePath::filePathByJoiningPaths(this->collection().asReference().volume(), path).asEncodedString();
    NSURL *url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:fullPathAsString.asUTF8()]];
    if (!url) {
        internal->setMaybeMacOSBookmark(nothing);
    }
    else {
        NSData* bookmark = [url bookmarkDataWithOptions:0 includingResourceValuesForKeys:nil relativeToURL:nil error:nil];
        internal->setMaybeMacOSBookmark(bookmark ? Optional<Blob>(Blob::withMemoryAndSize(static_cast<const byte*>(bookmark.bytes),
                                                                                          bookmark.length)) : nothing);
    }
}
