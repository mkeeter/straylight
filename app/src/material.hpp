#pragma once

#include <QObject>
#include <QColor>
#include <QJSEngine>
#include <QQmlEngine>

class Material: public QObject
{
    Q_OBJECT
public:
    /*
     *  Constructor for the QML singleton
     */
    static QObject* singleton(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_PROPERTY(QColor red MEMBER red CONSTANT)
    const static QColor red;
    Q_PROPERTY(QColor red_50 MEMBER red_50 CONSTANT)
    const static QColor red_50;
    Q_PROPERTY(QColor red_100 MEMBER red_100 CONSTANT)
    const static QColor red_100;
    Q_PROPERTY(QColor red_200 MEMBER red_200 CONSTANT)
    const static QColor red_200;
    Q_PROPERTY(QColor red_300 MEMBER red_300 CONSTANT)
    const static QColor red_300;
    Q_PROPERTY(QColor red_400 MEMBER red_400 CONSTANT)
    const static QColor red_400;
    Q_PROPERTY(QColor red_500 MEMBER red_500 CONSTANT)
    const static QColor red_500;
    Q_PROPERTY(QColor red_600 MEMBER red_600 CONSTANT)
    const static QColor red_600;
    Q_PROPERTY(QColor red_700 MEMBER red_700 CONSTANT)
    const static QColor red_700;
    Q_PROPERTY(QColor red_800 MEMBER red_800 CONSTANT)
    const static QColor red_800;
    Q_PROPERTY(QColor red_900 MEMBER red_900 CONSTANT)
    const static QColor red_900;
    Q_PROPERTY(QColor red_a100 MEMBER red_a100 CONSTANT)
    const static QColor red_a100;
    Q_PROPERTY(QColor red_a200 MEMBER red_a200 CONSTANT)
    const static QColor red_a200;
    Q_PROPERTY(QColor red_a400 MEMBER red_a400 CONSTANT)
    const static QColor red_a400;
    Q_PROPERTY(QColor red_a700 MEMBER red_a700 CONSTANT)
    const static QColor red_a700;

/* Pink */
    Q_PROPERTY(QColor pink MEMBER pink CONSTANT)
    const static QColor pink;
    Q_PROPERTY(QColor pink_50 MEMBER pink_50 CONSTANT)
    const static QColor pink_50;
    Q_PROPERTY(QColor pink_100 MEMBER pink_100 CONSTANT)
    const static QColor pink_100;
    Q_PROPERTY(QColor pink_200 MEMBER pink_200 CONSTANT)
    const static QColor pink_200;
    Q_PROPERTY(QColor pink_300 MEMBER pink_300 CONSTANT)
    const static QColor pink_300;
    Q_PROPERTY(QColor pink_400 MEMBER pink_400 CONSTANT)
    const static QColor pink_400;
    Q_PROPERTY(QColor pink_500 MEMBER pink_500 CONSTANT)
    const static QColor pink_500;
    Q_PROPERTY(QColor pink_600 MEMBER pink_600 CONSTANT)
    const static QColor pink_600;
    Q_PROPERTY(QColor pink_700 MEMBER pink_700 CONSTANT)
    const static QColor pink_700;
    Q_PROPERTY(QColor pink_800 MEMBER pink_800 CONSTANT)
    const static QColor pink_800;
    Q_PROPERTY(QColor pink_900 MEMBER pink_900 CONSTANT)
    const static QColor pink_900;
    Q_PROPERTY(QColor pink_a100 MEMBER pink_a100 CONSTANT)
    const static QColor pink_a100;
    Q_PROPERTY(QColor pink_a200 MEMBER pink_a200 CONSTANT)
    const static QColor pink_a200;
    Q_PROPERTY(QColor pink_a400 MEMBER pink_a400 CONSTANT)
    const static QColor pink_a400;
    Q_PROPERTY(QColor pink_a700 MEMBER pink_a700 CONSTANT)
    const static QColor pink_a700;

/* Purple */
    Q_PROPERTY(QColor purple MEMBER purple CONSTANT)
    const static QColor purple;
    Q_PROPERTY(QColor purple_50 MEMBER purple_50 CONSTANT)
    const static QColor purple_50;
    Q_PROPERTY(QColor purple_100 MEMBER purple_100 CONSTANT)
    const static QColor purple_100;
    Q_PROPERTY(QColor purple_200 MEMBER purple_200 CONSTANT)
    const static QColor purple_200;
    Q_PROPERTY(QColor purple_300 MEMBER purple_300 CONSTANT)
    const static QColor purple_300;
    Q_PROPERTY(QColor purple_400 MEMBER purple_400 CONSTANT)
    const static QColor purple_400;
    Q_PROPERTY(QColor purple_500 MEMBER purple_500 CONSTANT)
    const static QColor purple_500;
    Q_PROPERTY(QColor purple_600 MEMBER purple_600 CONSTANT)
    const static QColor purple_600;
    Q_PROPERTY(QColor purple_700 MEMBER purple_700 CONSTANT)
    const static QColor purple_700;
    Q_PROPERTY(QColor purple_800 MEMBER purple_800 CONSTANT)
    const static QColor purple_800;
    Q_PROPERTY(QColor purple_900 MEMBER purple_900 CONSTANT)
    const static QColor purple_900;
    Q_PROPERTY(QColor purple_a100 MEMBER purple_a100 CONSTANT)
    const static QColor purple_a100;
    Q_PROPERTY(QColor purple_a200 MEMBER purple_a200 CONSTANT)
    const static QColor purple_a200;
    Q_PROPERTY(QColor purple_a400 MEMBER purple_a400 CONSTANT)
    const static QColor purple_a400;
    Q_PROPERTY(QColor purple_a700 MEMBER purple_a700 CONSTANT)
    const static QColor purple_a700;

/* Deep Purple */
    Q_PROPERTY(QColor deep_purple MEMBER deep_purple CONSTANT)
    const static QColor deep_purple;
    Q_PROPERTY(QColor deep_purple_50 MEMBER deep_purple_50 CONSTANT)
    const static QColor deep_purple_50;
    Q_PROPERTY(QColor deep_purple_100 MEMBER deep_purple_100 CONSTANT)
    const static QColor deep_purple_100;
    Q_PROPERTY(QColor deep_purple_200 MEMBER deep_purple_200 CONSTANT)
    const static QColor deep_purple_200;
    Q_PROPERTY(QColor deep_purple_300 MEMBER deep_purple_300 CONSTANT)
    const static QColor deep_purple_300;
    Q_PROPERTY(QColor deep_purple_400 MEMBER deep_purple_400 CONSTANT)
    const static QColor deep_purple_400;
    Q_PROPERTY(QColor deep_purple_500 MEMBER deep_purple_500 CONSTANT)
    const static QColor deep_purple_500;
    Q_PROPERTY(QColor deep_purple_600 MEMBER deep_purple_600 CONSTANT)
    const static QColor deep_purple_600;
    Q_PROPERTY(QColor deep_purple_700 MEMBER deep_purple_700 CONSTANT)
    const static QColor deep_purple_700;
    Q_PROPERTY(QColor deep_purple_800 MEMBER deep_purple_800 CONSTANT)
    const static QColor deep_purple_800;
    Q_PROPERTY(QColor deep_purple_900 MEMBER deep_purple_900 CONSTANT)
    const static QColor deep_purple_900;
    Q_PROPERTY(QColor deep_purple_a100 MEMBER deep_purple_a100 CONSTANT)
    const static QColor deep_purple_a100;
    Q_PROPERTY(QColor deep_purple_a200 MEMBER deep_purple_a200 CONSTANT)
    const static QColor deep_purple_a200;
    Q_PROPERTY(QColor deep_purple_a400 MEMBER deep_purple_a400 CONSTANT)
    const static QColor deep_purple_a400;
    Q_PROPERTY(QColor deep_purple_a700 MEMBER deep_purple_a700 CONSTANT)
    const static QColor deep_purple_a700;

/* Indigo */
    Q_PROPERTY(QColor indigo MEMBER indigo CONSTANT)
    const static QColor indigo;
    Q_PROPERTY(QColor indigo_50 MEMBER indigo_50 CONSTANT)
    const static QColor indigo_50;
    Q_PROPERTY(QColor indigo_100 MEMBER indigo_100 CONSTANT)
    const static QColor indigo_100;
    Q_PROPERTY(QColor indigo_200 MEMBER indigo_200 CONSTANT)
    const static QColor indigo_200;
    Q_PROPERTY(QColor indigo_300 MEMBER indigo_300 CONSTANT)
    const static QColor indigo_300;
    Q_PROPERTY(QColor indigo_400 MEMBER indigo_400 CONSTANT)
    const static QColor indigo_400;
    Q_PROPERTY(QColor indigo_500 MEMBER indigo_500 CONSTANT)
    const static QColor indigo_500;
    Q_PROPERTY(QColor indigo_600 MEMBER indigo_600 CONSTANT)
    const static QColor indigo_600;
    Q_PROPERTY(QColor indigo_700 MEMBER indigo_700 CONSTANT)
    const static QColor indigo_700;
    Q_PROPERTY(QColor indigo_800 MEMBER indigo_800 CONSTANT)
    const static QColor indigo_800;
    Q_PROPERTY(QColor indigo_900 MEMBER indigo_900 CONSTANT)
    const static QColor indigo_900;
    Q_PROPERTY(QColor indigo_a100 MEMBER indigo_a100 CONSTANT)
    const static QColor indigo_a100;
    Q_PROPERTY(QColor indigo_a200 MEMBER indigo_a200 CONSTANT)
    const static QColor indigo_a200;
    Q_PROPERTY(QColor indigo_a400 MEMBER indigo_a400 CONSTANT)
    const static QColor indigo_a400;
    Q_PROPERTY(QColor indigo_a700 MEMBER indigo_a700 CONSTANT)
    const static QColor indigo_a700;

/* Blue */
    Q_PROPERTY(QColor blue MEMBER blue CONSTANT)
    const static QColor blue;
    Q_PROPERTY(QColor blue_50 MEMBER blue_50 CONSTANT)
    const static QColor blue_50;
    Q_PROPERTY(QColor blue_100 MEMBER blue_100 CONSTANT)
    const static QColor blue_100;
    Q_PROPERTY(QColor blue_200 MEMBER blue_200 CONSTANT)
    const static QColor blue_200;
    Q_PROPERTY(QColor blue_300 MEMBER blue_300 CONSTANT)
    const static QColor blue_300;
    Q_PROPERTY(QColor blue_400 MEMBER blue_400 CONSTANT)
    const static QColor blue_400;
    Q_PROPERTY(QColor blue_500 MEMBER blue_500 CONSTANT)
    const static QColor blue_500;
    Q_PROPERTY(QColor blue_600 MEMBER blue_600 CONSTANT)
    const static QColor blue_600;
    Q_PROPERTY(QColor blue_700 MEMBER blue_700 CONSTANT)
    const static QColor blue_700;
    Q_PROPERTY(QColor blue_800 MEMBER blue_800 CONSTANT)
    const static QColor blue_800;
    Q_PROPERTY(QColor blue_900 MEMBER blue_900 CONSTANT)
    const static QColor blue_900;
    Q_PROPERTY(QColor blue_a100 MEMBER blue_a100 CONSTANT)
    const static QColor blue_a100;
    Q_PROPERTY(QColor blue_a200 MEMBER blue_a200 CONSTANT)
    const static QColor blue_a200;
    Q_PROPERTY(QColor blue_a400 MEMBER blue_a400 CONSTANT)
    const static QColor blue_a400;
    Q_PROPERTY(QColor blue_a700 MEMBER blue_a700 CONSTANT)
    const static QColor blue_a700;

/* Light Blue */
    Q_PROPERTY(QColor light_blue MEMBER light_blue CONSTANT)
    const static QColor light_blue;
    Q_PROPERTY(QColor light_blue_50 MEMBER light_blue_50 CONSTANT)
    const static QColor light_blue_50;
    Q_PROPERTY(QColor light_blue_100 MEMBER light_blue_100 CONSTANT)
    const static QColor light_blue_100;
    Q_PROPERTY(QColor light_blue_200 MEMBER light_blue_200 CONSTANT)
    const static QColor light_blue_200;
    Q_PROPERTY(QColor light_blue_300 MEMBER light_blue_300 CONSTANT)
    const static QColor light_blue_300;
    Q_PROPERTY(QColor light_blue_400 MEMBER light_blue_400 CONSTANT)
    const static QColor light_blue_400;
    Q_PROPERTY(QColor light_blue_500 MEMBER light_blue_500 CONSTANT)
    const static QColor light_blue_500;
    Q_PROPERTY(QColor light_blue_600 MEMBER light_blue_600 CONSTANT)
    const static QColor light_blue_600;
    Q_PROPERTY(QColor light_blue_700 MEMBER light_blue_700 CONSTANT)
    const static QColor light_blue_700;
    Q_PROPERTY(QColor light_blue_800 MEMBER light_blue_800 CONSTANT)
    const static QColor light_blue_800;
    Q_PROPERTY(QColor light_blue_900 MEMBER light_blue_900 CONSTANT)
    const static QColor light_blue_900;
    Q_PROPERTY(QColor light_blue_a100 MEMBER light_blue_a100 CONSTANT)
    const static QColor light_blue_a100;
    Q_PROPERTY(QColor light_blue_a200 MEMBER light_blue_a200 CONSTANT)
    const static QColor light_blue_a200;
    Q_PROPERTY(QColor light_blue_a400 MEMBER light_blue_a400 CONSTANT)
    const static QColor light_blue_a400;
    Q_PROPERTY(QColor light_blue_a700 MEMBER light_blue_a700 CONSTANT)
    const static QColor light_blue_a700;

/* Cyan */
    Q_PROPERTY(QColor cyan MEMBER cyan CONSTANT)
    const static QColor cyan;
    Q_PROPERTY(QColor cyan_50 MEMBER cyan_50 CONSTANT)
    const static QColor cyan_50;
    Q_PROPERTY(QColor cyan_100 MEMBER cyan_100 CONSTANT)
    const static QColor cyan_100;
    Q_PROPERTY(QColor cyan_200 MEMBER cyan_200 CONSTANT)
    const static QColor cyan_200;
    Q_PROPERTY(QColor cyan_300 MEMBER cyan_300 CONSTANT)
    const static QColor cyan_300;
    Q_PROPERTY(QColor cyan_400 MEMBER cyan_400 CONSTANT)
    const static QColor cyan_400;
    Q_PROPERTY(QColor cyan_500 MEMBER cyan_500 CONSTANT)
    const static QColor cyan_500;
    Q_PROPERTY(QColor cyan_600 MEMBER cyan_600 CONSTANT)
    const static QColor cyan_600;
    Q_PROPERTY(QColor cyan_700 MEMBER cyan_700 CONSTANT)
    const static QColor cyan_700;
    Q_PROPERTY(QColor cyan_800 MEMBER cyan_800 CONSTANT)
    const static QColor cyan_800;
    Q_PROPERTY(QColor cyan_900 MEMBER cyan_900 CONSTANT)
    const static QColor cyan_900;
    Q_PROPERTY(QColor cyan_a100 MEMBER cyan_a100 CONSTANT)
    const static QColor cyan_a100;
    Q_PROPERTY(QColor cyan_a200 MEMBER cyan_a200 CONSTANT)
    const static QColor cyan_a200;
    Q_PROPERTY(QColor cyan_a400 MEMBER cyan_a400 CONSTANT)
    const static QColor cyan_a400;
    Q_PROPERTY(QColor cyan_a700 MEMBER cyan_a700 CONSTANT)
    const static QColor cyan_a700;

/* Teal */
    Q_PROPERTY(QColor teal MEMBER teal CONSTANT)
    const static QColor teal;
    Q_PROPERTY(QColor teal_50 MEMBER teal_50 CONSTANT)
    const static QColor teal_50;
    Q_PROPERTY(QColor teal_100 MEMBER teal_100 CONSTANT)
    const static QColor teal_100;
    Q_PROPERTY(QColor teal_200 MEMBER teal_200 CONSTANT)
    const static QColor teal_200;
    Q_PROPERTY(QColor teal_300 MEMBER teal_300 CONSTANT)
    const static QColor teal_300;
    Q_PROPERTY(QColor teal_400 MEMBER teal_400 CONSTANT)
    const static QColor teal_400;
    Q_PROPERTY(QColor teal_500 MEMBER teal_500 CONSTANT)
    const static QColor teal_500;
    Q_PROPERTY(QColor teal_600 MEMBER teal_600 CONSTANT)
    const static QColor teal_600;
    Q_PROPERTY(QColor teal_700 MEMBER teal_700 CONSTANT)
    const static QColor teal_700;
    Q_PROPERTY(QColor teal_800 MEMBER teal_800 CONSTANT)
    const static QColor teal_800;
    Q_PROPERTY(QColor teal_900 MEMBER teal_900 CONSTANT)
    const static QColor teal_900;
    Q_PROPERTY(QColor teal_a100 MEMBER teal_a100 CONSTANT)
    const static QColor teal_a100;
    Q_PROPERTY(QColor teal_a200 MEMBER teal_a200 CONSTANT)
    const static QColor teal_a200;
    Q_PROPERTY(QColor teal_a400 MEMBER teal_a400 CONSTANT)
    const static QColor teal_a400;
    Q_PROPERTY(QColor teal_a700 MEMBER teal_a700 CONSTANT)
    const static QColor teal_a700;

/* Green */
    Q_PROPERTY(QColor green MEMBER green CONSTANT)
    const static QColor green;
    Q_PROPERTY(QColor green_50 MEMBER green_50 CONSTANT)
    const static QColor green_50;
    Q_PROPERTY(QColor green_100 MEMBER green_100 CONSTANT)
    const static QColor green_100;
    Q_PROPERTY(QColor green_200 MEMBER green_200 CONSTANT)
    const static QColor green_200;
    Q_PROPERTY(QColor green_300 MEMBER green_300 CONSTANT)
    const static QColor green_300;
    Q_PROPERTY(QColor green_400 MEMBER green_400 CONSTANT)
    const static QColor green_400;
    Q_PROPERTY(QColor green_500 MEMBER green_500 CONSTANT)
    const static QColor green_500;
    Q_PROPERTY(QColor green_600 MEMBER green_600 CONSTANT)
    const static QColor green_600;
    Q_PROPERTY(QColor green_700 MEMBER green_700 CONSTANT)
    const static QColor green_700;
    Q_PROPERTY(QColor green_800 MEMBER green_800 CONSTANT)
    const static QColor green_800;
    Q_PROPERTY(QColor green_900 MEMBER green_900 CONSTANT)
    const static QColor green_900;
    Q_PROPERTY(QColor green_a100 MEMBER green_a100 CONSTANT)
    const static QColor green_a100;
    Q_PROPERTY(QColor green_a200 MEMBER green_a200 CONSTANT)
    const static QColor green_a200;
    Q_PROPERTY(QColor green_a400 MEMBER green_a400 CONSTANT)
    const static QColor green_a400;
    Q_PROPERTY(QColor green_a700 MEMBER green_a700 CONSTANT)
    const static QColor green_a700;

/* Light Green */
    Q_PROPERTY(QColor light_green MEMBER light_green CONSTANT)
    const static QColor light_green;
    Q_PROPERTY(QColor light_green_50 MEMBER light_green_50 CONSTANT)
    const static QColor light_green_50;
    Q_PROPERTY(QColor light_green_100 MEMBER light_green_100 CONSTANT)
    const static QColor light_green_100;
    Q_PROPERTY(QColor light_green_200 MEMBER light_green_200 CONSTANT)
    const static QColor light_green_200;
    Q_PROPERTY(QColor light_green_300 MEMBER light_green_300 CONSTANT)
    const static QColor light_green_300;
    Q_PROPERTY(QColor light_green_400 MEMBER light_green_400 CONSTANT)
    const static QColor light_green_400;
    Q_PROPERTY(QColor light_green_500 MEMBER light_green_500 CONSTANT)
    const static QColor light_green_500;
    Q_PROPERTY(QColor light_green_600 MEMBER light_green_600 CONSTANT)
    const static QColor light_green_600;
    Q_PROPERTY(QColor light_green_700 MEMBER light_green_700 CONSTANT)
    const static QColor light_green_700;
    Q_PROPERTY(QColor light_green_800 MEMBER light_green_800 CONSTANT)
    const static QColor light_green_800;
    Q_PROPERTY(QColor light_green_900 MEMBER light_green_900 CONSTANT)
    const static QColor light_green_900;
    Q_PROPERTY(QColor light_green_a100 MEMBER light_green_a100 CONSTANT)
    const static QColor light_green_a100;
    Q_PROPERTY(QColor light_green_a200 MEMBER light_green_a200 CONSTANT)
    const static QColor light_green_a200;
    Q_PROPERTY(QColor light_green_a400 MEMBER light_green_a400 CONSTANT)
    const static QColor light_green_a400;
    Q_PROPERTY(QColor light_green_a700 MEMBER light_green_a700 CONSTANT)
    const static QColor light_green_a700;

/* Lime */
    Q_PROPERTY(QColor lime MEMBER lime CONSTANT)
    const static QColor lime;
    Q_PROPERTY(QColor lime_50 MEMBER lime_50 CONSTANT)
    const static QColor lime_50;
    Q_PROPERTY(QColor lime_100 MEMBER lime_100 CONSTANT)
    const static QColor lime_100;
    Q_PROPERTY(QColor lime_200 MEMBER lime_200 CONSTANT)
    const static QColor lime_200;
    Q_PROPERTY(QColor lime_300 MEMBER lime_300 CONSTANT)
    const static QColor lime_300;
    Q_PROPERTY(QColor lime_400 MEMBER lime_400 CONSTANT)
    const static QColor lime_400;
    Q_PROPERTY(QColor lime_500 MEMBER lime_500 CONSTANT)
    const static QColor lime_500;
    Q_PROPERTY(QColor lime_600 MEMBER lime_600 CONSTANT)
    const static QColor lime_600;
    Q_PROPERTY(QColor lime_700 MEMBER lime_700 CONSTANT)
    const static QColor lime_700;
    Q_PROPERTY(QColor lime_800 MEMBER lime_800 CONSTANT)
    const static QColor lime_800;
    Q_PROPERTY(QColor lime_900 MEMBER lime_900 CONSTANT)
    const static QColor lime_900;
    Q_PROPERTY(QColor lime_a100 MEMBER lime_a100 CONSTANT)
    const static QColor lime_a100;
    Q_PROPERTY(QColor lime_a200 MEMBER lime_a200 CONSTANT)
    const static QColor lime_a200;
    Q_PROPERTY(QColor lime_a400 MEMBER lime_a400 CONSTANT)
    const static QColor lime_a400;
    Q_PROPERTY(QColor lime_a700 MEMBER lime_a700 CONSTANT)
    const static QColor lime_a700;

/* Yellow */
    Q_PROPERTY(QColor yellow MEMBER yellow CONSTANT)
    const static QColor yellow;
    Q_PROPERTY(QColor yellow_50 MEMBER yellow_50 CONSTANT)
    const static QColor yellow_50;
    Q_PROPERTY(QColor yellow_100 MEMBER yellow_100 CONSTANT)
    const static QColor yellow_100;
    Q_PROPERTY(QColor yellow_200 MEMBER yellow_200 CONSTANT)
    const static QColor yellow_200;
    Q_PROPERTY(QColor yellow_300 MEMBER yellow_300 CONSTANT)
    const static QColor yellow_300;
    Q_PROPERTY(QColor yellow_400 MEMBER yellow_400 CONSTANT)
    const static QColor yellow_400;
    Q_PROPERTY(QColor yellow_500 MEMBER yellow_500 CONSTANT)
    const static QColor yellow_500;
    Q_PROPERTY(QColor yellow_600 MEMBER yellow_600 CONSTANT)
    const static QColor yellow_600;
    Q_PROPERTY(QColor yellow_700 MEMBER yellow_700 CONSTANT)
    const static QColor yellow_700;
    Q_PROPERTY(QColor yellow_800 MEMBER yellow_800 CONSTANT)
    const static QColor yellow_800;
    Q_PROPERTY(QColor yellow_900 MEMBER yellow_900 CONSTANT)
    const static QColor yellow_900;
    Q_PROPERTY(QColor yellow_a100 MEMBER yellow_a100 CONSTANT)
    const static QColor yellow_a100;
    Q_PROPERTY(QColor yellow_a200 MEMBER yellow_a200 CONSTANT)
    const static QColor yellow_a200;
    Q_PROPERTY(QColor yellow_a400 MEMBER yellow_a400 CONSTANT)
    const static QColor yellow_a400;
    Q_PROPERTY(QColor yellow_a700 MEMBER yellow_a700 CONSTANT)
    const static QColor yellow_a700;

/* Amber */
    Q_PROPERTY(QColor amber MEMBER amber CONSTANT)
    const static QColor amber;
    Q_PROPERTY(QColor amber_50 MEMBER amber_50 CONSTANT)
    const static QColor amber_50;
    Q_PROPERTY(QColor amber_100 MEMBER amber_100 CONSTANT)
    const static QColor amber_100;
    Q_PROPERTY(QColor amber_200 MEMBER amber_200 CONSTANT)
    const static QColor amber_200;
    Q_PROPERTY(QColor amber_300 MEMBER amber_300 CONSTANT)
    const static QColor amber_300;
    Q_PROPERTY(QColor amber_400 MEMBER amber_400 CONSTANT)
    const static QColor amber_400;
    Q_PROPERTY(QColor amber_500 MEMBER amber_500 CONSTANT)
    const static QColor amber_500;
    Q_PROPERTY(QColor amber_600 MEMBER amber_600 CONSTANT)
    const static QColor amber_600;
    Q_PROPERTY(QColor amber_700 MEMBER amber_700 CONSTANT)
    const static QColor amber_700;
    Q_PROPERTY(QColor amber_800 MEMBER amber_800 CONSTANT)
    const static QColor amber_800;
    Q_PROPERTY(QColor amber_900 MEMBER amber_900 CONSTANT)
    const static QColor amber_900;
    Q_PROPERTY(QColor amber_a100 MEMBER amber_a100 CONSTANT)
    const static QColor amber_a100;
    Q_PROPERTY(QColor amber_a200 MEMBER amber_a200 CONSTANT)
    const static QColor amber_a200;
    Q_PROPERTY(QColor amber_a400 MEMBER amber_a400 CONSTANT)
    const static QColor amber_a400;
    Q_PROPERTY(QColor amber_a700 MEMBER amber_a700 CONSTANT)
    const static QColor amber_a700;

/* Orange */
    Q_PROPERTY(QColor orange MEMBER orange CONSTANT)
    const static QColor orange;
    Q_PROPERTY(QColor orange_50 MEMBER orange_50 CONSTANT)
    const static QColor orange_50;
    Q_PROPERTY(QColor orange_100 MEMBER orange_100 CONSTANT)
    const static QColor orange_100;
    Q_PROPERTY(QColor orange_200 MEMBER orange_200 CONSTANT)
    const static QColor orange_200;
    Q_PROPERTY(QColor orange_300 MEMBER orange_300 CONSTANT)
    const static QColor orange_300;
    Q_PROPERTY(QColor orange_400 MEMBER orange_400 CONSTANT)
    const static QColor orange_400;
    Q_PROPERTY(QColor orange_500 MEMBER orange_500 CONSTANT)
    const static QColor orange_500;
    Q_PROPERTY(QColor orange_600 MEMBER orange_600 CONSTANT)
    const static QColor orange_600;
    Q_PROPERTY(QColor orange_700 MEMBER orange_700 CONSTANT)
    const static QColor orange_700;
    Q_PROPERTY(QColor orange_800 MEMBER orange_800 CONSTANT)
    const static QColor orange_800;
    Q_PROPERTY(QColor orange_900 MEMBER orange_900 CONSTANT)
    const static QColor orange_900;
    Q_PROPERTY(QColor orange_a100 MEMBER orange_a100 CONSTANT)
    const static QColor orange_a100;
    Q_PROPERTY(QColor orange_a200 MEMBER orange_a200 CONSTANT)
    const static QColor orange_a200;
    Q_PROPERTY(QColor orange_a400 MEMBER orange_a400 CONSTANT)
    const static QColor orange_a400;
    Q_PROPERTY(QColor orange_a700 MEMBER orange_a700 CONSTANT)
    const static QColor orange_a700;

/* Deep Orange */
    Q_PROPERTY(QColor deep_orange MEMBER deep_orange CONSTANT)
    const static QColor deep_orange;
    Q_PROPERTY(QColor deep_orange_50 MEMBER deep_orange_50 CONSTANT)
    const static QColor deep_orange_50;
    Q_PROPERTY(QColor deep_orange_100 MEMBER deep_orange_100 CONSTANT)
    const static QColor deep_orange_100;
    Q_PROPERTY(QColor deep_orange_200 MEMBER deep_orange_200 CONSTANT)
    const static QColor deep_orange_200;
    Q_PROPERTY(QColor deep_orange_300 MEMBER deep_orange_300 CONSTANT)
    const static QColor deep_orange_300;
    Q_PROPERTY(QColor deep_orange_400 MEMBER deep_orange_400 CONSTANT)
    const static QColor deep_orange_400;
    Q_PROPERTY(QColor deep_orange_500 MEMBER deep_orange_500 CONSTANT)
    const static QColor deep_orange_500;
    Q_PROPERTY(QColor deep_orange_600 MEMBER deep_orange_600 CONSTANT)
    const static QColor deep_orange_600;
    Q_PROPERTY(QColor deep_orange_700 MEMBER deep_orange_700 CONSTANT)
    const static QColor deep_orange_700;
    Q_PROPERTY(QColor deep_orange_800 MEMBER deep_orange_800 CONSTANT)
    const static QColor deep_orange_800;
    Q_PROPERTY(QColor deep_orange_900 MEMBER deep_orange_900 CONSTANT)
    const static QColor deep_orange_900;
    Q_PROPERTY(QColor deep_orange_a100 MEMBER deep_orange_a100 CONSTANT)
    const static QColor deep_orange_a100;
    Q_PROPERTY(QColor deep_orange_a200 MEMBER deep_orange_a200 CONSTANT)
    const static QColor deep_orange_a200;
    Q_PROPERTY(QColor deep_orange_a400 MEMBER deep_orange_a400 CONSTANT)
    const static QColor deep_orange_a400;
    Q_PROPERTY(QColor deep_orange_a700 MEMBER deep_orange_a700 CONSTANT)
    const static QColor deep_orange_a700;

/* Brown */
    Q_PROPERTY(QColor brown MEMBER brown CONSTANT)
    const static QColor brown;
    Q_PROPERTY(QColor brown_50 MEMBER brown_50 CONSTANT)
    const static QColor brown_50;
    Q_PROPERTY(QColor brown_100 MEMBER brown_100 CONSTANT)
    const static QColor brown_100;
    Q_PROPERTY(QColor brown_200 MEMBER brown_200 CONSTANT)
    const static QColor brown_200;
    Q_PROPERTY(QColor brown_300 MEMBER brown_300 CONSTANT)
    const static QColor brown_300;
    Q_PROPERTY(QColor brown_400 MEMBER brown_400 CONSTANT)
    const static QColor brown_400;
    Q_PROPERTY(QColor brown_500 MEMBER brown_500 CONSTANT)
    const static QColor brown_500;
    Q_PROPERTY(QColor brown_600 MEMBER brown_600 CONSTANT)
    const static QColor brown_600;
    Q_PROPERTY(QColor brown_700 MEMBER brown_700 CONSTANT)
    const static QColor brown_700;
    Q_PROPERTY(QColor brown_800 MEMBER brown_800 CONSTANT)
    const static QColor brown_800;
    Q_PROPERTY(QColor brown_900 MEMBER brown_900 CONSTANT)
    const static QColor brown_900;

/* Grey */
    Q_PROPERTY(QColor grey MEMBER grey CONSTANT)
    const static QColor grey;
    Q_PROPERTY(QColor grey_50 MEMBER grey_50 CONSTANT)
    const static QColor grey_50;
    Q_PROPERTY(QColor grey_100 MEMBER grey_100 CONSTANT)
    const static QColor grey_100;
    Q_PROPERTY(QColor grey_200 MEMBER grey_200 CONSTANT)
    const static QColor grey_200;
    Q_PROPERTY(QColor grey_300 MEMBER grey_300 CONSTANT)
    const static QColor grey_300;
    Q_PROPERTY(QColor grey_400 MEMBER grey_400 CONSTANT)
    const static QColor grey_400;
    Q_PROPERTY(QColor grey_500 MEMBER grey_500 CONSTANT)
    const static QColor grey_500;
    Q_PROPERTY(QColor grey_600 MEMBER grey_600 CONSTANT)
    const static QColor grey_600;
    Q_PROPERTY(QColor grey_700 MEMBER grey_700 CONSTANT)
    const static QColor grey_700;
    Q_PROPERTY(QColor grey_800 MEMBER grey_800 CONSTANT)
    const static QColor grey_800;
    Q_PROPERTY(QColor grey_900 MEMBER grey_900 CONSTANT)
    const static QColor grey_900;

/* Blue Grey */
    Q_PROPERTY(QColor blue_grey MEMBER blue_grey CONSTANT)
    const static QColor blue_grey;
    Q_PROPERTY(QColor blue_grey_50 MEMBER blue_grey_50 CONSTANT)
    const static QColor blue_grey_50;
    Q_PROPERTY(QColor blue_grey_100 MEMBER blue_grey_100 CONSTANT)
    const static QColor blue_grey_100;
    Q_PROPERTY(QColor blue_grey_200 MEMBER blue_grey_200 CONSTANT)
    const static QColor blue_grey_200;
    Q_PROPERTY(QColor blue_grey_300 MEMBER blue_grey_300 CONSTANT)
    const static QColor blue_grey_300;
    Q_PROPERTY(QColor blue_grey_400 MEMBER blue_grey_400 CONSTANT)
    const static QColor blue_grey_400;
    Q_PROPERTY(QColor blue_grey_500 MEMBER blue_grey_500 CONSTANT)
    const static QColor blue_grey_500;
    Q_PROPERTY(QColor blue_grey_600 MEMBER blue_grey_600 CONSTANT)
    const static QColor blue_grey_600;
    Q_PROPERTY(QColor blue_grey_700 MEMBER blue_grey_700 CONSTANT)
    const static QColor blue_grey_700;
    Q_PROPERTY(QColor blue_grey_800 MEMBER blue_grey_800 CONSTANT)
    const static QColor blue_grey_800;
    Q_PROPERTY(QColor blue_grey_900 MEMBER blue_grey_900 CONSTANT)
    const static QColor blue_grey_900;

/* White / Black */
    Q_PROPERTY(QColor white MEMBER white CONSTANT)
    const static QColor white;
    Q_PROPERTY(QColor black MEMBER black CONSTANT)
    const static QColor black;
};
