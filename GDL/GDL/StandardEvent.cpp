/** \file
 *  Game Develop
 *  2008-2011 Florian Rival (Florian.Rival@gmail.com)
 */

#include "StandardEvent.h"
#include "GDL/tinyxml.h"
#include "GDL/OpenSaveGame.h"
#include "GDL/CommonTools.h"
#include "GDL/EventsCodeGenerator.h"
#include "GDL/EventsCodeGenerationContext.h"

#if defined(GD_IDE_ONLY)
#include "EventsRenderingHelper.h"
#endif

StandardEvent::StandardEvent() :
BaseEvent()
{
}

std::string StandardEvent::GenerateEventCode(const Game & game, const Scene & scene, EventsCodeGenerationContext & context)
{
    std::string outputCode;

    outputCode += EventsCodeGenerator::GenerateConditionsListCode(game, scene, conditions, context);

    std::string ifPredicat = "true";
    for (unsigned int i = 0;i<conditions.size();++i)
        ifPredicat += " && condition"+ToString(i)+"IsTrue";

    outputCode += "if (" +ifPredicat+ ")\n";
    outputCode += "{\n";
    outputCode += EventsCodeGenerator::GenerateActionsListCode(game, scene, actions, context);
    if ( !events.empty() ) //Sub events
    {
        outputCode += "\n{\n";
        outputCode += EventsCodeGenerator::GenerateEventsListCode(game, scene, events, context);
        outputCode += "}\n";
    }

    outputCode += "}\n";

    return outputCode;
}

vector < vector<Instruction>* > StandardEvent::GetAllConditionsVectors()
{
    vector < vector<Instruction>* > allConditions;
    allConditions.push_back(&conditions);

    return allConditions;
}

vector < vector<Instruction>* > StandardEvent::GetAllActionsVectors()
{
    vector < vector<Instruction>* > allActions;
    allActions.push_back(&actions);

    return allActions;
}

#if defined(GD_IDE_ONLY)
void StandardEvent::SaveToXml(TiXmlElement * eventElem) const
{
    //Les conditions
    TiXmlElement * conditionsElem = new TiXmlElement( "Conditions" );
    eventElem->LinkEndChild( conditionsElem );
    OpenSaveGame::SaveConditions(conditions, conditionsElem);

    //Les actions
    TiXmlElement * actionsElem = new TiXmlElement( "Actions" );
    eventElem->LinkEndChild( actionsElem );
    OpenSaveGame::SaveActions(actions, actionsElem);

    //Sous �v�nements
    if ( !GetSubEvents().empty() )
    {
        TiXmlElement * subeventsElem;
        subeventsElem = new TiXmlElement( "Events" );
        eventElem->LinkEndChild( subeventsElem );

        OpenSaveGame::SaveEvents(events, subeventsElem);
    }
}
#endif

void StandardEvent::LoadFromXml(const TiXmlElement * eventElem)
{
    //Conditions
    if ( eventElem->FirstChildElement( "Conditions" ) != NULL )
        OpenSaveGame::OpenConditions(conditions, eventElem->FirstChildElement( "Conditions" ));
    else
        cout << "Aucune informations sur les conditions d'un �v�nement";

    //Actions
    if ( eventElem->FirstChildElement( "Actions" ) != NULL )
        OpenSaveGame::OpenActions(actions, eventElem->FirstChildElement( "Actions" ));
    else
        cout << "Aucune informations sur les actions d'un �v�nement";

    //Subevents
    if ( eventElem->FirstChildElement( "Events" ) != NULL )
        OpenSaveGame::OpenEvents(events, eventElem->FirstChildElement( "Events" ));
}

#if defined(GD_IDE_ONLY)
/**
 * Render the event in the bitmap
 */
void StandardEvent::Render(wxDC & dc, int x, int y, unsigned int width) const
{
    EventsRenderingHelper * renderingHelper = EventsRenderingHelper::GetInstance();

    //Draw event rectangle
    wxRect rect(x, y, width, GetRenderedHeight(width));
    renderingHelper->DrawNiceRectangle(dc, rect);

    renderingHelper->DrawConditionsList(conditions, dc, x+renderingHelper->instructionsListBorder, y+renderingHelper->instructionsListBorder, renderingHelper->GetConditionsColumnWidth()-renderingHelper->instructionsListBorder*2, IsDisabled());
    renderingHelper->DrawActionsList(actions, dc, x+renderingHelper->GetConditionsColumnWidth()+renderingHelper->instructionsListBorder, y+renderingHelper->instructionsListBorder, width-renderingHelper->GetConditionsColumnWidth()-renderingHelper->instructionsListBorder*2, IsDisabled());
}

unsigned int StandardEvent::GetRenderedHeight(unsigned int width) const
{
    if ( eventHeightNeedUpdate )
    {
        EventsRenderingHelper * renderingHelper = EventsRenderingHelper::GetInstance();

        //Get maximum height needed
        int conditionsHeight = renderingHelper->GetRenderedConditionsListHeight(conditions, renderingHelper->GetConditionsColumnWidth());
        int actionsHeight = renderingHelper->GetRenderedActionsListHeight(actions, width-renderingHelper->GetConditionsColumnWidth());

        renderedHeight = (conditionsHeight > actionsHeight ? conditionsHeight : actionsHeight)+renderingHelper->instructionsListBorder*2;
        eventHeightNeedUpdate = false;
    }

    return renderedHeight;
}

void StandardEvent::OnSingleClick(int x, int y, vector < boost::tuple< vector < BaseEventSPtr > *, unsigned int, vector < Instruction > *, unsigned int > > & eventsSelected,
                         bool & conditionsSelected, bool & instructionsSelected)
{
    EventsRenderingHelper * renderingHelper = EventsRenderingHelper::GetInstance();

    if ( x <= renderingHelper->GetConditionsColumnWidth())
    {
        conditionsSelected = true;

        vector < Instruction > * conditionsListSelected = NULL;
        unsigned int conditionIdInList = 0;

        bool found = renderingHelper->GetConditionAt(conditions, x-renderingHelper->instructionsListBorder, y-renderingHelper->instructionsListBorder, conditionsListSelected, conditionIdInList);

        if ( found )
        {
            //Update event and conditions selection information
            if ( conditionIdInList < conditionsListSelected->size() ) (*conditionsListSelected)[conditionIdInList].selected = true;

            //Update editor selection information
            instructionsSelected = true;
            boost::tuples::get<2>(eventsSelected.back()) = conditionsListSelected;
            boost::tuples::get<3>(eventsSelected.back()) = conditionIdInList;

            return;
        }
        else if ( y <= 18 )
        {
            //Update selection information
            instructionsSelected = true;
            boost::tuples::get<2>(eventsSelected.back()) = &conditions;
            boost::tuples::get<3>(eventsSelected.back()) = 0;

            return;
        }
    }
    else
    {
        conditionsSelected = false;

        vector < Instruction > * actionsListSelected = NULL;
        unsigned int actionIdInList = 0;

        bool found = renderingHelper->GetActionAt(actions, x-renderingHelper->instructionsListBorder, y-renderingHelper->instructionsListBorder, actionsListSelected, actionIdInList);

        if ( found )
        {
            //Update event and action selection information
            if ( actionIdInList < actionsListSelected->size() ) (*actionsListSelected)[actionIdInList].selected = true;

            //Update selection information
            instructionsSelected = true;
            boost::tuples::get<2>(eventsSelected.back()) = actionsListSelected;
            boost::tuples::get<3>(eventsSelected.back()) = actionIdInList;
        }
        else
        {
            //Update selection information
            instructionsSelected = true;
            boost::tuples::get<2>(eventsSelected.back()) = &actions;
            boost::tuples::get<3>(eventsSelected.back()) = 0;
        }
    }
}
#endif

/**
 * Initialize from another StandardEvent.
 * Used by copy ctor and assignement operator
 */
void StandardEvent::Init(const StandardEvent & event)
{
    events = CloneVectorOfEvents(event.events);

    conditions = event.conditions;
    actions = event.actions;
}

/**
 * Custom copy constructor
 */
StandardEvent::StandardEvent(const StandardEvent & event) :
BaseEvent(event)
{
    Init(event);
}

/**
 * Custom assignement operator
 */
StandardEvent& StandardEvent::operator=(const StandardEvent & event)
{
    if ( this != &event )
    {
        BaseEvent::operator=(event);
        Init(event);
    }

    return *this;
}
