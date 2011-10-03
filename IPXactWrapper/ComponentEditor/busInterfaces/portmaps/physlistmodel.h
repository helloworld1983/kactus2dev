/* 
 *
 *  Created on: 11.5.2011
 *      Author: Antti Kamppi
 * 		filename: physlistmodel.h
 */

#ifndef PHYSLISTMODEL_H
#define PHYSLISTMODEL_H

#include "portlistmodel.h"

#include <QSharedPointer>

class Component;
class PortMapsModel;

/*! \brief Model to display the physical ports of a component.
 *
 */
class PhysListModel : public PortListModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that's ports are displayed.
	 * \param portMapsModel Pointer to the port maps model to ask if connection is possible.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	PhysListModel(QSharedPointer<Component> component, 
		PortMapsModel* portMapsModel,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~PhysListModel();

	/*! \brief Refresh the list of physical ports.
	 *
	*/
	virtual void refresh();

public slots:

	//! \brief Create a mapping with dropped port and the drop target port.
	//void onPortDropped(const QString& droppedPort, const QModelIndex& dropIndex);

	//! \brief Create a mapping with ports and the drop index port.
	//void onPortsDropped(const QStringList& ports, const QModelIndex& dropIndex);

	//! \brief Create a mapping with ports and the indexed ports.
	//void onPortsDropped(const QStringList& ports, const QModelIndexList& indexes);

private:

	//! \brief No copying
	PhysListModel(const PhysListModel& other);

	//! No assignment
	PhysListModel& operator=(const PhysListModel& other);
	
	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // PHYSLISTMODEL_H
