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

#include <Base/Types.hpp>
#include <Base/NotNull.hpp>
#include <Base/Platform.hpp>
#include <Widgets/SidebarView.hpp>
#include <UiModel/SidebarModel.hpp>
#include <qevent.h>

#include "Widgets/moc_SidebarView.cpp"

using namespace NxA;
using namespace RekordBuddy;

// -- Constructors & Destructors

SidebarView::SidebarView(QWidget* parent) : QTreeView{ parent }
{
    this->setSelectionMode(SelectionMode::ExtendedSelection);
    this->setDragDropMode(QTreeView::DragDrop);
    this->setDropIndicatorShown(true);
}

// -- Instance Methods

void SidebarView::keyPressEvent(QKeyEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
#if defined(NXA_PLATFORM_WINDOWS)
        if (keyEvent->key() == Qt::Key_Delete) {
#elif defined(NXA_PLATFORM_MACOS)
        if (keyEvent->key() == Qt::Key_Backspace) {
#else
        #error Unsupported platform.
#endif

            auto selectedIndices = this->selectedIndexes();
            NotNull<SidebarModel*> model = dynamic_cast<SidebarModel*>(this->model());
            if (model->requestDeletionOfItemsAtIndices(selectedIndices) &&
                model->displayedItemIsnItemsOrChildOfItemsAtIndices(selectedIndices)) {
                QMetaObject::invokeMethod(this->parent(), "clearSidebarSelection", Qt::QueuedConnection);
            }
        }

        // -- Handled
        return;
    }

    // -- Unhandled inputs go to default handling
    return QTreeView::keyPressEvent(event);
}

boolean SidebarView::selectionContains(const QModelIndex& index) const
{
    return this->selectedIndexes().contains(index);
}

void SidebarView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    auto model = dynamic_cast<SidebarModel*>(this->model());

    // -- The code below is going to potentially de-select stuff (and only de-select) so we don't want to be caught in a loop.
    if (selected.length() && this->selectedIndexes().length()) {
        SidebarItem::TypeOfThingsSelectable currentTypesSelected;

        // -- We can only select one type of items right now, so we only need to check the first item.
        auto typeOfFirstItemCurrentlySelected = model->itemFromIndex(this->selectedIndexes().first())->typeOfSelectionForThis();
        currentTypesSelected.set(typeOfFirstItemCurrentlySelected);

        bool needToClearOldSelection = false;
        if (typeOfFirstItemCurrentlySelected != SidebarItem::TypeOfSelection::Crate) {
            needToClearOldSelection = true;
        }

        MutableSet<QModelIndex> newIndicesSelected;
        for (auto&& index : selected.indexes()) {
            newIndicesSelected.add(index);

            auto item = model->itemFromIndex(index);
            if (typeOfFirstItemCurrentlySelected != item->typeOfSelectionForThis()) {
                needToClearOldSelection = true;
            }
        }

        if (needToClearOldSelection) {
            for (auto&& index : this->selectedIndexes()) {
                if (!newIndicesSelected.contains(index)) {
                    this->selectionModel()->select(index, QItemSelectionModel::Deselect);
                }
            }
        }
    }
}
