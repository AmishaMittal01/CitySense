// ===================== UndoStack.h =====================
#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <stack>
#include <string>

// Generic undo action for simulation events
struct UndoAction {
    std::string type;      // e.g., "parking", "movement"
    std::string targetID;  // spotID, vehicleID, etc.

    // Previous state
    bool prevOccupied;
    int prevVehicleID;
};

class UndoStack {
private:
    std::stack<UndoAction> stk;

public:
    UndoStack() = default;

    void pushAction(const UndoAction &action);
    bool hasUndo() const;
    UndoAction popAction();
};

#endif
