/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorrootitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorrootitem.h"

#include "componenteditorgeneralitem.h"
#include "componenteditorfilesetsitem.h"
#include "componenteditormodelparamsitem.h"
#include "componenteditorparametersitem.h"
#include "componenteditormemmapsitem.h"
#include "componenteditoraddrspacesitem.h"
#include "componenteditorviewsitem.h"
#include "componenteditorportsitem.h"
#include "componenteditorbusinterfacesitem.h"
#include "componenteditorchannelsitem.h"
#include "componenteditorcpusitem.h"
#include "componenteditorotherclocksitem.h"
#include "componenteditorcominterfacesitem.h"
#include "componenteditorswviewsitem.h"
#include "componenteditorapiinterfacesitem.h"
#include "componenteditorswpropertiesitem.h"
#include "ComponentEditorSystemViewsItem.h"
#include "componenteditortreemodel.h"

ComponentEditorRootItem::ComponentEditorRootItem(LibraryInterface* libHandler,
                                                 PluginManager& pluginMgr,
												 QSharedPointer<Component> component,
												 ComponentEditorTreeModel* parent,
                                                 QWidget* parentWnd):
ComponentEditorItem(libHandler, component, parent) {

	childItems_.append(QSharedPointer<ComponentEditorGeneralItem>(
		new ComponentEditorGeneralItem(parent, libHandler, component, this)));

	childItems_.append(QSharedPointer<ComponentEditorFileSetsItem>(
		new ComponentEditorFileSetsItem(parent, libHandler, pluginMgr, component, this)));

	if (component->getComponentImplementation() == KactusAttribute::KTS_HW)
	{
		childItems_.append(QSharedPointer<ComponentEditorModelParamsItem>(
			new ComponentEditorModelParamsItem(parent, libHandler, component, this)));
	}

	childItems_.append(QSharedPointer<ComponentEditorParametersItem>(
		new ComponentEditorParametersItem(parent, libHandler, component, this)));

	if (component->getComponentImplementation() == KactusAttribute::KTS_HW) {
		childItems_.append(QSharedPointer<ComponentEditorMemMapsItem>(
			new ComponentEditorMemMapsItem(parent, libHandler, component, this)));
	}

	if (component->getComponentImplementation() == KactusAttribute::KTS_HW)
	{
		childItems_.append(QSharedPointer<ComponentEditorAddrSpacesItem>(
			new ComponentEditorAddrSpacesItem(parent, libHandler, component, this)));

		childItems_.append(QSharedPointer<ComponentEditorViewsItem>(
			new ComponentEditorViewsItem(parent, libHandler, component, this)));
	}

	childItems_.append(QSharedPointer<ComponentEditorSWViewsItem>(
		new ComponentEditorSWViewsItem(parent, libHandler, component, this)));

	if (component->getComponentImplementation() != KactusAttribute::KTS_SW)
	{
		childItems_.append(QSharedPointer<ComponentEditorSystemViewsItem>(
			new ComponentEditorSystemViewsItem(parent, libHandler, component, this)));
	}

	if (component->getComponentImplementation() == KactusAttribute::KTS_HW)
	{
		childItems_.append(QSharedPointer<ComponentEditorPortsItem>(
			new ComponentEditorPortsItem(parent, libHandler, component, this)));
            connect(childItems_.back().data(), SIGNAL(createInterface()), 
                    this, SLOT(addInterface()), Qt::UniqueConnection);

		childItems_.append(QSharedPointer<ComponentEditorBusInterfacesItem>(
			new ComponentEditorBusInterfacesItem(parent, libHandler, component, this, parentWnd)));

		childItems_.append(QSharedPointer<ComponentEditorChannelsItem>(
			new ComponentEditorChannelsItem(parent, libHandler, component, this)));

		childItems_.append(QSharedPointer<ComponentEditorCpusItem>(
			new ComponentEditorCpusItem(parent, libHandler, component, this)));

		childItems_.append(QSharedPointer<ComponentEditorOtherClocksItem>(
			new ComponentEditorOtherClocksItem(parent, libHandler, component, this)));
	}

	childItems_.append(QSharedPointer<ComponentEditorComInterfacesItem>(
		new ComponentEditorComInterfacesItem(parent, libHandler, component, this)));

	if (component->getComponentImplementation() == KactusAttribute::KTS_SW)
	{
		childItems_.append(QSharedPointer<ComponentEditorAPIInterfacesItem>(
			new ComponentEditorAPIInterfacesItem(parent, libHandler, component, this)));
	}

	childItems_.append(QSharedPointer<ComponentEditorSWPropertiesItem>(
		new ComponentEditorSWPropertiesItem(parent, libHandler, component, this)));
}

ComponentEditorRootItem::~ComponentEditorRootItem() {

}

QString ComponentEditorRootItem::text() const {
	return tr("root");
}

bool ComponentEditorRootItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorRootItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorRootItem::editor() const {
	return NULL;
}

QString ComponentEditorRootItem::getTooltip() const {
	return tr("The root item of the navigation tree");
}

QSharedPointer<ComponentEditorItem> ComponentEditorRootItem::getBusInterfaceItem( const QString& interfaceName ) const {

	// check all children
	foreach (const QSharedPointer<ComponentEditorItem> item, childItems_) {
		const QSharedPointer<ComponentEditorBusInterfacesItem> busIfsItem =
			item.dynamicCast<ComponentEditorBusInterfacesItem>();

		// if the child was for bus interfaces summary item
		if (busIfsItem) {
			return busIfsItem->getBusInterfaceItem(interfaceName);
		}
	}

	// if there was no item for bus interfaces summary
	return QSharedPointer<ComponentEditorItem>();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::addInterface()
//-----------------------------------------------------------------------------
void ComponentEditorRootItem::addInterface()
{
    foreach (const QSharedPointer<ComponentEditorItem> item, childItems_) {
        const QSharedPointer<ComponentEditorBusInterfacesItem> busIfsItem =
            item.dynamicCast<ComponentEditorBusInterfacesItem>();

        // if the child was for bus interfaces summary item.
        if (busIfsItem) {            
            int childIndex = busIfsItem->children().count();
            model_->addItem(busIfsItem.data(), childIndex);
            busIfsItem->refreshEditor();
            busIfsItem->child(childIndex)->setHighlight(true);
            return;
        }
    }
}
