/*
 * Copyright (C) 2024 Bernd Herzog
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "uart.hpp"

#include <memory>

ui::Widget *standaloneViewMirror = nullptr;

extern "C" void initialize(const standalone_application_api_t &api)
{
    _api = &api;

    standaloneViewMirror = new StandaloneViewMirror();
    standaloneViewMirror->set_style(ui::Theme::getInstance()->bg_dark);
}

extern "C" void on_event(const uint32_t &events)
{
    (void)events;
    //_api->fill_rectangle(50, 50, 50, 50, 0x7733);
}

extern "C" void shutdown()
{
}

extern "C" void PaintViewMirror()
{
    ui::Painter painter;
    if (standaloneViewMirror)
        painter.paint_widget_tree(standaloneViewMirror);
    // standaloneViewMirror->paint(painter);
}
