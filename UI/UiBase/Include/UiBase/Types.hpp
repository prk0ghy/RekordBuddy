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

#include <Base/FilePath.hpp>
#include <Base/String.hpp>

#include <QString>
#include <QMetaType>
#include <QStringList>

// Windows and MacOS on QT use different meanings for Points (pt), which is how fonts are sized.
// Windows uses 96pt/inch, and MacOS uses the postscript standard 72pt/inch. Hence, wherever
// points are used in QT, we have to munge them by 96/72 or 4/3. The result of not doing this
// is that windows will look too big OR macos fonts are too small.
#if defined(NXA_PLATFORM_MACOS)
#define NXA_NORMAL_FONT 13
#define NXA_SMALL_FONT 11
#elif defined(NXA_PLATFORM_WINDOWS)
#define NXA_NORMAL_FONT 10
#define NXA_SMALL_FONT 8
#else
#error Unsupported platform.
#endif

namespace NxA::RekordBuddy { namespace {

inline NxA::String fromString(const QString& from)
{
    auto encoded = from.toUtf8();
    return { encoded.data(), static_cast<count>(encoded.length()) };
}

inline QString fromString(const NxA::FilePath& from)
{
#if defined(NXA_PLATFORM_WINDOWS)
    return QString::fromStdWString(from.asPlatformNativeString());
#elif defined(NXA_PLATFORM_MACOS)
    return QString::fromStdString(from.asPlatformNativeString());
#else
    #error Unsupported platform.
#endif
}

inline QString fromString(const NxA::String& from)
{
    return QString::fromUtf8(from.asUTF8(), from.sizeInBytesOfStringAsUTF8());
}

inline FilePath fromLocalFile(const QString& from)
{
#if defined(NXA_PLATFORM_WINDOWS)
    FilePath::NativeStringType nativePath = from.toStdWString();
#elif defined(NXA_PLATFORM_MACOS)
    FilePath::NativeStringType nativePath = from.toStdString();
#else
    #error Unsupported platform.
#endif

    return FilePath{ nativePath };
}

} }
