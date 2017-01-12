#pragma once

#include <QObject>
#include <QColor>
#include <QJSEngine>
#include <QQmlEngine>

class Material
{
public:
    const static QColor red;
    const static QColor red_50;
    const static QColor red_100;
    const static QColor red_200;
    const static QColor red_300;
    const static QColor red_400;
    const static QColor red_500;
    const static QColor red_600;
    const static QColor red_700;
    const static QColor red_800;
    const static QColor red_900;
    const static QColor red_a100;
    const static QColor red_a200;
    const static QColor red_a400;
    const static QColor red_a700;

/* Pink */
    const static QColor pink;
    const static QColor pink_50;
    const static QColor pink_100;
    const static QColor pink_200;
    const static QColor pink_300;
    const static QColor pink_400;
    const static QColor pink_500;
    const static QColor pink_600;
    const static QColor pink_700;
    const static QColor pink_800;
    const static QColor pink_900;
    const static QColor pink_a100;
    const static QColor pink_a200;
    const static QColor pink_a400;
    const static QColor pink_a700;

/* Purple */
    const static QColor purple;
    const static QColor purple_50;
    const static QColor purple_100;
    const static QColor purple_200;
    const static QColor purple_300;
    const static QColor purple_400;
    const static QColor purple_500;
    const static QColor purple_600;
    const static QColor purple_700;
    const static QColor purple_800;
    const static QColor purple_900;
    const static QColor purple_a100;
    const static QColor purple_a200;
    const static QColor purple_a400;
    const static QColor purple_a700;

/* Deep Purple */
    const static QColor deep_purple;
    const static QColor deep_purple_50;
    const static QColor deep_purple_100;
    const static QColor deep_purple_200;
    const static QColor deep_purple_300;
    const static QColor deep_purple_400;
    const static QColor deep_purple_500;
    const static QColor deep_purple_600;
    const static QColor deep_purple_700;
    const static QColor deep_purple_800;
    const static QColor deep_purple_900;
    const static QColor deep_purple_a100;
    const static QColor deep_purple_a200;
    const static QColor deep_purple_a400;
    const static QColor deep_purple_a700;

/* Indigo */
    const static QColor indigo;
    const static QColor indigo_50;
    const static QColor indigo_100;
    const static QColor indigo_200;
    const static QColor indigo_300;
    const static QColor indigo_400;
    const static QColor indigo_500;
    const static QColor indigo_600;
    const static QColor indigo_700;
    const static QColor indigo_800;
    const static QColor indigo_900;
    const static QColor indigo_a100;
    const static QColor indigo_a200;
    const static QColor indigo_a400;
    const static QColor indigo_a700;

/* Blue */
    const static QColor blue;
    const static QColor blue_50;
    const static QColor blue_100;
    const static QColor blue_200;
    const static QColor blue_300;
    const static QColor blue_400;
    const static QColor blue_500;
    const static QColor blue_600;
    const static QColor blue_700;
    const static QColor blue_800;
    const static QColor blue_900;
    const static QColor blue_a100;
    const static QColor blue_a200;
    const static QColor blue_a400;
    const static QColor blue_a700;

/* Light Blue */
    const static QColor light_blue;
    const static QColor light_blue_50;
    const static QColor light_blue_100;
    const static QColor light_blue_200;
    const static QColor light_blue_300;
    const static QColor light_blue_400;
    const static QColor light_blue_500;
    const static QColor light_blue_600;
    const static QColor light_blue_700;
    const static QColor light_blue_800;
    const static QColor light_blue_900;
    const static QColor light_blue_a100;
    const static QColor light_blue_a200;
    const static QColor light_blue_a400;
    const static QColor light_blue_a700;

/* Cyan */
    const static QColor cyan;
    const static QColor cyan_50;
    const static QColor cyan_100;
    const static QColor cyan_200;
    const static QColor cyan_300;
    const static QColor cyan_400;
    const static QColor cyan_500;
    const static QColor cyan_600;
    const static QColor cyan_700;
    const static QColor cyan_800;
    const static QColor cyan_900;
    const static QColor cyan_a100;
    const static QColor cyan_a200;
    const static QColor cyan_a400;
    const static QColor cyan_a700;

/* Teal */
    const static QColor teal;
    const static QColor teal_50;
    const static QColor teal_100;
    const static QColor teal_200;
    const static QColor teal_300;
    const static QColor teal_400;
    const static QColor teal_500;
    const static QColor teal_600;
    const static QColor teal_700;
    const static QColor teal_800;
    const static QColor teal_900;
    const static QColor teal_a100;
    const static QColor teal_a200;
    const static QColor teal_a400;
    const static QColor teal_a700;

/* Green */
    const static QColor green;
    const static QColor green_50;
    const static QColor green_100;
    const static QColor green_200;
    const static QColor green_300;
    const static QColor green_400;
    const static QColor green_500;
    const static QColor green_600;
    const static QColor green_700;
    const static QColor green_800;
    const static QColor green_900;
    const static QColor green_a100;
    const static QColor green_a200;
    const static QColor green_a400;
    const static QColor green_a700;

/* Light Green */
    const static QColor light_green;
    const static QColor light_green_50;
    const static QColor light_green_100;
    const static QColor light_green_200;
    const static QColor light_green_300;
    const static QColor light_green_400;
    const static QColor light_green_500;
    const static QColor light_green_600;
    const static QColor light_green_700;
    const static QColor light_green_800;
    const static QColor light_green_900;
    const static QColor light_green_a100;
    const static QColor light_green_a200;
    const static QColor light_green_a400;
    const static QColor light_green_a700;

/* Lime */
    const static QColor lime;
    const static QColor lime_50;
    const static QColor lime_100;
    const static QColor lime_200;
    const static QColor lime_300;
    const static QColor lime_400;
    const static QColor lime_500;
    const static QColor lime_600;
    const static QColor lime_700;
    const static QColor lime_800;
    const static QColor lime_900;
    const static QColor lime_a100;
    const static QColor lime_a200;
    const static QColor lime_a400;
    const static QColor lime_a700;

/* Yellow */
    const static QColor yellow;
    const static QColor yellow_50;
    const static QColor yellow_100;
    const static QColor yellow_200;
    const static QColor yellow_300;
    const static QColor yellow_400;
    const static QColor yellow_500;
    const static QColor yellow_600;
    const static QColor yellow_700;
    const static QColor yellow_800;
    const static QColor yellow_900;
    const static QColor yellow_a100;
    const static QColor yellow_a200;
    const static QColor yellow_a400;
    const static QColor yellow_a700;

/* Amber */
    const static QColor amber;
    const static QColor amber_50;
    const static QColor amber_100;
    const static QColor amber_200;
    const static QColor amber_300;
    const static QColor amber_400;
    const static QColor amber_500;
    const static QColor amber_600;
    const static QColor amber_700;
    const static QColor amber_800;
    const static QColor amber_900;
    const static QColor amber_a100;
    const static QColor amber_a200;
    const static QColor amber_a400;
    const static QColor amber_a700;

/* Orange */
    const static QColor orange;
    const static QColor orange_50;
    const static QColor orange_100;
    const static QColor orange_200;
    const static QColor orange_300;
    const static QColor orange_400;
    const static QColor orange_500;
    const static QColor orange_600;
    const static QColor orange_700;
    const static QColor orange_800;
    const static QColor orange_900;
    const static QColor orange_a100;
    const static QColor orange_a200;
    const static QColor orange_a400;
    const static QColor orange_a700;

/* Deep Orange */
    const static QColor deep_orange;
    const static QColor deep_orange_50;
    const static QColor deep_orange_100;
    const static QColor deep_orange_200;
    const static QColor deep_orange_300;
    const static QColor deep_orange_400;
    const static QColor deep_orange_500;
    const static QColor deep_orange_600;
    const static QColor deep_orange_700;
    const static QColor deep_orange_800;
    const static QColor deep_orange_900;
    const static QColor deep_orange_a100;
    const static QColor deep_orange_a200;
    const static QColor deep_orange_a400;
    const static QColor deep_orange_a700;

/* Brown */
    const static QColor brown;
    const static QColor brown_50;
    const static QColor brown_100;
    const static QColor brown_200;
    const static QColor brown_300;
    const static QColor brown_400;
    const static QColor brown_500;
    const static QColor brown_600;
    const static QColor brown_700;
    const static QColor brown_800;
    const static QColor brown_900;

/* Grey */
    const static QColor grey;
    const static QColor grey_50;
    const static QColor grey_100;
    const static QColor grey_200;
    const static QColor grey_300;
    const static QColor grey_400;
    const static QColor grey_500;
    const static QColor grey_600;
    const static QColor grey_700;
    const static QColor grey_800;
    const static QColor grey_900;

/* Blue Grey */
    const static QColor blue_grey;
    const static QColor blue_grey_50;
    const static QColor blue_grey_100;
    const static QColor blue_grey_200;
    const static QColor blue_grey_300;
    const static QColor blue_grey_400;
    const static QColor blue_grey_500;
    const static QColor blue_grey_600;
    const static QColor blue_grey_700;
    const static QColor blue_grey_800;
    const static QColor blue_grey_900;

/* White / Black */
    const static QColor white;
    const static QColor black;
};

class MaterialSingleton : public QObject
{
    Q_OBJECT
public:
    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_PROPERTY(QColor red READ _red CONSTANT)
    QColor _red() const { return Material::red; }
    Q_PROPERTY(QColor red_50 READ _red_50 CONSTANT)
    QColor _red_50() const { return Material::red_50; }
    Q_PROPERTY(QColor red_100 READ _red_100 CONSTANT)
    QColor _red_100() const { return Material::red_100; }
    Q_PROPERTY(QColor red_200 READ _red_200 CONSTANT)
    QColor _red_200() const { return Material::red_200; }
    Q_PROPERTY(QColor red_300 READ _red_300 CONSTANT)
    QColor _red_300() const { return Material::red_300; }
    Q_PROPERTY(QColor red_400 READ _red_400 CONSTANT)
    QColor _red_400() const { return Material::red_400; }
    Q_PROPERTY(QColor red_500 READ _red_500 CONSTANT)
    QColor _red_500() const { return Material::red_500; }
    Q_PROPERTY(QColor red_600 READ _red_600 CONSTANT)
    QColor _red_600() const { return Material::red_600; }
    Q_PROPERTY(QColor red_700 READ _red_700 CONSTANT)
    QColor _red_700() const { return Material::red_700; }
    Q_PROPERTY(QColor red_800 READ _red_800 CONSTANT)
    QColor _red_800() const { return Material::red_800; }
    Q_PROPERTY(QColor red_900 READ _red_900 CONSTANT)
    QColor _red_900() const { return Material::red_900; }
    Q_PROPERTY(QColor red_a100 READ _red_a100 CONSTANT)
    QColor _red_a100() const { return Material::red_a100; }
    Q_PROPERTY(QColor red_a200 READ _red_a200 CONSTANT)
    QColor _red_a200() const { return Material::red_a200; }
    Q_PROPERTY(QColor red_a400 READ _red_a400 CONSTANT)
    QColor _red_a400() const { return Material::red_a400; }
    Q_PROPERTY(QColor red_a700 READ _red_a700 CONSTANT)
    QColor _red_a700() const { return Material::red_a700; }

/* Pink */
    Q_PROPERTY(QColor pink READ _pink CONSTANT)
    QColor _pink() const { return Material::pink; }
    Q_PROPERTY(QColor pink_50 READ _pink_50 CONSTANT)
    QColor _pink_50() const { return Material::pink_50; }
    Q_PROPERTY(QColor pink_100 READ _pink_100 CONSTANT)
    QColor _pink_100() const { return Material::pink_100; }
    Q_PROPERTY(QColor pink_200 READ _pink_200 CONSTANT)
    QColor _pink_200() const { return Material::pink_200; }
    Q_PROPERTY(QColor pink_300 READ _pink_300 CONSTANT)
    QColor _pink_300() const { return Material::pink_300; }
    Q_PROPERTY(QColor pink_400 READ _pink_400 CONSTANT)
    QColor _pink_400() const { return Material::pink_400; }
    Q_PROPERTY(QColor pink_500 READ _pink_500 CONSTANT)
    QColor _pink_500() const { return Material::pink_500; }
    Q_PROPERTY(QColor pink_600 READ _pink_600 CONSTANT)
    QColor _pink_600() const { return Material::pink_600; }
    Q_PROPERTY(QColor pink_700 READ _pink_700 CONSTANT)
    QColor _pink_700() const { return Material::pink_700; }
    Q_PROPERTY(QColor pink_800 READ _pink_800 CONSTANT)
    QColor _pink_800() const { return Material::pink_800; }
    Q_PROPERTY(QColor pink_900 READ _pink_900 CONSTANT)
    QColor _pink_900() const { return Material::pink_900; }
    Q_PROPERTY(QColor pink_a100 READ _pink_a100 CONSTANT)
    QColor _pink_a100() const { return Material::pink_a100; }
    Q_PROPERTY(QColor pink_a200 READ _pink_a200 CONSTANT)
    QColor _pink_a200() const { return Material::pink_a200; }
    Q_PROPERTY(QColor pink_a400 READ _pink_a400 CONSTANT)
    QColor _pink_a400() const { return Material::pink_a400; }
    Q_PROPERTY(QColor pink_a700 READ _pink_a700 CONSTANT)
    QColor _pink_a700() const { return Material::pink_a700; }

/* Purple */
    Q_PROPERTY(QColor purple READ _purple CONSTANT)
    QColor _purple() const { return Material::purple; }
    Q_PROPERTY(QColor purple_50 READ _purple_50 CONSTANT)
    QColor _purple_50() const { return Material::purple_50; }
    Q_PROPERTY(QColor purple_100 READ _purple_100 CONSTANT)
    QColor _purple_100() const { return Material::purple_100; }
    Q_PROPERTY(QColor purple_200 READ _purple_200 CONSTANT)
    QColor _purple_200() const { return Material::purple_200; }
    Q_PROPERTY(QColor purple_300 READ _purple_300 CONSTANT)
    QColor _purple_300() const { return Material::purple_300; }
    Q_PROPERTY(QColor purple_400 READ _purple_400 CONSTANT)
    QColor _purple_400() const { return Material::purple_400; }
    Q_PROPERTY(QColor purple_500 READ _purple_500 CONSTANT)
    QColor _purple_500() const { return Material::purple_500; }
    Q_PROPERTY(QColor purple_600 READ _purple_600 CONSTANT)
    QColor _purple_600() const { return Material::purple_600; }
    Q_PROPERTY(QColor purple_700 READ _purple_700 CONSTANT)
    QColor _purple_700() const { return Material::purple_700; }
    Q_PROPERTY(QColor purple_800 READ _purple_800 CONSTANT)
    QColor _purple_800() const { return Material::purple_800; }
    Q_PROPERTY(QColor purple_900 READ _purple_900 CONSTANT)
    QColor _purple_900() const { return Material::purple_900; }
    Q_PROPERTY(QColor purple_a100 READ _purple_a100 CONSTANT)
    QColor _purple_a100() const { return Material::purple_a100; }
    Q_PROPERTY(QColor purple_a200 READ _purple_a200 CONSTANT)
    QColor _purple_a200() const { return Material::purple_a200; }
    Q_PROPERTY(QColor purple_a400 READ _purple_a400 CONSTANT)
    QColor _purple_a400() const { return Material::purple_a400; }
    Q_PROPERTY(QColor purple_a700 READ _purple_a700 CONSTANT)
    QColor _purple_a700() const { return Material::purple_a700; }

/* Deep Purple */
    Q_PROPERTY(QColor deep_purple READ _deep_purple CONSTANT)
    QColor _deep_purple() const { return Material::deep_purple; }
    Q_PROPERTY(QColor deep_purple_50 READ _deep_purple_50 CONSTANT)
    QColor _deep_purple_50() const { return Material::deep_purple_50; }
    Q_PROPERTY(QColor deep_purple_100 READ _deep_purple_100 CONSTANT)
    QColor _deep_purple_100() const { return Material::deep_purple_100; }
    Q_PROPERTY(QColor deep_purple_200 READ _deep_purple_200 CONSTANT)
    QColor _deep_purple_200() const { return Material::deep_purple_200; }
    Q_PROPERTY(QColor deep_purple_300 READ _deep_purple_300 CONSTANT)
    QColor _deep_purple_300() const { return Material::deep_purple_300; }
    Q_PROPERTY(QColor deep_purple_400 READ _deep_purple_400 CONSTANT)
    QColor _deep_purple_400() const { return Material::deep_purple_400; }
    Q_PROPERTY(QColor deep_purple_500 READ _deep_purple_500 CONSTANT)
    QColor _deep_purple_500() const { return Material::deep_purple_500; }
    Q_PROPERTY(QColor deep_purple_600 READ _deep_purple_600 CONSTANT)
    QColor _deep_purple_600() const { return Material::deep_purple_600; }
    Q_PROPERTY(QColor deep_purple_700 READ _deep_purple_700 CONSTANT)
    QColor _deep_purple_700() const { return Material::deep_purple_700; }
    Q_PROPERTY(QColor deep_purple_800 READ _deep_purple_800 CONSTANT)
    QColor _deep_purple_800() const { return Material::deep_purple_800; }
    Q_PROPERTY(QColor deep_purple_900 READ _deep_purple_900 CONSTANT)
    QColor _deep_purple_900() const { return Material::deep_purple_900; }
    Q_PROPERTY(QColor deep_purple_a100 READ _deep_purple_a100 CONSTANT)
    QColor _deep_purple_a100() const { return Material::deep_purple_a100; }
    Q_PROPERTY(QColor deep_purple_a200 READ _deep_purple_a200 CONSTANT)
    QColor _deep_purple_a200() const { return Material::deep_purple_a200; }
    Q_PROPERTY(QColor deep_purple_a400 READ _deep_purple_a400 CONSTANT)
    QColor _deep_purple_a400() const { return Material::deep_purple_a400; }
    Q_PROPERTY(QColor deep_purple_a700 READ _deep_purple_a700 CONSTANT)
    QColor _deep_purple_a700() const { return Material::deep_purple_a700; }

/* Indigo */
    Q_PROPERTY(QColor indigo READ _indigo CONSTANT)
    QColor _indigo() const { return Material::indigo; }
    Q_PROPERTY(QColor indigo_50 READ _indigo_50 CONSTANT)
    QColor _indigo_50() const { return Material::indigo_50; }
    Q_PROPERTY(QColor indigo_100 READ _indigo_100 CONSTANT)
    QColor _indigo_100() const { return Material::indigo_100; }
    Q_PROPERTY(QColor indigo_200 READ _indigo_200 CONSTANT)
    QColor _indigo_200() const { return Material::indigo_200; }
    Q_PROPERTY(QColor indigo_300 READ _indigo_300 CONSTANT)
    QColor _indigo_300() const { return Material::indigo_300; }
    Q_PROPERTY(QColor indigo_400 READ _indigo_400 CONSTANT)
    QColor _indigo_400() const { return Material::indigo_400; }
    Q_PROPERTY(QColor indigo_500 READ _indigo_500 CONSTANT)
    QColor _indigo_500() const { return Material::indigo_500; }
    Q_PROPERTY(QColor indigo_600 READ _indigo_600 CONSTANT)
    QColor _indigo_600() const { return Material::indigo_600; }
    Q_PROPERTY(QColor indigo_700 READ _indigo_700 CONSTANT)
    QColor _indigo_700() const { return Material::indigo_700; }
    Q_PROPERTY(QColor indigo_800 READ _indigo_800 CONSTANT)
    QColor _indigo_800() const { return Material::indigo_800; }
    Q_PROPERTY(QColor indigo_900 READ _indigo_900 CONSTANT)
    QColor _indigo_900() const { return Material::indigo_900; }
    Q_PROPERTY(QColor indigo_a100 READ _indigo_a100 CONSTANT)
    QColor _indigo_a100() const { return Material::indigo_a100; }
    Q_PROPERTY(QColor indigo_a200 READ _indigo_a200 CONSTANT)
    QColor _indigo_a200() const { return Material::indigo_a200; }
    Q_PROPERTY(QColor indigo_a400 READ _indigo_a400 CONSTANT)
    QColor _indigo_a400() const { return Material::indigo_a400; }
    Q_PROPERTY(QColor indigo_a700 READ _indigo_a700 CONSTANT)
    QColor _indigo_a700() const { return Material::indigo_a700; }

/* Blue */
    Q_PROPERTY(QColor blue READ _blue CONSTANT)
    QColor _blue() const { return Material::blue; }
    Q_PROPERTY(QColor blue_50 READ _blue_50 CONSTANT)
    QColor _blue_50() const { return Material::blue_50; }
    Q_PROPERTY(QColor blue_100 READ _blue_100 CONSTANT)
    QColor _blue_100() const { return Material::blue_100; }
    Q_PROPERTY(QColor blue_200 READ _blue_200 CONSTANT)
    QColor _blue_200() const { return Material::blue_200; }
    Q_PROPERTY(QColor blue_300 READ _blue_300 CONSTANT)
    QColor _blue_300() const { return Material::blue_300; }
    Q_PROPERTY(QColor blue_400 READ _blue_400 CONSTANT)
    QColor _blue_400() const { return Material::blue_400; }
    Q_PROPERTY(QColor blue_500 READ _blue_500 CONSTANT)
    QColor _blue_500() const { return Material::blue_500; }
    Q_PROPERTY(QColor blue_600 READ _blue_600 CONSTANT)
    QColor _blue_600() const { return Material::blue_600; }
    Q_PROPERTY(QColor blue_700 READ _blue_700 CONSTANT)
    QColor _blue_700() const { return Material::blue_700; }
    Q_PROPERTY(QColor blue_800 READ _blue_800 CONSTANT)
    QColor _blue_800() const { return Material::blue_800; }
    Q_PROPERTY(QColor blue_900 READ _blue_900 CONSTANT)
    QColor _blue_900() const { return Material::blue_900; }
    Q_PROPERTY(QColor blue_a100 READ _blue_a100 CONSTANT)
    QColor _blue_a100() const { return Material::blue_a100; }
    Q_PROPERTY(QColor blue_a200 READ _blue_a200 CONSTANT)
    QColor _blue_a200() const { return Material::blue_a200; }
    Q_PROPERTY(QColor blue_a400 READ _blue_a400 CONSTANT)
    QColor _blue_a400() const { return Material::blue_a400; }
    Q_PROPERTY(QColor blue_a700 READ _blue_a700 CONSTANT)
    QColor _blue_a700() const { return Material::blue_a700; }

/* Light Blue */
    Q_PROPERTY(QColor light_blue READ _light_blue CONSTANT)
    QColor _light_blue() const { return Material::light_blue; }
    Q_PROPERTY(QColor light_blue_50 READ _light_blue_50 CONSTANT)
    QColor _light_blue_50() const { return Material::light_blue_50; }
    Q_PROPERTY(QColor light_blue_100 READ _light_blue_100 CONSTANT)
    QColor _light_blue_100() const { return Material::light_blue_100; }
    Q_PROPERTY(QColor light_blue_200 READ _light_blue_200 CONSTANT)
    QColor _light_blue_200() const { return Material::light_blue_200; }
    Q_PROPERTY(QColor light_blue_300 READ _light_blue_300 CONSTANT)
    QColor _light_blue_300() const { return Material::light_blue_300; }
    Q_PROPERTY(QColor light_blue_400 READ _light_blue_400 CONSTANT)
    QColor _light_blue_400() const { return Material::light_blue_400; }
    Q_PROPERTY(QColor light_blue_500 READ _light_blue_500 CONSTANT)
    QColor _light_blue_500() const { return Material::light_blue_500; }
    Q_PROPERTY(QColor light_blue_600 READ _light_blue_600 CONSTANT)
    QColor _light_blue_600() const { return Material::light_blue_600; }
    Q_PROPERTY(QColor light_blue_700 READ _light_blue_700 CONSTANT)
    QColor _light_blue_700() const { return Material::light_blue_700; }
    Q_PROPERTY(QColor light_blue_800 READ _light_blue_800 CONSTANT)
    QColor _light_blue_800() const { return Material::light_blue_800; }
    Q_PROPERTY(QColor light_blue_900 READ _light_blue_900 CONSTANT)
    QColor _light_blue_900() const { return Material::light_blue_900; }
    Q_PROPERTY(QColor light_blue_a100 READ _light_blue_a100 CONSTANT)
    QColor _light_blue_a100() const { return Material::light_blue_a100; }
    Q_PROPERTY(QColor light_blue_a200 READ _light_blue_a200 CONSTANT)
    QColor _light_blue_a200() const { return Material::light_blue_a200; }
    Q_PROPERTY(QColor light_blue_a400 READ _light_blue_a400 CONSTANT)
    QColor _light_blue_a400() const { return Material::light_blue_a400; }
    Q_PROPERTY(QColor light_blue_a700 READ _light_blue_a700 CONSTANT)
    QColor _light_blue_a700() const { return Material::light_blue_a700; }

/* Cyan */
    Q_PROPERTY(QColor cyan READ _cyan CONSTANT)
    QColor _cyan() const { return Material::cyan; }
    Q_PROPERTY(QColor cyan_50 READ _cyan_50 CONSTANT)
    QColor _cyan_50() const { return Material::cyan_50; }
    Q_PROPERTY(QColor cyan_100 READ _cyan_100 CONSTANT)
    QColor _cyan_100() const { return Material::cyan_100; }
    Q_PROPERTY(QColor cyan_200 READ _cyan_200 CONSTANT)
    QColor _cyan_200() const { return Material::cyan_200; }
    Q_PROPERTY(QColor cyan_300 READ _cyan_300 CONSTANT)
    QColor _cyan_300() const { return Material::cyan_300; }
    Q_PROPERTY(QColor cyan_400 READ _cyan_400 CONSTANT)
    QColor _cyan_400() const { return Material::cyan_400; }
    Q_PROPERTY(QColor cyan_500 READ _cyan_500 CONSTANT)
    QColor _cyan_500() const { return Material::cyan_500; }
    Q_PROPERTY(QColor cyan_600 READ _cyan_600 CONSTANT)
    QColor _cyan_600() const { return Material::cyan_600; }
    Q_PROPERTY(QColor cyan_700 READ _cyan_700 CONSTANT)
    QColor _cyan_700() const { return Material::cyan_700; }
    Q_PROPERTY(QColor cyan_800 READ _cyan_800 CONSTANT)
    QColor _cyan_800() const { return Material::cyan_800; }
    Q_PROPERTY(QColor cyan_900 READ _cyan_900 CONSTANT)
    QColor _cyan_900() const { return Material::cyan_900; }
    Q_PROPERTY(QColor cyan_a100 READ _cyan_a100 CONSTANT)
    QColor _cyan_a100() const { return Material::cyan_a100; }
    Q_PROPERTY(QColor cyan_a200 READ _cyan_a200 CONSTANT)
    QColor _cyan_a200() const { return Material::cyan_a200; }
    Q_PROPERTY(QColor cyan_a400 READ _cyan_a400 CONSTANT)
    QColor _cyan_a400() const { return Material::cyan_a400; }
    Q_PROPERTY(QColor cyan_a700 READ _cyan_a700 CONSTANT)
    QColor _cyan_a700() const { return Material::cyan_a700; }

/* Teal */
    Q_PROPERTY(QColor teal READ _teal CONSTANT)
    QColor _teal() const { return Material::teal; }
    Q_PROPERTY(QColor teal_50 READ _teal_50 CONSTANT)
    QColor _teal_50() const { return Material::teal_50; }
    Q_PROPERTY(QColor teal_100 READ _teal_100 CONSTANT)
    QColor _teal_100() const { return Material::teal_100; }
    Q_PROPERTY(QColor teal_200 READ _teal_200 CONSTANT)
    QColor _teal_200() const { return Material::teal_200; }
    Q_PROPERTY(QColor teal_300 READ _teal_300 CONSTANT)
    QColor _teal_300() const { return Material::teal_300; }
    Q_PROPERTY(QColor teal_400 READ _teal_400 CONSTANT)
    QColor _teal_400() const { return Material::teal_400; }
    Q_PROPERTY(QColor teal_500 READ _teal_500 CONSTANT)
    QColor _teal_500() const { return Material::teal_500; }
    Q_PROPERTY(QColor teal_600 READ _teal_600 CONSTANT)
    QColor _teal_600() const { return Material::teal_600; }
    Q_PROPERTY(QColor teal_700 READ _teal_700 CONSTANT)
    QColor _teal_700() const { return Material::teal_700; }
    Q_PROPERTY(QColor teal_800 READ _teal_800 CONSTANT)
    QColor _teal_800() const { return Material::teal_800; }
    Q_PROPERTY(QColor teal_900 READ _teal_900 CONSTANT)
    QColor _teal_900() const { return Material::teal_900; }
    Q_PROPERTY(QColor teal_a100 READ _teal_a100 CONSTANT)
    QColor _teal_a100() const { return Material::teal_a100; }
    Q_PROPERTY(QColor teal_a200 READ _teal_a200 CONSTANT)
    QColor _teal_a200() const { return Material::teal_a200; }
    Q_PROPERTY(QColor teal_a400 READ _teal_a400 CONSTANT)
    QColor _teal_a400() const { return Material::teal_a400; }
    Q_PROPERTY(QColor teal_a700 READ _teal_a700 CONSTANT)
    QColor _teal_a700() const { return Material::teal_a700; }

/* Green */
    Q_PROPERTY(QColor green READ _green CONSTANT)
    QColor _green() const { return Material::green; }
    Q_PROPERTY(QColor green_50 READ _green_50 CONSTANT)
    QColor _green_50() const { return Material::green_50; }
    Q_PROPERTY(QColor green_100 READ _green_100 CONSTANT)
    QColor _green_100() const { return Material::green_100; }
    Q_PROPERTY(QColor green_200 READ _green_200 CONSTANT)
    QColor _green_200() const { return Material::green_200; }
    Q_PROPERTY(QColor green_300 READ _green_300 CONSTANT)
    QColor _green_300() const { return Material::green_300; }
    Q_PROPERTY(QColor green_400 READ _green_400 CONSTANT)
    QColor _green_400() const { return Material::green_400; }
    Q_PROPERTY(QColor green_500 READ _green_500 CONSTANT)
    QColor _green_500() const { return Material::green_500; }
    Q_PROPERTY(QColor green_600 READ _green_600 CONSTANT)
    QColor _green_600() const { return Material::green_600; }
    Q_PROPERTY(QColor green_700 READ _green_700 CONSTANT)
    QColor _green_700() const { return Material::green_700; }
    Q_PROPERTY(QColor green_800 READ _green_800 CONSTANT)
    QColor _green_800() const { return Material::green_800; }
    Q_PROPERTY(QColor green_900 READ _green_900 CONSTANT)
    QColor _green_900() const { return Material::green_900; }
    Q_PROPERTY(QColor green_a100 READ _green_a100 CONSTANT)
    QColor _green_a100() const { return Material::green_a100; }
    Q_PROPERTY(QColor green_a200 READ _green_a200 CONSTANT)
    QColor _green_a200() const { return Material::green_a200; }
    Q_PROPERTY(QColor green_a400 READ _green_a400 CONSTANT)
    QColor _green_a400() const { return Material::green_a400; }
    Q_PROPERTY(QColor green_a700 READ _green_a700 CONSTANT)
    QColor _green_a700() const { return Material::green_a700; }

/* Light Green */
    Q_PROPERTY(QColor light_green READ _light_green CONSTANT)
    QColor _light_green() const { return Material::light_green; }
    Q_PROPERTY(QColor light_green_50 READ _light_green_50 CONSTANT)
    QColor _light_green_50() const { return Material::light_green_50; }
    Q_PROPERTY(QColor light_green_100 READ _light_green_100 CONSTANT)
    QColor _light_green_100() const { return Material::light_green_100; }
    Q_PROPERTY(QColor light_green_200 READ _light_green_200 CONSTANT)
    QColor _light_green_200() const { return Material::light_green_200; }
    Q_PROPERTY(QColor light_green_300 READ _light_green_300 CONSTANT)
    QColor _light_green_300() const { return Material::light_green_300; }
    Q_PROPERTY(QColor light_green_400 READ _light_green_400 CONSTANT)
    QColor _light_green_400() const { return Material::light_green_400; }
    Q_PROPERTY(QColor light_green_500 READ _light_green_500 CONSTANT)
    QColor _light_green_500() const { return Material::light_green_500; }
    Q_PROPERTY(QColor light_green_600 READ _light_green_600 CONSTANT)
    QColor _light_green_600() const { return Material::light_green_600; }
    Q_PROPERTY(QColor light_green_700 READ _light_green_700 CONSTANT)
    QColor _light_green_700() const { return Material::light_green_700; }
    Q_PROPERTY(QColor light_green_800 READ _light_green_800 CONSTANT)
    QColor _light_green_800() const { return Material::light_green_800; }
    Q_PROPERTY(QColor light_green_900 READ _light_green_900 CONSTANT)
    QColor _light_green_900() const { return Material::light_green_900; }
    Q_PROPERTY(QColor light_green_a100 READ _light_green_a100 CONSTANT)
    QColor _light_green_a100() const { return Material::light_green_a100; }
    Q_PROPERTY(QColor light_green_a200 READ _light_green_a200 CONSTANT)
    QColor _light_green_a200() const { return Material::light_green_a200; }
    Q_PROPERTY(QColor light_green_a400 READ _light_green_a400 CONSTANT)
    QColor _light_green_a400() const { return Material::light_green_a400; }
    Q_PROPERTY(QColor light_green_a700 READ _light_green_a700 CONSTANT)
    QColor _light_green_a700() const { return Material::light_green_a700; }

/* Lime */
    Q_PROPERTY(QColor lime READ _lime CONSTANT)
    QColor _lime() const { return Material::lime; }
    Q_PROPERTY(QColor lime_50 READ _lime_50 CONSTANT)
    QColor _lime_50() const { return Material::lime_50; }
    Q_PROPERTY(QColor lime_100 READ _lime_100 CONSTANT)
    QColor _lime_100() const { return Material::lime_100; }
    Q_PROPERTY(QColor lime_200 READ _lime_200 CONSTANT)
    QColor _lime_200() const { return Material::lime_200; }
    Q_PROPERTY(QColor lime_300 READ _lime_300 CONSTANT)
    QColor _lime_300() const { return Material::lime_300; }
    Q_PROPERTY(QColor lime_400 READ _lime_400 CONSTANT)
    QColor _lime_400() const { return Material::lime_400; }
    Q_PROPERTY(QColor lime_500 READ _lime_500 CONSTANT)
    QColor _lime_500() const { return Material::lime_500; }
    Q_PROPERTY(QColor lime_600 READ _lime_600 CONSTANT)
    QColor _lime_600() const { return Material::lime_600; }
    Q_PROPERTY(QColor lime_700 READ _lime_700 CONSTANT)
    QColor _lime_700() const { return Material::lime_700; }
    Q_PROPERTY(QColor lime_800 READ _lime_800 CONSTANT)
    QColor _lime_800() const { return Material::lime_800; }
    Q_PROPERTY(QColor lime_900 READ _lime_900 CONSTANT)
    QColor _lime_900() const { return Material::lime_900; }
    Q_PROPERTY(QColor lime_a100 READ _lime_a100 CONSTANT)
    QColor _lime_a100() const { return Material::lime_a100; }
    Q_PROPERTY(QColor lime_a200 READ _lime_a200 CONSTANT)
    QColor _lime_a200() const { return Material::lime_a200; }
    Q_PROPERTY(QColor lime_a400 READ _lime_a400 CONSTANT)
    QColor _lime_a400() const { return Material::lime_a400; }
    Q_PROPERTY(QColor lime_a700 READ _lime_a700 CONSTANT)
    QColor _lime_a700() const { return Material::lime_a700; }

/* Yellow */
    Q_PROPERTY(QColor yellow READ _yellow CONSTANT)
    QColor _yellow() const { return Material::yellow; }
    Q_PROPERTY(QColor yellow_50 READ _yellow_50 CONSTANT)
    QColor _yellow_50() const { return Material::yellow_50; }
    Q_PROPERTY(QColor yellow_100 READ _yellow_100 CONSTANT)
    QColor _yellow_100() const { return Material::yellow_100; }
    Q_PROPERTY(QColor yellow_200 READ _yellow_200 CONSTANT)
    QColor _yellow_200() const { return Material::yellow_200; }
    Q_PROPERTY(QColor yellow_300 READ _yellow_300 CONSTANT)
    QColor _yellow_300() const { return Material::yellow_300; }
    Q_PROPERTY(QColor yellow_400 READ _yellow_400 CONSTANT)
    QColor _yellow_400() const { return Material::yellow_400; }
    Q_PROPERTY(QColor yellow_500 READ _yellow_500 CONSTANT)
    QColor _yellow_500() const { return Material::yellow_500; }
    Q_PROPERTY(QColor yellow_600 READ _yellow_600 CONSTANT)
    QColor _yellow_600() const { return Material::yellow_600; }
    Q_PROPERTY(QColor yellow_700 READ _yellow_700 CONSTANT)
    QColor _yellow_700() const { return Material::yellow_700; }
    Q_PROPERTY(QColor yellow_800 READ _yellow_800 CONSTANT)
    QColor _yellow_800() const { return Material::yellow_800; }
    Q_PROPERTY(QColor yellow_900 READ _yellow_900 CONSTANT)
    QColor _yellow_900() const { return Material::yellow_900; }
    Q_PROPERTY(QColor yellow_a100 READ _yellow_a100 CONSTANT)
    QColor _yellow_a100() const { return Material::yellow_a100; }
    Q_PROPERTY(QColor yellow_a200 READ _yellow_a200 CONSTANT)
    QColor _yellow_a200() const { return Material::yellow_a200; }
    Q_PROPERTY(QColor yellow_a400 READ _yellow_a400 CONSTANT)
    QColor _yellow_a400() const { return Material::yellow_a400; }
    Q_PROPERTY(QColor yellow_a700 READ _yellow_a700 CONSTANT)
    QColor _yellow_a700() const { return Material::yellow_a700; }

/* Amber */
    Q_PROPERTY(QColor amber READ _amber CONSTANT)
    QColor _amber() const { return Material::amber; }
    Q_PROPERTY(QColor amber_50 READ _amber_50 CONSTANT)
    QColor _amber_50() const { return Material::amber_50; }
    Q_PROPERTY(QColor amber_100 READ _amber_100 CONSTANT)
    QColor _amber_100() const { return Material::amber_100; }
    Q_PROPERTY(QColor amber_200 READ _amber_200 CONSTANT)
    QColor _amber_200() const { return Material::amber_200; }
    Q_PROPERTY(QColor amber_300 READ _amber_300 CONSTANT)
    QColor _amber_300() const { return Material::amber_300; }
    Q_PROPERTY(QColor amber_400 READ _amber_400 CONSTANT)
    QColor _amber_400() const { return Material::amber_400; }
    Q_PROPERTY(QColor amber_500 READ _amber_500 CONSTANT)
    QColor _amber_500() const { return Material::amber_500; }
    Q_PROPERTY(QColor amber_600 READ _amber_600 CONSTANT)
    QColor _amber_600() const { return Material::amber_600; }
    Q_PROPERTY(QColor amber_700 READ _amber_700 CONSTANT)
    QColor _amber_700() const { return Material::amber_700; }
    Q_PROPERTY(QColor amber_800 READ _amber_800 CONSTANT)
    QColor _amber_800() const { return Material::amber_800; }
    Q_PROPERTY(QColor amber_900 READ _amber_900 CONSTANT)
    QColor _amber_900() const { return Material::amber_900; }
    Q_PROPERTY(QColor amber_a100 READ _amber_a100 CONSTANT)
    QColor _amber_a100() const { return Material::amber_a100; }
    Q_PROPERTY(QColor amber_a200 READ _amber_a200 CONSTANT)
    QColor _amber_a200() const { return Material::amber_a200; }
    Q_PROPERTY(QColor amber_a400 READ _amber_a400 CONSTANT)
    QColor _amber_a400() const { return Material::amber_a400; }
    Q_PROPERTY(QColor amber_a700 READ _amber_a700 CONSTANT)
    QColor _amber_a700() const { return Material::amber_a700; }

/* Orange */
    Q_PROPERTY(QColor orange READ _orange CONSTANT)
    QColor _orange() const { return Material::orange; }
    Q_PROPERTY(QColor orange_50 READ _orange_50 CONSTANT)
    QColor _orange_50() const { return Material::orange_50; }
    Q_PROPERTY(QColor orange_100 READ _orange_100 CONSTANT)
    QColor _orange_100() const { return Material::orange_100; }
    Q_PROPERTY(QColor orange_200 READ _orange_200 CONSTANT)
    QColor _orange_200() const { return Material::orange_200; }
    Q_PROPERTY(QColor orange_300 READ _orange_300 CONSTANT)
    QColor _orange_300() const { return Material::orange_300; }
    Q_PROPERTY(QColor orange_400 READ _orange_400 CONSTANT)
    QColor _orange_400() const { return Material::orange_400; }
    Q_PROPERTY(QColor orange_500 READ _orange_500 CONSTANT)
    QColor _orange_500() const { return Material::orange_500; }
    Q_PROPERTY(QColor orange_600 READ _orange_600 CONSTANT)
    QColor _orange_600() const { return Material::orange_600; }
    Q_PROPERTY(QColor orange_700 READ _orange_700 CONSTANT)
    QColor _orange_700() const { return Material::orange_700; }
    Q_PROPERTY(QColor orange_800 READ _orange_800 CONSTANT)
    QColor _orange_800() const { return Material::orange_800; }
    Q_PROPERTY(QColor orange_900 READ _orange_900 CONSTANT)
    QColor _orange_900() const { return Material::orange_900; }
    Q_PROPERTY(QColor orange_a100 READ _orange_a100 CONSTANT)
    QColor _orange_a100() const { return Material::orange_a100; }
    Q_PROPERTY(QColor orange_a200 READ _orange_a200 CONSTANT)
    QColor _orange_a200() const { return Material::orange_a200; }
    Q_PROPERTY(QColor orange_a400 READ _orange_a400 CONSTANT)
    QColor _orange_a400() const { return Material::orange_a400; }
    Q_PROPERTY(QColor orange_a700 READ _orange_a700 CONSTANT)
    QColor _orange_a700() const { return Material::orange_a700; }

/* Deep Orange */
    Q_PROPERTY(QColor deep_orange READ _deep_orange CONSTANT)
    QColor _deep_orange() const { return Material::deep_orange; }
    Q_PROPERTY(QColor deep_orange_50 READ _deep_orange_50 CONSTANT)
    QColor _deep_orange_50() const { return Material::deep_orange_50; }
    Q_PROPERTY(QColor deep_orange_100 READ _deep_orange_100 CONSTANT)
    QColor _deep_orange_100() const { return Material::deep_orange_100; }
    Q_PROPERTY(QColor deep_orange_200 READ _deep_orange_200 CONSTANT)
    QColor _deep_orange_200() const { return Material::deep_orange_200; }
    Q_PROPERTY(QColor deep_orange_300 READ _deep_orange_300 CONSTANT)
    QColor _deep_orange_300() const { return Material::deep_orange_300; }
    Q_PROPERTY(QColor deep_orange_400 READ _deep_orange_400 CONSTANT)
    QColor _deep_orange_400() const { return Material::deep_orange_400; }
    Q_PROPERTY(QColor deep_orange_500 READ _deep_orange_500 CONSTANT)
    QColor _deep_orange_500() const { return Material::deep_orange_500; }
    Q_PROPERTY(QColor deep_orange_600 READ _deep_orange_600 CONSTANT)
    QColor _deep_orange_600() const { return Material::deep_orange_600; }
    Q_PROPERTY(QColor deep_orange_700 READ _deep_orange_700 CONSTANT)
    QColor _deep_orange_700() const { return Material::deep_orange_700; }
    Q_PROPERTY(QColor deep_orange_800 READ _deep_orange_800 CONSTANT)
    QColor _deep_orange_800() const { return Material::deep_orange_800; }
    Q_PROPERTY(QColor deep_orange_900 READ _deep_orange_900 CONSTANT)
    QColor _deep_orange_900() const { return Material::deep_orange_900; }
    Q_PROPERTY(QColor deep_orange_a100 READ _deep_orange_a100 CONSTANT)
    QColor _deep_orange_a100() const { return Material::deep_orange_a100; }
    Q_PROPERTY(QColor deep_orange_a200 READ _deep_orange_a200 CONSTANT)
    QColor _deep_orange_a200() const { return Material::deep_orange_a200; }
    Q_PROPERTY(QColor deep_orange_a400 READ _deep_orange_a400 CONSTANT)
    QColor _deep_orange_a400() const { return Material::deep_orange_a400; }
    Q_PROPERTY(QColor deep_orange_a700 READ _deep_orange_a700 CONSTANT)
    QColor _deep_orange_a700() const { return Material::deep_orange_a700; }

/* Brown */
    Q_PROPERTY(QColor brown READ _brown CONSTANT)
    QColor _brown() const { return Material::brown; }
    Q_PROPERTY(QColor brown_50 READ _brown_50 CONSTANT)
    QColor _brown_50() const { return Material::brown_50; }
    Q_PROPERTY(QColor brown_100 READ _brown_100 CONSTANT)
    QColor _brown_100() const { return Material::brown_100; }
    Q_PROPERTY(QColor brown_200 READ _brown_200 CONSTANT)
    QColor _brown_200() const { return Material::brown_200; }
    Q_PROPERTY(QColor brown_300 READ _brown_300 CONSTANT)
    QColor _brown_300() const { return Material::brown_300; }
    Q_PROPERTY(QColor brown_400 READ _brown_400 CONSTANT)
    QColor _brown_400() const { return Material::brown_400; }
    Q_PROPERTY(QColor brown_500 READ _brown_500 CONSTANT)
    QColor _brown_500() const { return Material::brown_500; }
    Q_PROPERTY(QColor brown_600 READ _brown_600 CONSTANT)
    QColor _brown_600() const { return Material::brown_600; }
    Q_PROPERTY(QColor brown_700 READ _brown_700 CONSTANT)
    QColor _brown_700() const { return Material::brown_700; }
    Q_PROPERTY(QColor brown_800 READ _brown_800 CONSTANT)
    QColor _brown_800() const { return Material::brown_800; }
    Q_PROPERTY(QColor brown_900 READ _brown_900 CONSTANT)
    QColor _brown_900() const { return Material::brown_900; }

/* Grey */
    Q_PROPERTY(QColor grey READ _grey CONSTANT)
    QColor _grey() const { return Material::grey; }
    Q_PROPERTY(QColor grey_50 READ _grey_50 CONSTANT)
    QColor _grey_50() const { return Material::grey_50; }
    Q_PROPERTY(QColor grey_100 READ _grey_100 CONSTANT)
    QColor _grey_100() const { return Material::grey_100; }
    Q_PROPERTY(QColor grey_200 READ _grey_200 CONSTANT)
    QColor _grey_200() const { return Material::grey_200; }
    Q_PROPERTY(QColor grey_300 READ _grey_300 CONSTANT)
    QColor _grey_300() const { return Material::grey_300; }
    Q_PROPERTY(QColor grey_400 READ _grey_400 CONSTANT)
    QColor _grey_400() const { return Material::grey_400; }
    Q_PROPERTY(QColor grey_500 READ _grey_500 CONSTANT)
    QColor _grey_500() const { return Material::grey_500; }
    Q_PROPERTY(QColor grey_600 READ _grey_600 CONSTANT)
    QColor _grey_600() const { return Material::grey_600; }
    Q_PROPERTY(QColor grey_700 READ _grey_700 CONSTANT)
    QColor _grey_700() const { return Material::grey_700; }
    Q_PROPERTY(QColor grey_800 READ _grey_800 CONSTANT)
    QColor _grey_800() const { return Material::grey_800; }
    Q_PROPERTY(QColor grey_900 READ _grey_900 CONSTANT)
    QColor _grey_900() const { return Material::grey_900; }

/* Blue Grey */
    Q_PROPERTY(QColor blue_grey READ _blue_grey CONSTANT)
    QColor _blue_grey() const { return Material::blue_grey; }
    Q_PROPERTY(QColor blue_grey_50 READ _blue_grey_50 CONSTANT)
    QColor _blue_grey_50() const { return Material::blue_grey_50; }
    Q_PROPERTY(QColor blue_grey_100 READ _blue_grey_100 CONSTANT)
    QColor _blue_grey_100() const { return Material::blue_grey_100; }
    Q_PROPERTY(QColor blue_grey_200 READ _blue_grey_200 CONSTANT)
    QColor _blue_grey_200() const { return Material::blue_grey_200; }
    Q_PROPERTY(QColor blue_grey_300 READ _blue_grey_300 CONSTANT)
    QColor _blue_grey_300() const { return Material::blue_grey_300; }
    Q_PROPERTY(QColor blue_grey_400 READ _blue_grey_400 CONSTANT)
    QColor _blue_grey_400() const { return Material::blue_grey_400; }
    Q_PROPERTY(QColor blue_grey_500 READ _blue_grey_500 CONSTANT)
    QColor _blue_grey_500() const { return Material::blue_grey_500; }
    Q_PROPERTY(QColor blue_grey_600 READ _blue_grey_600 CONSTANT)
    QColor _blue_grey_600() const { return Material::blue_grey_600; }
    Q_PROPERTY(QColor blue_grey_700 READ _blue_grey_700 CONSTANT)
    QColor _blue_grey_700() const { return Material::blue_grey_700; }
    Q_PROPERTY(QColor blue_grey_800 READ _blue_grey_800 CONSTANT)
    QColor _blue_grey_800() const { return Material::blue_grey_800; }
    Q_PROPERTY(QColor blue_grey_900 READ _blue_grey_900 CONSTANT)
    QColor _blue_grey_900() const { return Material::blue_grey_900; }

/* White / Black */
    Q_PROPERTY(QColor white READ _white CONSTANT)
    QColor _white() const { return Material::white; }
    Q_PROPERTY(QColor black READ _black CONSTANT)
    QColor _black() const { return Material::black; }
};
