/* 
 *  	Created on: 4.1.2013
 *      Author: Antti Kamppi
 * 		filename: addressspacegapitem.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEGAPITEM_H
#define ADDRESSSPACEGAPITEM_H

#include <IPXactWrapper/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>

class AddressSpaceGapItem : public AddressSpaceVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space containing the gaps.
	 * \param parent Pointer to the parent graph item.
	 *
	*/
	AddressSpaceGapItem(QSharedPointer<AddressSpace> addrSpace,
		QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~AddressSpaceGapItem();

	//! \brief Refresh the item.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item.
	*/
	virtual quint64 getOffset() const;

	/*! \brief Get the last address contained in the item.
	 *
	 * \return The last address.
	*/
	virtual quint64 getLastAddress() const;

	/*! \brief Set start address for the gap.
	 *
	 * \param address The address that limits the gap start.
	 * \param contains If true then the address is contained in the gap, 
	 * otherwise the gap starts from next address.
	 *
	*/
	void setStartAddress(quint64 address, bool contains = true);

	/*! \brief Set end address for the gap.
	 *
	 * \param address The address that limits the gap end.
	 * \param contains If true then the address is contained in the gap,
	 * otherwise the gap ends before the specified address.
	 *
	*/
	void setEndAddress(quint64 address, bool contains = true);

private:
	
	//! \brief No copying
	AddressSpaceGapItem(const AddressSpaceGapItem& other);

	//! \brief No assignment
	AddressSpaceGapItem& operator=(const AddressSpaceGapItem& other);

	//! \brief The start address of the gap.
	quint64 start_;

	//! \brief The end address of the gap.
	quint64 end_;
};

#endif // ADDRESSSPACEGAPITEM_H