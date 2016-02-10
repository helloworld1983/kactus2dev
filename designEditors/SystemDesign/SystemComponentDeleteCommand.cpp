//-----------------------------------------------------------------------------
// File: SystemComponentDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2016
//
// Description:
// Delete command for component instances in system designs.
//-----------------------------------------------------------------------------

#include "SystemComponentDeleteCommand.h"

#include "SystemDeleteCommands.h"
#include "SWPortItem.h"

#include <common/graphicsItems/ComponentItem.h>

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/kactusExtensions/SWInstance.h>

//-----------------------------------------------------------------------------
// Function: SystemComponentDeleteCommand::SystemComponentDeleteCommand()
//-----------------------------------------------------------------------------
SystemComponentDeleteCommand::SystemComponentDeleteCommand(ComponentItem* item,
    QSharedPointer<Design> containingDesign, QUndoCommand* parent /* = 0 */):
QObject(),
QUndoCommand(parent),
item_(item),
stack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem())),
scene_(item->scene()),
del_(true),
firstRun_(true),
containingDesign_(containingDesign)
{

}

//-----------------------------------------------------------------------------
// Function: SystemComponentDeleteCommand::~SystemComponentDeleteCommand()
//-----------------------------------------------------------------------------
SystemComponentDeleteCommand::~SystemComponentDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentDeleteCommand::undo()
//-----------------------------------------------------------------------------
void SystemComponentDeleteCommand::undo()
{
    // Add the item back to the scene.
    stack_->addItem(item_);
    del_ = false;

    QSharedPointer<SWInstance> swInstance = item_->getComponentInstance().dynamicCast<SWInstance>();
    if (swInstance)
    {
        QList<QSharedPointer<SWInstance> > containedSwInstances = containingDesign_->getSWInstances();
        containedSwInstances.append(swInstance);
        containingDesign_->setSWInstances(containedSwInstances);
    }
    else
    {
        containingDesign_->getComponentInstances()->append(item_->getComponentInstance());
    }

    emit componentInstantiated(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentDeleteCommand::redo()
//-----------------------------------------------------------------------------
void SystemComponentDeleteCommand::redo()
{
    if (firstRun_)
    {
        // Create child commands for removing connections.
        QList<GraphicsConnection*> connections;

        foreach (QGraphicsItem* childItem, item_->childItems())
        {
            if (childItem->type() != SWPortItem::Type)
            {
                continue;
            }

            SWPortItem* endpoint = static_cast<SWPortItem*>(childItem);

            foreach (GraphicsConnection* conn, endpoint->getConnections())
            {
                if (!connections.contains(conn))
                {
                    new SWConnectionDeleteCommand(conn, this);
                    connections.append(conn);
                }
            }

            if (endpoint->getOffPageConnector() != 0)
            {
                foreach (GraphicsConnection* conn, endpoint->getOffPageConnector()->getConnections())
                {
                    if (!connections.contains(conn))
                    {
                        new SWConnectionDeleteCommand(conn, this);
                        connections.append(conn);
                    }
                }
            }
        }

        firstRun_ = false;
    }

    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    stack_->removeItem(item_);
    scene_->removeItem(item_);
    del_ = true;

    QSharedPointer<SWInstance> swInstance = item_->getComponentInstance().dynamicCast<SWInstance>();
    if (swInstance)
    {
        QList<QSharedPointer<SWInstance> > containedSwInstances = containingDesign_->getSWInstances();
        containedSwInstances.removeAll(swInstance);
        containingDesign_->setSWInstances(containedSwInstances);
    }
    else
    {
        containingDesign_->getComponentInstances()->removeAll(item_->getComponentInstance());
    }


    emit componentInstanceRemoved(item_);
}
