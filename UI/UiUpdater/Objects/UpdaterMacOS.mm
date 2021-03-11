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

#include <UiUpdater/Updater.hpp>

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

using namespace NxA::RekordBuddy;
using namespace NxA;

// -- Types

struct Updater::Private
{
    SUUpdater* p_updater;
};

// -- Constructors & Destructors

Updater::Updater(const String& aUrl)
{
#if defined(NXA_DISABLE_APP_UPDATER)
    NXA_DLOG("Auto updater is disabled. (NXA_DISABLE_APP_UPDATER)");
    return;
#endif

    NXA_DLOG_WITH_FORMAT("Updater initialized at: %s", aUrl.asUTF8());

    this->p_internal = new Private;

    this->p_internal->p_updater = [SUUpdater sharedUpdater];
    [this->p_internal->p_updater retain];

    NSURL* _Nullable url = [NSURL URLWithString:[NSString stringWithUTF8String:aUrl.asUTF8()]];
    [this->p_internal->p_updater setFeedURL:url];
}

Updater::~Updater()
{
#if !defined(NXA_DISABLE_APP_UPDATER)
    [this->p_internal->p_updater release];
    delete this->p_internal;
#endif
}

// -- Instance Methods

void Updater::checkForUpdatesInBackground()
{
#if defined(NXA_DISABLE_APP_UPDATER)
    NXA_DLOG("Auto updater is disabled. (NXA_DISABLE_APP_UPDATER)");
    return;
#endif

    [this->p_internal->p_updater checkForUpdatesInBackground];
}

void Updater::userCheckForUpdates()
{
#if defined(NXA_DISABLE_APP_UPDATER)
    NXA_DLOG("Auto updater is disabled. (NXA_DISABLE_APP_UPDATER)");
    return;
#endif

    [this->p_internal->p_updater checkForUpdates:this->p_internal->p_updater];
}
