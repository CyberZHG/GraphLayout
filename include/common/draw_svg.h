#ifndef GRAPHLAYOUT_SVG_H
#define GRAPHLAYOUT_SVG_H

#ifdef GRAPH_LAYOUT_ENABLE_SVG

#include <string>
#include <cairo.h>

namespace graph_layout {

    class DrawSVG {
    public:
        DrawSVG(const std::string& path, double width, double height);
        ~DrawSVG();

        void drawBackground(double red, double green, double blue) const;

        void drawCircle(double x, double y, double r) const;
        void drawLine(double x1, double y1, double x2, double y2, bool arrowhead = false) const;
        void drawText(double x, double y, const std::string& text) const;

    private:
        double _width, _height;

        cairo_surface_t* _surface{};
        cairo_t* _cairo{};
    };

}

#endif

#endif //GRAPHLAYOUT_SVG_H