/*
 * Game Develop Core
 * Copyright 2008-2014 Florian Rival (Florian.Rival@gmail.com). All rights reserved.
 * This project is released under the GNU Lesser General Public License.
 */
#if defined(GD_IDE_ONLY)
#ifndef GDCORE_EVENTSLIST_H
#define GDCORE_EVENTSLIST_H
#include <vector>
#include <boost/shared_ptr.hpp>
namespace gd { class Project; }
namespace gd { class BaseEvent; }
namespace gd { class SerializerElement; }
class TiXmlElement;

namespace gd
{

/**
 * \brief A list of events.
 *
 * \see BaseEvent
 * \ingroup Events
 */
class GD_CORE_API EventsList
{
public:
	EventsList();
    EventsList(const EventsList&);
    virtual ~EventsList() {};
    EventsList& operator=(const EventsList & rhs);

    /**
     * \brief Return a pointer to a copy of the event list.
     */
    EventsList * Clone() const { return new EventsList(*this); };

    /**
     * \brief Insert the specified event to the list
     * \note The event passed by parameter is copied.
     * \param event The event that must be copied and inserted into the list
     * \param position Insertion position. If the position is invalid, the object is inserted at the end of the objects list.
     * \return A reference to the event in the list
     */
    gd::BaseEvent & InsertEvent(const gd::BaseEvent & event, size_t position = (size_t)-1);

    /**
     * \brief Insert the specified event to the list.
     * \note The event passed by parameter is not copied.
     * \param event The smart pointer to the event that must be inserted into the list
     * \param position Insertion position. If the position is invalid, the object is inserted at the end of the objects list.
     */
    void InsertEvent(boost::shared_ptr<gd::BaseEvent> event, size_t position = (size_t)-1);

    /**
     * \brief Insert a new event to the list.
     * \note The event is created using the project current platform.
     * \param project The project the events list belongs to.
     * \param eventType The type of the event
     * \param position Insertion position. If the position is invalid, the object is inserted at the end of the objects list.
     */
    gd::BaseEvent & InsertNewEvent(gd::Project & project, const std::string & eventType, size_t position = (size_t)-1);

    /**
     * \brief Copy events from another list
     */
    void InsertEvents(const EventsList & otherEvents, size_t begin, size_t end, size_t position = (size_t)-1);

    /**
     * \brief Return the number of events.
     */
    size_t GetEventsCount() const { return events.size(); };

    /**
     * \brief Return the smart pointer to the event at position \a index in the events list.
     */
    boost::shared_ptr<BaseEvent> GetEventSmartPtr(size_t index) { return events[index]; };

    /**
     * \brief Return a reference to the event at position \a index in the events list.
     */
    gd::BaseEvent & GetEvent(size_t index) { return *events[index]; };

    /**
     * \brief Return a reference to the event at position \a index in the events list.
     */
    const gd::BaseEvent & GetEvent(size_t index) const { return *events[index]; };

    /**
     * \brief Remove the specified event.
     */
    void RemoveEvent(const gd::BaseEvent & event);

    /**
     * \brief Remove the event at the specified index in the list.
     */
    void RemoveEvent(size_t index);

    /**
     * \brief Return true if there isn't any event in the list
     */
    bool IsEmpty() const { return events.empty(); };

    /**
     * \brief Clear the list of events.
     */
    void Clear() { return events.clear(); };

    /** \name Utilities
     * Utility methods
     */
    ///@{
    /**
     * Return true if the specified event exists in the list.
     * \param event The event to searched for
     * \param recursive Set it to false to prevent sub events to be inspected.
     */
    bool Contains(const gd::BaseEvent & eventToSearch, bool recursive = true) const;
    ///@}

    /** \name std::vector API compatibility
     * These functions ensure that the class can be used just like a std::vector.
     */
    ///@{

    /**
     * \brief Alias for GetEventsCount()
     * \see EventsList::GetEventsCount.
     */
    size_t size() const { return GetEventsCount(); }

    /**
     * \brief Alias for GetEvent()
     * \see EventsList::GetEvent.
     */
    gd::BaseEvent & operator[](size_t index) { return GetEvent(index); };

    /**
     * \brief Alias for GetEvent()
     * \see EventsList::GetEvent.
     */
    const gd::BaseEvent & operator[](size_t index) const { return GetEvent(index); };
    ///@}

    /** \name Saving and loading
     * Members functions related to saving and loading the events list.
     */
    ///@{
    /**
     * \brief Save the objects to xml
     * \see EventsListSerialization
     */
    void SerializeTo(SerializerElement & element) const;

    /**
     * \brief Load the objects from xml
     * \see EventsListSerialization
     */
    void UnserializeFrom(gd::Project & project, const SerializerElement & element);
    ///@}

private:
	std::vector< boost::shared_ptr<BaseEvent> > events;

    /**
     * Initialize from another list of events, copying events. Used by copy-ctor and assign-op.
     * Don't forget to update me if members were changed !
     */
    void Init(const gd::EventsList & other);
};

}

#endif
#endif