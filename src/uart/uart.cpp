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
#include <string>

StandaloneViewMirror *standaloneViewMirror = nullptr;

extern "C" void initialize(const standalone_application_api_t &api)
{
    _api = &api;

    standaloneViewMirror = new StandaloneViewMirror();
}

extern "C" void on_event(const uint32_t &events)
{
    (void)events;

    // standaloneViewMirror->get_console().write(".");

    //_api->fill_rectangle(50, 50, 50, 50, 0x7733);

    std::vector<uint8_t> data(66);
    uint8_t more_data_available;
    uint16_t cmd = 0x7F01;
    do
    {
        if (_api->i2c_read((uint8_t *)&cmd, 2, data.data(), 66) == false)
            return;

        uint8_t data_len = data[0];
        more_data_available = data[1];
        uint8_t *data_ptr = data.data() + 2;

        if (data_len > 0)
        {
            standaloneViewMirror->get_console().write(std::string((char *)data_ptr, data_len));
        }
    } while (more_data_available == 1);
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
