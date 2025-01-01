#include "common/draw_svg.h"

#ifdef GRAPH_LAYOUT_ENABLE_SVG
#include <cairo.h>
#include <cairo-svg.h>
#include <pango/pangocairo.h>
using namespace std;
using namespace graph_layout;

DrawSVG::DrawSVG(const string& path, const double width, const double height) {
    _width = width;
    _height = height;
    _surface = cairo_svg_surface_create(path.c_str(), width, height);
    _cairo = cairo_create(_surface);
}

DrawSVG::~DrawSVG() {
    cairo_destroy(_cairo);
    cairo_surface_finish(_surface);
    cairo_surface_destroy(_surface);
}

void DrawSVG::drawBackground(const double red, const double green, const double blue) const {
    cairo_set_source_rgb(_cairo, red, green, blue);
    cairo_rectangle(_cairo, 0, 0, _width, _height);
    cairo_fill(_cairo);
}

void DrawSVG::drawCircle(const double x, const double y, const double r) const {
    cairo_set_source_rgb(_cairo, 0, 0, 0);
    cairo_set_line_width(_cairo, 1);
    cairo_arc(_cairo, x, y, r, 0, 2 * 3.1415926);
    cairo_stroke(_cairo);
    cairo_new_path(_cairo);
}

void DrawSVG::drawLine(const double x1, const double y1, const double x2, const double y2, const bool arrowhead) const {
    cairo_set_source_rgb(_cairo, 0, 0, 0);
    cairo_set_line_width(_cairo, 1);
    cairo_move_to(_cairo, x1, y1);
    cairo_line_to(_cairo, x2, y2);
    cairo_stroke(_cairo);
    if (arrowhead) {
        const double angle = atan2(y2 - y1, x2 - x1);
        const double x3 = x2 - 10 * cos(angle - M_PI/8);
        const double y3 = y2 - 10 * sin(angle - M_PI/8);
        const double x4 = x2 - 10 * cos(angle + M_PI/8);
        const double y4 = y2 - 10 * sin(angle + M_PI/8);
        cairo_move_to(_cairo, x2, y2);
        cairo_line_to(_cairo, x3, y3);
        cairo_line_to(_cairo, x4, y4);
        cairo_line_to(_cairo, x2, y2);
        cairo_fill(_cairo);
    }
    cairo_new_path(_cairo);
}

void DrawSVG::drawText(const double x, const double y, const std::string &text) const {
    cairo_set_source_rgb(_cairo, 0, 0, 0);
    PangoLayout* layout = pango_cairo_create_layout(_cairo);
    pango_layout_set_text(layout, text.c_str(), -1);
    PangoFontDescription* font_desc = pango_font_description_from_string("Serif 16");
    pango_layout_set_font_description(layout, font_desc);
    pango_font_description_free(font_desc);
    PangoRectangle ink_rect, logical_rect;
    pango_layout_get_extents(layout, &ink_rect, &logical_rect);
    const double iw = ink_rect.width / 1024.0;
    const double ih = ink_rect.height / 1024.0;
    const double textX = x - iw / 2.0 - ink_rect.x / 1024.0;
    const double textY = y - ih / 2.0 - ink_rect.y / 1024.0;
    cairo_move_to(_cairo, textX, textY);
    pango_cairo_show_layout(_cairo, layout);
    g_object_unref(layout);
    cairo_new_path(_cairo);
}

#endif
