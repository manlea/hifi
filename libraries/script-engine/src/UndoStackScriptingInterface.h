//
//  UndoStackScriptingInterface.h
//  libraries/script-engine/src
//
//  Created by Ryan Huffman on 10/22/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_UndoStackScriptingInterface_h
#define hifi_UndoStackScriptingInterface_h

#include <QUndoCommand>
#include <QUndoStack>
#include <QScriptValue>

class UndoStackScriptingInterface : public QObject {
    Q_OBJECT
public:
    UndoStackScriptingInterface(QUndoStack* undoStack);

public slots:
    void pushCommand(QScriptValue undoFunction, QScriptValue undoData, QScriptValue redoFunction, QScriptValue redoData);

private:
    QUndoStack* _undoStack;
};

class ScriptUndoCommand : public QObject, public QUndoCommand {
    Q_OBJECT
public:
    ScriptUndoCommand(QScriptValue undoFunction, QScriptValue undoData, QScriptValue redoFunction, QScriptValue redoData);

    virtual void undo();
    virtual void redo();
    virtual bool mergeWith(const QUndoCommand* command) { return false; }
    virtual int id() const { return -1; }

public slots:
    void doUndo();
    void doRedo();

private:
    QScriptValue _undoFunction;
    QScriptValue _undoData;
    QScriptValue _redoFunction;
    QScriptValue _redoData;
};

#endif // hifi_UndoStackScriptingInterface_h
