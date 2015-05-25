//-----------------------------------------------------------------------------
// File: ViewConfigurer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2015
//
// Description:
// Configures the views in the component design of the selected component.
//-----------------------------------------------------------------------------

#include "ViewConfigurer.h"
#include "ViewConfigurerColumns.h"
#include "ViewConfigurerDelegate.h"

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <QSharedPointer>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::ViewConfigurer()
//-----------------------------------------------------------------------------
ViewConfigurer::ViewConfigurer(QSharedPointer<Component> selectedComponent, QSharedPointer<Design> selectedDesign,
    QSharedPointer<DesignConfiguration> selectedDesignConfiguration, LibraryHandler* libraryHandler,
    QString const& openViewName, QWidget* parent /* = 0 */):
QDialog(parent),
selectedDesignConfiguration_(selectedDesignConfiguration),
viewsTree_(new QTreeWidget(this)),
libraryHandler_(libraryHandler),
usedHierarchicalComponentVLNVS_(),
commitButton_ (new QPushButton(tr("Save"), this))
{
    setMinimumWidth(600);
    setMinimumHeight(400);

    QStringList treeHeaderLabels;
    treeHeaderLabels << "VLNV" << "Instance" << "View" << "ID";
    
    viewsTree_->setHeaderLabels(treeHeaderLabels);
    viewsTree_->setColumnCount(ViewConfigurerColumns::COLUMN_COUNT);
    viewsTree_->hideColumn(ViewConfigurerColumns::INSTANCE_ID);
    viewsTree_->setIndentation(15);

    viewsTree_->setItemDelegate(new ViewConfigurerDelegate(libraryHandler, this));

    QTreeWidgetItem* topComponentItem (new QTreeWidgetItem(viewsTree_));
    
    if (!selectedDesign && !selectedDesignConfiguration_)
    {
        topComponentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    }
    else
    {
        usedHierarchicalComponentVLNVS_.append(selectedComponent->getVlnv()->toString(":"));
    }

    topComponentItem->setText(ViewConfigurerColumns::ITEM_VLNV, selectedComponent->getVlnv()->toString(":"));
    topComponentItem->setText(ViewConfigurerColumns::INSTANCE_NAME, selectedComponent->getVlnv()->getName());

    if (openViewName.isEmpty())
    {
        topComponentItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, ViewConfigurerColumns::NOVIEWTEXT);

        QBrush foreGroundBrush (Qt::red);
        topComponentItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
    }
    else
    {
        topComponentItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, openViewName);
    }

    viewsTree_->addTopLevelItem(topComponentItem);

    if (selectedDesign)
    {
        createChildTreeWidgetItems(selectedDesign, selectedDesignConfiguration_, topComponentItem);

        modifyTreeWithExistingViewGroup();
    }

    viewsTree_->expandAll();

    viewsTree_->resizeColumnToContents(ViewConfigurerColumns::ITEM_VLNV);
    viewsTree_->resizeColumnToContents(ViewConfigurerColumns::INSTANCE_NAME);
    viewsTree_->resizeColumnToContents(ViewConfigurerColumns::INSTANCE_VIEW);

    setupLayout();

    connect(viewsTree_, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
        this, SLOT(onInstanceViewChanged(QTreeWidgetItem*, int)), Qt::UniqueConnection);

    setCommmitButtonEnabledStatus();
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::~ViewConfigurer()
//-----------------------------------------------------------------------------
ViewConfigurer::~ViewConfigurer()
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::setupLayout()
//-----------------------------------------------------------------------------
void ViewConfigurer::setupLayout()
{
    connect(commitButton_, SIGNAL(released()), this, SLOT(saveAndCloseConfigurer()), Qt::UniqueConnection);

    QPushButton* cancelButton (new QPushButton(tr("Cancel"), this));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()), Qt::UniqueConnection);

    QDialogButtonBox* buttonBox (new QDialogButtonBox(Qt::Horizontal));
    buttonBox->addButton(commitButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    setWindowTitle("Configuration of views");

    QVBoxLayout* mainLayout (new QVBoxLayout(this));
    mainLayout->addWidget(viewsTree_);
    mainLayout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::createChildTreeWidgetItem()
//-----------------------------------------------------------------------------
void ViewConfigurer::createChildTreeWidgetItems(QSharedPointer<Design> currentDesign,
    QSharedPointer<DesignConfiguration> currentDesignConfiguration, QTreeWidgetItem* parentItem)
{
    foreach (ComponentInstance currentInstance, currentDesign->getComponentInstances())
    {
        VLNV componentReference = currentInstance.getComponentRef();
        QSharedPointer<Component> component = libraryHandler_->getModel(componentReference).dynamicCast<Component>();

        QString instanceViewName = currentDesignConfiguration->getActiveView(currentInstance.getInstanceName());

        QTreeWidgetItem* instanceItem (new QTreeWidgetItem(parentItem));
        instanceItem->setText(ViewConfigurerColumns::ITEM_VLNV, componentReference.toString(":"));
        instanceItem->setText(ViewConfigurerColumns::INSTANCE_NAME, currentInstance.getInstanceName());

        if (instanceViewName.isEmpty())
        {
            instanceViewName = ViewConfigurerColumns::NOVIEWTEXT;

            QBrush foreGroundBrush(Qt::red);
            instanceItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
        }

        instanceItem->setText(ViewConfigurerColumns::INSTANCE_VIEW, instanceViewName);
        instanceItem->setText(ViewConfigurerColumns::INSTANCE_ID, currentInstance.getUuid());

        instanceItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

        checkInstanceDesign(component, instanceViewName, instanceItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::checkInstanceDesign()
//-----------------------------------------------------------------------------
void ViewConfigurer::checkInstanceDesign(QSharedPointer<Component> component, QString const& viewName,
    QTreeWidgetItem* currentTreeItem)
{
    if (!viewName.isEmpty())
    {
        VLNV viewDesignVLNV = component->getHierRef(viewName);

        if (viewDesignVLNV.isValid())
        {
            QSharedPointer<DesignConfiguration> viewDesignConfiguration = libraryHandler_->getModel(
                viewDesignVLNV).dynamicCast<DesignConfiguration>();

            if (viewDesignConfiguration)
            {
                VLNV designReference = viewDesignConfiguration->getDesignRef();

                QSharedPointer<Design> viewDesign = libraryHandler_->getModel(designReference).
                    dynamicCast<Design>();

                if (viewDesign && !usedHierarchicalComponentVLNVS_.contains(
                    currentTreeItem->text(ViewConfigurerColumns::ITEM_VLNV)))
                {
                    usedHierarchicalComponentVLNVS_.append(currentTreeItem->text(
                        ViewConfigurerColumns::ITEM_VLNV));

                    createChildTreeWidgetItems(viewDesign, viewDesignConfiguration, currentTreeItem);

                    if (usedHierarchicalComponentVLNVS_.size() > 1)
                    {
                        usedHierarchicalComponentVLNVS_.removeAll(currentTreeItem->text(
                            ViewConfigurerColumns::ITEM_VLNV));
                    }
                }
                else
                {
                    currentTreeItem->setText(ViewConfigurerColumns::INSTANCE_VIEW,
                        ViewConfigurerColumns::CYCLICCOMPONENTTEXT);

                    QBrush foreGroundBrush (Qt::red);
                    currentTreeItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
                    currentTreeItem->setForeground(ViewConfigurerColumns::ITEM_VLNV, foreGroundBrush);
                    currentTreeItem->setForeground(ViewConfigurerColumns::INSTANCE_NAME, foreGroundBrush);

                    currentTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                }
            }
        } 
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::createTreeWithExistingItems()
//-----------------------------------------------------------------------------
void ViewConfigurer::modifyTreeWithExistingViewGroup()
{
    if(selectedDesignConfiguration_)
    {
        QList<QSharedPointer<VendorExtension> > designConfigVendors =
            selectedDesignConfiguration_->getVendorExtensions();

        foreach (QSharedPointer<VendorExtension> extension, designConfigVendors)
        {
            if (extension->type() == "kactus2:viewConfigurations")
            {
                QSharedPointer<Kactus2Group> viewGroup = extension.dynamicCast<Kactus2Group>();

                for (int treeTopIndex = 0; treeTopIndex < viewsTree_->topLevelItemCount(); ++treeTopIndex)
                {
                    parseExistingViewGroup(viewsTree_->topLevelItem(treeTopIndex), viewGroup);
                }

                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::parseExistingViewGroup()
//-----------------------------------------------------------------------------
void ViewConfigurer::parseExistingViewGroup(QTreeWidgetItem* currentTreeItem,
    QSharedPointer<Kactus2Group> viewGroup)
{
    foreach (QSharedPointer<VendorExtension> extension, viewGroup->getByType("kactus2:instanceView"))
    {
        QSharedPointer<Kactus2Placeholder> viewConfiguration = extension.dynamicCast<Kactus2Placeholder>();

        if (viewConfiguration->getAttributeValue("id") == currentTreeItem->text(ViewConfigurerColumns::INSTANCE_ID))
        {
            currentTreeItem->setText(ViewConfigurerColumns::INSTANCE_VIEW,
                viewConfiguration->getAttributeValue("viewName"));

            onInstanceViewChanged(currentTreeItem, ViewConfigurerColumns::INSTANCE_VIEW);
        }
    }

    for (int treeChildItem = 0; treeChildItem < currentTreeItem->childCount(); ++treeChildItem)
    {
        parseExistingViewGroup(currentTreeItem->child(treeChildItem), viewGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::onInstanceViewChanged()
//-----------------------------------------------------------------------------
void ViewConfigurer::onInstanceViewChanged(QTreeWidgetItem* changedItem, int column)
{
    if (changedItem && column == ViewConfigurerColumns::INSTANCE_VIEW)
    {
        changedItem->takeChildren();

        QString itemVLNV = changedItem->text(ViewConfigurerColumns::ITEM_VLNV);
        VLNV componentVLNV (VLNV::COMPONENT, itemVLNV, ":");

        QSharedPointer<Component> component = libraryHandler_->getModel(componentVLNV).dynamicCast<Component>();

        QString viewName = changedItem->text(ViewConfigurerColumns::INSTANCE_VIEW);

        if (isChangedItemTopItem(changedItem))
        {
            changedTopItemChangesDesignConfiguration(component, viewName);
        }

        viewsTree_->blockSignals(true);

        checkInstanceDesign(component, viewName, changedItem);

        if (changedItem->text(ViewConfigurerColumns::INSTANCE_VIEW) == ViewConfigurerColumns::NOVIEWTEXT ||
            !component->getViewNames().contains(changedItem->text(ViewConfigurerColumns::INSTANCE_VIEW)))
        {
            QBrush foreGroundBrush (Qt::red);
            changedItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
        }
        else
        {
            QBrush foreGroundBrush = changedItem->foreground(ViewConfigurerColumns::ITEM_VLNV);
            changedItem->setForeground(ViewConfigurerColumns::INSTANCE_VIEW, foreGroundBrush);
        }

        if (isChangedItemTopItem(changedItem))
        {
            modifyTreeWithExistingViewGroup();
        }

        viewsTree_->blockSignals(false);

        setCommmitButtonEnabledStatus();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::isChangedItemTopItem()
//-----------------------------------------------------------------------------
bool ViewConfigurer::isChangedItemTopItem(QTreeWidgetItem* changedTreeItem)
{
    for (int treeTopIndex = 0; treeTopIndex < viewsTree_->topLevelItemCount(); ++treeTopIndex)
    {
        if (changedTreeItem == viewsTree_->topLevelItem(treeTopIndex))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::changedTopItemChangesDesignConfiguration()
//-----------------------------------------------------------------------------
void ViewConfigurer::changedTopItemChangesDesignConfiguration(QSharedPointer<Component> component,
    QString const& viewName)
{
    selectedDesignConfiguration_.clear();

    VLNV viewDesignVLNV = component->getHierRef(viewName);
    if (viewDesignVLNV.isValid())
    {
        QSharedPointer<DesignConfiguration> viewDesignConfiguration =
            libraryHandler_->getModel(viewDesignVLNV).dynamicCast<DesignConfiguration>();

        if (viewDesignConfiguration)
        {
            selectedDesignConfiguration_ = viewDesignConfiguration;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::setCommmitButtonEnabledStatus()
//-----------------------------------------------------------------------------
void ViewConfigurer::setCommmitButtonEnabledStatus()
{
    bool commitButtonStatus = true;

    for (int i = 0; i < viewsTree_->topLevelItemCount(); ++i)
    {
        if (treeItemContainsEmptyOrCyclicalViews(viewsTree_->topLevelItem(i)))
        {
            commitButtonStatus = false;
            break;
        }
    }

    commitButton_->setEnabled(commitButtonStatus);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::treeContainsEmptyOrCyclicalViews()
//-----------------------------------------------------------------------------
bool ViewConfigurer::treeItemContainsEmptyOrCyclicalViews(QTreeWidgetItem* treeItem)
{
    if (treeItem->text(ViewConfigurerColumns::INSTANCE_VIEW) == ViewConfigurerColumns::NOVIEWTEXT ||
        treeItem->text(ViewConfigurerColumns::INSTANCE_VIEW) == ViewConfigurerColumns::CYCLICCOMPONENTTEXT)
    {
        return true;
    }

    for (int treeItemIndex = 0; treeItemIndex < treeItem->childCount(); ++treeItemIndex)
    {
        if (treeItemContainsEmptyOrCyclicalViews(treeItem->child(treeItemIndex)))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::saveAndCloseConfigurer()
//-----------------------------------------------------------------------------
void ViewConfigurer::saveAndCloseConfigurer()
{
    if(selectedDesignConfiguration_)
    {
        QList<QSharedPointer<VendorExtension> > designConfigVendors =
            selectedDesignConfiguration_->getVendorExtensions();

        foreach (QSharedPointer<VendorExtension> extension, designConfigVendors)
        {
            if (extension->type() == "kactus2:viewConfigurations")
            {
                designConfigVendors.removeAll(extension);
                break;
            }
        }

        QSharedPointer<Kactus2Group> viewGroup(new Kactus2Group("kactus2:viewConfigurations"));
        bool viewGroupIsEmpty = true;

        for (int treeIndex = 0; treeIndex < viewsTree_->topLevelItemCount(); ++treeIndex)
        {
            QTreeWidgetItem* topItem = viewsTree_->topLevelItem(treeIndex);

            for (int topItemChildIndex = 0; topItemChildIndex < topItem->childCount(); ++topItemChildIndex)
            {
                parseChildTreeItem(topItem->child(topItemChildIndex), viewGroup);
                viewGroupIsEmpty = false;
            }
        }

        if (!viewGroupIsEmpty)
        {
            designConfigVendors.append(viewGroup);
            selectedDesignConfiguration_->setVendorExtensions(designConfigVendors);
        }

        libraryHandler_->writeModelToFile(selectedDesignConfiguration_);
    }

    accept();
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurer::parseChildTreeItem()
//-----------------------------------------------------------------------------
void ViewConfigurer::parseChildTreeItem(QTreeWidgetItem* treeItem, QSharedPointer<Kactus2Group> viewGroup)
{
    QString treeItemID = treeItem->text(ViewConfigurerColumns::INSTANCE_ID);
    QString treeItemView = treeItem->text(ViewConfigurerColumns::INSTANCE_VIEW);
    
    if (treeItemView != ViewConfigurerColumns::NOVIEWTEXT &&
        treeItemView != ViewConfigurerColumns::CYCLICCOMPONENTTEXT)
    {
        QSharedPointer<Kactus2Placeholder> treeItemExtension(new Kactus2Placeholder("kactus2:instanceView"));
        treeItemExtension->setAttribute("id", treeItemID);
        treeItemExtension->setAttribute("viewName", treeItemView);
        viewGroup->addToGroup(treeItemExtension);

        for (int itemChildIndex = 0; itemChildIndex < treeItem->childCount(); ++itemChildIndex)
        {
            parseChildTreeItem(treeItem->child(itemChildIndex), viewGroup);
        }
    }
}