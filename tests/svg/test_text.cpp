#include <fstream>

#include <gtest/gtest.h>
using namespace std;

TEST(TestSVGText, SpecialCase1) {
    constexpr double WIDTH = 40;
    constexpr double HEIGHT = 30;
    constexpr double CX = WIDTH / 2;
    constexpr double CY = HEIGHT / 2;
    constexpr double width = 35702 / 1024.0;
    constexpr double height = 12843 / 1024.0;
    constexpr double tx = CX;
    constexpr double ty = CY;
    ofstream file("test_svg_text__special_case_1.svg");
    file << R"(<?xml version="1.0" encoding="UTF-8"?>)" << endl;
    file << format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="{}" height="{}" viewBox="0 0 {} {}">)", WIDTH, HEIGHT, WIDTH, HEIGHT) << endl;
    file << format(R"(  <rect x="0" y="0" width="{}" height="{}" fill="white" />)", WIDTH, HEIGHT) << endl;
    file << format(R"(  <rect x="{}" y="{}" width="{}" height="{}" stroke="red" fill="none" />)", CX - width * 0.5, CY - height * 0.5, width, height) << endl;
    file << format(R"(  <text x="{}" y="{}" text-anchor="middle" dominant-baseline="central" font-family="Serif" font-size="16" fill="black">)", tx, ty);
    file << "start</text>" << endl;
    file << "</svg>" << endl;
    file.close();
}

TEST(TestSVGText, SpecialCase2) {
    constexpr double WIDTH = 40;
    constexpr double HEIGHT = 30;
    constexpr double CX = WIDTH / 2;
    constexpr double CY = HEIGHT / 2;
    constexpr double width = 10069 / 1024.0;
    constexpr double height = 14656 / 1024.0;
    constexpr double tx = CX;
    constexpr double ty = CY;
    ofstream file("test_svg_text__special_case_2.svg");
    file << R"(<?xml version="1.0" encoding="UTF-8"?>)" << endl;
    file << format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="{}" height="{}" viewBox="0 0 {} {}">)", WIDTH, HEIGHT, WIDTH, HEIGHT) << endl;
    file << format(R"(  <rect x="0" y="0" width="{}" height="{}" fill="white" />)", WIDTH, HEIGHT) << endl;
    file << format(R"(  <rect x="{}" y="{}" width="{}" height="{}" stroke="red" fill="none" />)", CX - width * 0.5, CY - height * 0.5, width, height) << endl;
    file << format(R"(  <text x="{}" y="{}" text-anchor="middle" dominant-baseline="central" font-family="Serif" font-size="16" fill="black">)", tx, ty);
    file << "4</text>" << endl;
    file << "</svg>" << endl;
    file.close();
}

TEST(TestSVGText, SpecialCase3) {
    constexpr double WIDTH = 40;
    constexpr double HEIGHT = 30;
    constexpr double CX = WIDTH / 2;
    constexpr double CY = HEIGHT / 2;
    constexpr double width = 39919 / 1024.0;
    constexpr double height = 20622 / 1024.0;
    constexpr double tx = CX;
    constexpr double ty = CY;
    ofstream file("test_svg_text__special_case_3.svg");
    file << R"(<?xml version="1.0" encoding="UTF-8"?>)" << endl;
    file << format(R"(<svg xmlns="http://www.w3.org/2000/svg" width="{}" height="{}" viewBox="0 0 {} {}">)", WIDTH, HEIGHT, WIDTH, HEIGHT) << endl;
    file << format(R"(  <rect x="0" y="0" width="{}" height="{}" fill="white" />)", WIDTH, HEIGHT) << endl;
    file << format(R"(  <rect x="{}" y="{}" width="{}" height="{}" stroke="red" fill="none" />)", CX - width * 0.5, CY - height * 0.5, width, height) << endl;
    file << format(R"(  <text x="{}" y="{}" text-anchor="middle" dominant-baseline="central" font-family="Serif" font-size="16" fill="black">)", tx, ty);
    file << "居中</text>" << endl;
    file << "</svg>" << endl;
    file.close();
}
