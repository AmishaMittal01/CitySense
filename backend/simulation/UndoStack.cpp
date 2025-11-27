// ===================== UndoStack.cpp =====================
#include "UndoStack.h"
#include <stdexcept>

void UndoStack::pushAction(const UndoAction &action) {
    stk.push(action);
}

bool UndoStack::hasUndo() const {
    return !stk.empty();
}

UndoAction UndoStack::popAction() {
    if (stk.empty()) throw std::runtime_error("No undo actions available.");
    UndoAction action = stk.top();
    stk.pop();
    return action;
}
