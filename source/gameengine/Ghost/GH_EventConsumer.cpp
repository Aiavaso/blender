/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor(s): Tristan Porteries.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Ghost/GH_EventConsumer.cpp
 *  \ingroup ghost
 */


#include "GH_EventConsumer.h"
#include "GH_InputDevice.h"

#include "GHOST_IEvent.h"
#include "GHOST_IWindow.h"
#include "GHOST_ISystem.h"

#include "RAS_ICanvas.h"

#include "BLI_string_utf8.h"

#include <iostream>

GH_EventConsumer::GH_EventConsumer(GHOST_ISystem *system, GH_InputDevice *device, RAS_ICanvas *canvas)
	:m_device(device),
	m_canvas(canvas)
{
	// Setup the default mouse position.
	int cursorx, cursory;
	system->getCursorPosition(cursorx, cursory);
	int x, y;
	m_canvas->ConvertMousePosition(cursorx, cursory, x, y, true);
	m_device->ConvertMoveEvent(x, y);
}

GH_EventConsumer::~GH_EventConsumer()
{
}

void GH_EventConsumer::HandleWindowEvent(GHOST_TEventType type)
{
	m_device->ConvertWindowEvent(type);
}

void GH_EventConsumer::HandleKeyEvent(GHOST_TEventDataPtr data, bool down)
{
	GHOST_TEventKeyData *keyData = (GHOST_TEventKeyData *)data;
	unsigned int unicode = keyData->utf8_buf[0] ? BLI_str_utf8_as_unicode(keyData->utf8_buf) : keyData->ascii;
	m_device->ConvertKeyEvent(keyData->key, down, unicode);
}

void GH_EventConsumer::HandleCursorEvent(GHOST_TEventDataPtr data, GHOST_IWindow *window)
{
	GHOST_TEventCursorData *cursorData = (GHOST_TEventCursorData *)data;
	int x, y;
	m_canvas->ConvertMousePosition(cursorData->x, cursorData->y, x, y, false);

	m_device->ConvertMoveEvent(x, y);
}

void GH_EventConsumer::HandleWheelEvent(GHOST_TEventDataPtr data)
{
	GHOST_TEventWheelData* wheelData = (GHOST_TEventWheelData *)data;

	m_device->ConvertWheelEvent(wheelData->z);
}

void GH_EventConsumer::HandleButtonEvent(GHOST_TEventDataPtr data, bool down)
{
	GHOST_TEventButtonData *buttonData = (GHOST_TEventButtonData *)data;

	m_device->ConvertButtonEvent(buttonData->button, down);
}

bool GH_EventConsumer::processEvent(GHOST_IEvent *event)
{
	GHOST_TEventDataPtr eventData = ((GHOST_IEvent*)event)->getData();
	switch (event->getType()) {
		case GHOST_kEventButtonDown:
		{
			HandleButtonEvent(eventData, true);
			break;
		}

		case GHOST_kEventButtonUp:
		{
			HandleButtonEvent(eventData, false);
			break;
		}

		case GHOST_kEventWheel:
		{
			HandleWheelEvent(eventData);
			break;
		}

		case GHOST_kEventCursorMove:
		{
			HandleCursorEvent(eventData, event->getWindow());
			break;
		}

		case GHOST_kEventKeyDown:
		{
			HandleKeyEvent(eventData, true);
			break;
		}
		case GHOST_kEventKeyUp:
		{
			HandleKeyEvent(eventData, false);
			break;
		}
		case GHOST_kEventWindowSize:
		case GHOST_kEventWindowClose:
		case GHOST_kEventQuit:
		{
			HandleWindowEvent(event->getType());
			break;
		}
		default:
			break;
	}

	return true;
}