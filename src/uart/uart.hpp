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

#pragma once

#include "standalone_application.hpp"

#include "ui/ui_widget.hpp"
#include "ui/theme.hpp"

class StandaloneViewMirror : public ui::View
{
public:
    StandaloneViewMirror(ui::Context &context, const ui::Rect parent_rect) : View{parent_rect}, context_(context)
    {
        set_style(ui::Theme::getInstance()->bg_dark);

        add_children({&text, &console, &button});

        text.set("BR: 115200");

        // console.hidden(false);
        // console.visible(true);

        button.on_select = [this](ui::Button &button)
        {
            button.blur();
            //_api->panic("Button pressed");
            // text.set("BR: 9600");
        };
    }

    ui::Console &get_console()
    {
        return console;
    }

    ui::Context &context() const override
    {
        return context_;
    }

private:
    ui::Text text{{4, 4, 120, 16}};
    ui::Button button{{120, 4 + 24, 116, 24}, "DETECT BR"};

    ui::Console console{{0, 4 * 16, 240, 240}};

    ui::Context &context_;
};
