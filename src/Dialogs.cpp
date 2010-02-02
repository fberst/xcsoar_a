/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}

*/

#include "Dialogs.h"
#include "Language.hpp"
#include "Version.hpp"
#include "LogFile.hpp"
#include "MapWindowProjection.hpp"
#include "MainWindow.hpp"
#include "SettingsTask.hpp"
#include "Screen/ProgressWindow.hpp"
#include "UtilsText.hpp"
#include "UtilsSystem.hpp"
#include "LocalPath.hpp"
#include "SettingsUser.hpp"
#include "InputEvents.h"
#include "Registry.hpp"
#include "Interface.hpp"
#include "Asset.hpp"

ProgressWindow *XCSoarInterface::progress_window = NULL;

#ifndef ENABLE_SDL
HCURSOR ActionInterface::oldCursor = NULL;
#endif /* !ENABLE_SDL */

/**
 * Activates the Hourglass animation
 */
void
ActionInterface::StartHourglassCursor(void)
{
  #ifdef ENABLE_SDL
  // XXX
  #else /* !ENABLE_SDL */
  HCURSOR newc = LoadCursor(NULL, IDC_WAIT);
  oldCursor = (HCURSOR)SetCursor(newc);
  if (is_altair()) {
    SetCursorPos(160,120);
  }
  #endif /* !ENABLE_SDL */
}

/**
 * Deactivates the Hourglass animation
 */
void
ActionInterface::StopHourglassCursor(void)
{
  #ifdef ENABLE_SDL
  // XXX
  #else /* !ENABLE_SDL */
  SetCursor(oldCursor);
  if (is_altair()) {
    SetCursorPos(640, 480);
  }
  oldCursor = NULL;
  #endif /* !ENABLE_SDL */
}

/**
 * Closes the ProgressWindow
 */
void
XCSoarInterface::CloseProgressDialog()
{
  if (progress_window != NULL) {
    delete progress_window;
    progress_window = NULL;
  }
}

/**
 * Updates the ProgressWindow to go up one step
 */
void
XCSoarInterface::StepProgressDialog(void)
{
  if (progress_window != NULL)
    progress_window->step();
}

bool
XCSoarInterface::SetProgressStepSize(int nSize)
{
  nSize = 5;

  if (nSize < 100 && progress_window != NULL)
    progress_window->set_step(nSize);

  return true;
}

/**
 * Creates or updates the ProgressWindow
 * @param text Text inside the progress bar
 */
void
XCSoarInterface::CreateProgressDialog(const TCHAR* text)
{
  if (progress_window == NULL)
    progress_window = new ProgressWindow(main_window);

  progress_window->set_message(text);
  progress_window->set_pos(0);
}

/**
 * Opens the Analysis window
 */
void
PopupAnalysis()
{
  dlgAnalysisShowModal();
}


#include "Interface.hpp"
#include "MapWindow.hpp"
#include "Waypoint/Waypoints.hpp"

/**
 * Opens up the WaypointDetails window of the nearest
 * waypoint to location
 * @param way_points Waypoints including all possible
 * waypoints for the calculation
 * @param location Location where to search
 * @param range Maximum range to search
 * @param pan True if in Pan mode
 * @return True if a waypoint was found
 */
bool
PopupNearestWaypointDetails(const Waypoints &way_points,
                            const GEOPOINT &location,
                            double range, bool pan)
{
  MapWindowProjection &map_window = XCSoarInterface::main_window.map;

  const Waypoint *way_point;
  if (!pan || !XCSoarInterface::SettingsMap().EnablePan) {
    way_point = way_points.lookup_location(location, fixed(range));
  } else {
    // nearest to center of screen if in pan mode
    way_point = way_points.lookup_location(map_window.GetPanLocation(),
                                           fixed(range));
  }
  if (way_point) {
    dlgWayPointDetailsShowModal(*way_point);
    return true;
  }
  return false;
}

