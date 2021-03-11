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

#include <Widgets/TagWidget.hpp>
#include <Widgets/RatingEditor.hpp>
#include <Widgets/ColorEditor.hpp>
#include <Widgets/DurationViewer.hpp>
#include <Widgets/ImageBrowser.hpp>

#include <Base/NotNull.hpp>
#include <Base/Pointers.hpp>
#include <Base/Set.hpp>
#include <Base/Variant.hpp>

#include <QModelIndex>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateTimeEdit>

#include <qlist.h>
#include <qobject.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlabel.h>
#include <qstringlist.h>

class QMouseEvent;
class QLabel;
class QObject;
class QWidget;

namespace Ui {

// -- Forward Declarations
class MultiValueEditor;

}

namespace NxA { namespace RekordBuddy {

// -- Forward Declarations
class AbstractTrackItem;
class MultiValueEditor;

// -- Types
using MutableTrackEditorList = MutableArray<std::tuple<QModelIndex, NotNull<NxA::RekordBuddy::AbstractTrackItem*>>>;
using TrackEditorList = Array<std::tuple<QModelIndex, NotNull<NxA::RekordBuddy::AbstractTrackItem*>>>;

enum class EditorType { TrackRating, QColor, QImage, QLabel, QString, QDate, QDateTime, Duration, count, QStringList };

template <typename T>
    struct EditorFor; // instantiating this means you've missed setting up an editor for T

template <>
    struct EditorFor<QStringList>
    {
        using EditorWidget = TagWidget;
        static constexpr auto uiType = EditorType::QStringList;
        static constexpr auto getter = &EditorWidget::tags;
        static constexpr auto setter = &EditorWidget::setTags;
    };

template <>
    struct EditorFor<Common::TrackRating>
    {
        using EditorWidget = RatingEditor;
        static constexpr auto uiType = EditorType::TrackRating;
        static constexpr auto getter = &EditorWidget::rating;
        static constexpr auto setter = &EditorWidget::setRating;
    };

template <>
    struct EditorFor<QImage>
    {
        using EditorWidget = ImageBrowser;
        static constexpr auto uiType = EditorType::QImage;
        static constexpr auto getter = &EditorWidget::image;
        static constexpr auto setter = &EditorWidget::setImage;
    };

template <>
    struct EditorFor<QColor>
    {
        using EditorWidget = ColorEditor;
        static constexpr auto uiType = EditorType::QColor;
        static constexpr auto getter = &EditorWidget::color;
        static constexpr auto setter = &EditorWidget::setColor;
    };

template <>
    struct EditorFor<QString>
    {
        using EditorWidget = QLineEdit;
        static constexpr auto uiType = EditorType::QString;
        using getterT = decltype(&EditorWidget::text);
        using setterT = decltype(&EditorWidget::setText);
        inline static getterT getter = &EditorWidget::text;
        inline static setterT setter = &EditorWidget::setText;
    };

template <>
    struct EditorFor<count>
    {
        using EditorWidget = QSpinBox;
        static constexpr auto uiType = EditorType::count;
        using getterT = decltype(&EditorWidget::value);
        using setterT = decltype(&EditorWidget::setValue);
        inline static getterT getter = &EditorWidget::value;
        inline static setterT setter = &EditorWidget::setValue;
    };

template <>
    struct EditorFor<QDate>
    {
        using EditorWidget = QDateTimeEdit;
        static constexpr auto uiType = EditorType::QDate;
        using getterT = decltype(&EditorWidget::date);
        using setterT = decltype(&EditorWidget::setDate);
        inline static getterT getter = &EditorWidget::date;
        inline static setterT setter = &EditorWidget::setDate;
    };

template <>
    struct EditorFor<QDateTime>
    {
        using EditorWidget = QDateTimeEdit;
        static constexpr auto uiType = EditorType::QDateTime;
        using getterT = decltype(&EditorWidget::dateTime);
        using setterT = decltype(&EditorWidget::setDateTime);
        inline static getterT getter = &EditorWidget::dateTime;
        inline static setterT setter = &EditorWidget::setDateTime;
    };

template <>
    struct EditorFor<Duration>
    {
        using EditorWidget = DurationViewer;
        static constexpr auto uiType = EditorType::Duration;
        using getterT = decltype(&EditorWidget::duration);
        using setterT = decltype(&EditorWidget::setDuration);
        inline static getterT getter = &EditorWidget::duration;
        inline static setterT setter = &EditorWidget::setDuration;
    };

using BaseColumnValue = Variant<QString, QImage, Duration, QColor, QDate, QDateTime, Common::TrackRating, count, QStringList>;

template <typename T, typename B>
    using GetterT = T (B::*)() const;

template <typename T, typename B>
    using SetterT = void (B::*)(const T&);

template <typename T, typename B, typename Editor = EditorFor<T>>
    struct Column
    {
        Column() = delete;
        Column(const Column& other) = default;
        Column(Column&& other) = default;
        Column& operator=(const Column& other) = default;

        Column(TrackEditorList& withList, NotNull<const char*> withName, GetterT<T, B> withGet)
                : list{withList}, name{withName}, getter{withGet}
        {
        }

        Column(TrackEditorList& withList, NotNull<const char*> withName, GetterT<T, B> withGet, std::function<QString(T)>&& withVisualizer)
                : list{withList}, name{withName}, getter{withGet}, maybeVisualizer{std::move(withVisualizer)}
        {
        }

        Column(TrackEditorList& withList, NotNull<const char*> withName, GetterT<T, B> withGet, SetterT<T, B> withSet)
                : list{withList}, name{withName}, getter{withGet}, maybeSetter{withSet}
        {
        }

        EditorType uiType() const
        {
            if (!this->maybeVisualizer.isValid()) {
                return Editor::uiType;
            }
            return EditorType::QLabel;
        }

        boolean applyToTrackIfChangedInUI(NotNull<MultiValueEditor*> editor, NotNull<B*> item) const
        {
            if (this->maybeSetter.isValid()) {
                auto maybeNewValues = this->getFromUI(editor);
                if (maybeNewValues != this->getFromTrack(item)) {
                    this->setToTrack(item, maybeNewValues);
                    return true;
                }
            }

            return false;
        }

        Optional<QString> maybeVisualizedFromTrack(NotNull<B*> item) const
        {
            return this->maybeVisualizer.maybe([this](auto&& func) { return func(this->getFromTrack()); });
        }

        void setToTrack(NotNull<B*> item, T value) const
        {
            NXA_ASSERT_TRUE(this->maybeSetter.isValid());
            return (item.get()->*(*this->maybeSetter))(value);
        }

        T getFromTrack(NotNull<B*> item) const
        {
            return (item.get()->*(this->getter))();
        }

        void setToUI(NotNull<MultiValueEditor*> editor, T value) const;
        void setToUI(NotNull<MultiValueEditor*> editor, BaseColumnValue value) const;
        T getFromUI(NotNull<MultiValueEditor*> editor) const;

        TrackEditorList& trackList()
        {
            return this->list;
        }
        const TrackEditorList& trackList() const
        {
            return this->list;
        }
        NotNull<const char*> trackName()
        {
            return this->name;
        }

    private:
        TrackEditorList& list;
        NotNull<const char*> name;
        GetterT<T, B> getter;
        Optional<std::function<QString(T)>> maybeVisualizer;
        Optional<SetterT<T, B>> maybeSetter;
    };

using BaseColumn = Variant<Column<QString, AbstractTrackItem>, Column<QImage, AbstractTrackItem>, Column<QColor, AbstractTrackItem>, Column<QDate, AbstractTrackItem>,
                           Column<QDateTime, AbstractTrackItem>, Column<Duration, AbstractTrackItem>, Column<Common::TrackRating, AbstractTrackItem>,
                           Column<count, AbstractTrackItem>, Column<QStringList, AbstractTrackItem>>;

// -- Public Interface
class MultiValueEditor : public QWidget
{
    Q_OBJECT

public:
    MultiValueEditor(QWidget* parent = nullptr);
    ~MultiValueEditor() override;

    void associateColumn(BaseColumn column);

    void setShowingCountValuesWithEditable(count toValue, boolean);

    void loadFromTracks();
    boolean saveToTracksIfModified();

    NotNull<QWidget*> currentEditor();

    NotNull<Ui::MultiValueEditor*> ui;

    Q_ENUM(EditorType)
    Q_PROPERTY(EditorType type READ type CONSTANT)
    EditorType type() const;

private slots:
    void onRevert();
    void onEditMulti();

private:
    Optional<BaseColumn> columnToEdit;
    boolean overrideMulti{ false };
    boolean isMultiEditing{ false };
};

template <typename T, typename B, typename Editor>
    void Column<T, B, Editor>::setToUI(NotNull<MultiValueEditor*> editor, T value) const
    {
        if (maybeVisualizer.isValid()) {
            auto ptr = *(editor->currentEditor().maybeAs<QLabel*>());
            auto labelText = (*maybeVisualizer)(value);
            return ptr->setText(labelText);
        }
        auto ptr = *(editor->currentEditor().maybeAs<typename Editor::EditorWidget*>());
        return (ptr->*(Editor::setter))(value);
    }

template <typename T, typename B, typename Editor>
    void Column<T, B, Editor>::setToUI(NotNull<MultiValueEditor*> editor, BaseColumnValue value) const
    {
        NXA_ASSERT_TRUE(value.isType<T>());
        if (maybeVisualizer.isValid()) {
            auto labelText = (*maybeVisualizer)(value.get<T>());
            auto ptr = *(editor->currentEditor().maybeAs<QLabel*>());
            return ptr->setText(labelText);
        }
        auto ptr = *(editor->currentEditor().maybeAs<typename Editor::EditorWidget*>());
        return (ptr.get()->*(Editor::setter))(value.get<T>());
    }

template <typename T, typename B, typename Editor>
    T Column<T, B, Editor>::getFromUI(NotNull<MultiValueEditor*> editor) const
    {
        NXA_ASSERT_FALSE(maybeVisualizer.isValid());
        auto ptr = *(editor->currentEditor().maybeAs<typename Editor::EditorWidget*>());
        return (ptr.get()->*(Editor::getter))();
    }

} }
