#include "material.hpp"


QObject* MaterialSingleton::singleton(QQmlEngine *engine,
                                      QJSEngine *scriptEngine)
{
    (void)engine;
    (void)scriptEngine;
    return new MaterialSingleton();
}

/* Red */
const QColor Material::red("#f44336");
const QColor Material::red_50("#ffebee");
const QColor Material::red_100("#ffcdd2");
const QColor Material::red_200("#ef9a9a");
const QColor Material::red_300("#e57373");
const QColor Material::red_400("#ef5350");
const QColor Material::red_500("#f44336");
const QColor Material::red_600("#e53935");
const QColor Material::red_700("#d32f2f");
const QColor Material::red_800("#c62828");
const QColor Material::red_900("#b71c1c");
const QColor Material::red_a100("#ff8a80");
const QColor Material::red_a200("#ff5252");
const QColor Material::red_a400("#ff1744");
const QColor Material::red_a700("#d50000");

/* Pink */
const QColor Material::pink("#e91e63");
const QColor Material::pink_50("#fce4ec");
const QColor Material::pink_100("#f8bbd0");
const QColor Material::pink_200("#f48fb1");
const QColor Material::pink_300("#f06292");
const QColor Material::pink_400("#ec407a");
const QColor Material::pink_500("#e91e63");
const QColor Material::pink_600("#d81b60");
const QColor Material::pink_700("#c2185b");
const QColor Material::pink_800("#ad1457");
const QColor Material::pink_900("#880e4f");
const QColor Material::pink_a100("#ff80ab");
const QColor Material::pink_a200("#ff4081");
const QColor Material::pink_a400("#f50057");
const QColor Material::pink_a700("#c51162");

/* Purple */
const QColor Material::purple("#9c27b0");
const QColor Material::purple_50("#f3e5f5");
const QColor Material::purple_100("#e1bee7");
const QColor Material::purple_200("#ce93d8");
const QColor Material::purple_300("#ba68c8");
const QColor Material::purple_400("#ab47bc");
const QColor Material::purple_500("#9c27b0");
const QColor Material::purple_600("#8e24aa");
const QColor Material::purple_700("#7b1fa2");
const QColor Material::purple_800("#6a1b9a");
const QColor Material::purple_900("#4a148c");
const QColor Material::purple_a100("#ea80fc");
const QColor Material::purple_a200("#e040fb");
const QColor Material::purple_a400("#d500f9");
const QColor Material::purple_a700("#aa00ff");

/* Deep Purple */
const QColor Material::deep_purple("#673ab7");
const QColor Material::deep_purple_50("#ede7f6");
const QColor Material::deep_purple_100("#d1c4e9");
const QColor Material::deep_purple_200("#b39ddb");
const QColor Material::deep_purple_300("#9575cd");
const QColor Material::deep_purple_400("#7e57c2");
const QColor Material::deep_purple_500("#673ab7");
const QColor Material::deep_purple_600("#5e35b1");
const QColor Material::deep_purple_700("#512da8");
const QColor Material::deep_purple_800("#4527a0");
const QColor Material::deep_purple_900("#311b92");
const QColor Material::deep_purple_a100("#b388ff");
const QColor Material::deep_purple_a200("#7c4dff");
const QColor Material::deep_purple_a400("#651fff");
const QColor Material::deep_purple_a700("#6200ea");

/* Indigo */
const QColor Material::indigo("#3f51b5");
const QColor Material::indigo_50("#e8eaf6");
const QColor Material::indigo_100("#c5cae9");
const QColor Material::indigo_200("#9fa8da");
const QColor Material::indigo_300("#7986cb");
const QColor Material::indigo_400("#5c6bc0");
const QColor Material::indigo_500("#3f51b5");
const QColor Material::indigo_600("#3949ab");
const QColor Material::indigo_700("#303f9f");
const QColor Material::indigo_800("#283593");
const QColor Material::indigo_900("#1a237e");
const QColor Material::indigo_a100("#8c9eff");
const QColor Material::indigo_a200("#536dfe");
const QColor Material::indigo_a400("#3d5afe");
const QColor Material::indigo_a700("#304ffe");

/* Blue */
const QColor Material::blue("#2196f3");
const QColor Material::blue_50("#e3f2fd");
const QColor Material::blue_100("#bbdefb");
const QColor Material::blue_200("#90caf9");
const QColor Material::blue_300("#64b5f6");
const QColor Material::blue_400("#42a5f5");
const QColor Material::blue_500("#2196f3");
const QColor Material::blue_600("#1e88e5");
const QColor Material::blue_700("#1976d2");
const QColor Material::blue_800("#1565c0");
const QColor Material::blue_900("#0d47a1");
const QColor Material::blue_a100("#82b1ff");
const QColor Material::blue_a200("#448aff");
const QColor Material::blue_a400("#2979ff");
const QColor Material::blue_a700("#2962ff");

/* Light Blue */
const QColor Material::light_blue("#03a9f4");
const QColor Material::light_blue_50("#e1f5fe");
const QColor Material::light_blue_100("#b3e5fc");
const QColor Material::light_blue_200("#81d4fa");
const QColor Material::light_blue_300("#4fc3f7");
const QColor Material::light_blue_400("#29b6f6");
const QColor Material::light_blue_500("#03a9f4");
const QColor Material::light_blue_600("#039be5");
const QColor Material::light_blue_700("#0288d1");
const QColor Material::light_blue_800("#0277bd");
const QColor Material::light_blue_900("#01579b");
const QColor Material::light_blue_a100("#80d8ff");
const QColor Material::light_blue_a200("#40c4ff");
const QColor Material::light_blue_a400("#00b0ff");
const QColor Material::light_blue_a700("#0091ea");

/* Cyan */
const QColor Material::cyan("#00bcd4");
const QColor Material::cyan_50("#e0f7fa");
const QColor Material::cyan_100("#b2ebf2");
const QColor Material::cyan_200("#80deea");
const QColor Material::cyan_300("#4dd0e1");
const QColor Material::cyan_400("#26c6da");
const QColor Material::cyan_500("#00bcd4");
const QColor Material::cyan_600("#00acc1");
const QColor Material::cyan_700("#0097a7");
const QColor Material::cyan_800("#00838f");
const QColor Material::cyan_900("#006064");
const QColor Material::cyan_a100("#84ffff");
const QColor Material::cyan_a200("#18ffff");
const QColor Material::cyan_a400("#00e5ff");
const QColor Material::cyan_a700("#00b8d4");

/* Teal */
const QColor Material::teal("#009688");
const QColor Material::teal_50("#e0f2f1");
const QColor Material::teal_100("#b2dfdb");
const QColor Material::teal_200("#80cbc4");
const QColor Material::teal_300("#4db6ac");
const QColor Material::teal_400("#26a69a");
const QColor Material::teal_500("#009688");
const QColor Material::teal_600("#00897b");
const QColor Material::teal_700("#00796b");
const QColor Material::teal_800("#00695c");
const QColor Material::teal_900("#004d40");
const QColor Material::teal_a100("#a7ffeb");
const QColor Material::teal_a200("#64ffda");
const QColor Material::teal_a400("#1de9b6");
const QColor Material::teal_a700("#00bfa5");

/* Green */
const QColor Material::green("#4caf50");
const QColor Material::green_50("#e8f5e9");
const QColor Material::green_100("#c8e6c9");
const QColor Material::green_200("#a5d6a7");
const QColor Material::green_300("#81c784");
const QColor Material::green_400("#66bb6a");
const QColor Material::green_500("#4caf50");
const QColor Material::green_600("#43a047");
const QColor Material::green_700("#388e3c");
const QColor Material::green_800("#2e7d32");
const QColor Material::green_900("#1b5e20");
const QColor Material::green_a100("#b9f6ca");
const QColor Material::green_a200("#69f0ae");
const QColor Material::green_a400("#00e676");
const QColor Material::green_a700("#00c853");

/* Light Green */
const QColor Material::light_green("#8bc34a");
const QColor Material::light_green_50("#f1f8e9");
const QColor Material::light_green_100("#dcedc8");
const QColor Material::light_green_200("#c5e1a5");
const QColor Material::light_green_300("#aed581");
const QColor Material::light_green_400("#9ccc65");
const QColor Material::light_green_500("#8bc34a");
const QColor Material::light_green_600("#7cb342");
const QColor Material::light_green_700("#689f38");
const QColor Material::light_green_800("#558b2f");
const QColor Material::light_green_900("#33691e");
const QColor Material::light_green_a100("#ccff90");
const QColor Material::light_green_a200("#b2ff59");
const QColor Material::light_green_a400("#76ff03");
const QColor Material::light_green_a700("#64dd17");

/* Lime */
const QColor Material::lime("#cddc39");
const QColor Material::lime_50("#f9fbe7");
const QColor Material::lime_100("#f0f4c3");
const QColor Material::lime_200("#e6ee9c");
const QColor Material::lime_300("#dce775");
const QColor Material::lime_400("#d4e157");
const QColor Material::lime_500("#cddc39");
const QColor Material::lime_600("#c0ca33");
const QColor Material::lime_700("#afb42b");
const QColor Material::lime_800("#9e9d24");
const QColor Material::lime_900("#827717");
const QColor Material::lime_a100("#f4ff81");
const QColor Material::lime_a200("#eeff41");
const QColor Material::lime_a400("#c6ff00");
const QColor Material::lime_a700("#aeea00");

/* Yellow */
const QColor Material::yellow("#ffeb3b");
const QColor Material::yellow_50("#fffde7");
const QColor Material::yellow_100("#fff9c4");
const QColor Material::yellow_200("#fff59d");
const QColor Material::yellow_300("#fff176");
const QColor Material::yellow_400("#ffee58");
const QColor Material::yellow_500("#ffeb3b");
const QColor Material::yellow_600("#fdd835");
const QColor Material::yellow_700("#fbc02d");
const QColor Material::yellow_800("#f9a825");
const QColor Material::yellow_900("#f57f17");
const QColor Material::yellow_a100("#ffff8d");
const QColor Material::yellow_a200("#ffff00");
const QColor Material::yellow_a400("#ffea00");
const QColor Material::yellow_a700("#ffd600");

/* Amber */
const QColor Material::amber("#ffc107");
const QColor Material::amber_50("#fff8e1");
const QColor Material::amber_100("#ffecb3");
const QColor Material::amber_200("#ffe082");
const QColor Material::amber_300("#ffd54f");
const QColor Material::amber_400("#ffca28");
const QColor Material::amber_500("#ffc107");
const QColor Material::amber_600("#ffb300");
const QColor Material::amber_700("#ffa000");
const QColor Material::amber_800("#ff8f00");
const QColor Material::amber_900("#ff6f00");
const QColor Material::amber_a100("#ffe57f");
const QColor Material::amber_a200("#ffd740");
const QColor Material::amber_a400("#ffc400");
const QColor Material::amber_a700("#ffab00");

/* Orange */
const QColor Material::orange("#ff9800");
const QColor Material::orange_50("#fff3e0");
const QColor Material::orange_100("#ffe0b2");
const QColor Material::orange_200("#ffcc80");
const QColor Material::orange_300("#ffb74d");
const QColor Material::orange_400("#ffa726");
const QColor Material::orange_500("#ff9800");
const QColor Material::orange_600("#fb8c00");
const QColor Material::orange_700("#f57c00");
const QColor Material::orange_800("#ef6c00");
const QColor Material::orange_900("#e65100");
const QColor Material::orange_a100("#ffd180");
const QColor Material::orange_a200("#ffab40");
const QColor Material::orange_a400("#ff9100");
const QColor Material::orange_a700("#ff6d00");

/* Deep Orange */
const QColor Material::deep_orange("#ff5722");
const QColor Material::deep_orange_50("#fbe9e7");
const QColor Material::deep_orange_100("#ffccbc");
const QColor Material::deep_orange_200("#ffab91");
const QColor Material::deep_orange_300("#ff8a65");
const QColor Material::deep_orange_400("#ff7043");
const QColor Material::deep_orange_500("#ff5722");
const QColor Material::deep_orange_600("#f4511e");
const QColor Material::deep_orange_700("#e64a19");
const QColor Material::deep_orange_800("#d84315");
const QColor Material::deep_orange_900("#bf360c");
const QColor Material::deep_orange_a100("#ff9e80");
const QColor Material::deep_orange_a200("#ff6e40");
const QColor Material::deep_orange_a400("#ff3d00");
const QColor Material::deep_orange_a700("#dd2c00");

/* Brown */
const QColor Material::brown("#795548");
const QColor Material::brown_50("#efebe9");
const QColor Material::brown_100("#d7ccc8");
const QColor Material::brown_200("#bcaaa4");
const QColor Material::brown_300("#a1887f");
const QColor Material::brown_400("#8d6e63");
const QColor Material::brown_500("#795548");
const QColor Material::brown_600("#6d4c41");
const QColor Material::brown_700("#5d4037");
const QColor Material::brown_800("#4e342e");
const QColor Material::brown_900("#3e2723");

/* Grey */
const QColor Material::grey("#9e9e9e");
const QColor Material::grey_50("#fafafa");
const QColor Material::grey_100("#f5f5f5");
const QColor Material::grey_200("#eeeeee");
const QColor Material::grey_300("#e0e0e0");
const QColor Material::grey_400("#bdbdbd");
const QColor Material::grey_500("#9e9e9e");
const QColor Material::grey_600("#757575");
const QColor Material::grey_700("#616161");
const QColor Material::grey_800("#424242");
const QColor Material::grey_900("#212121");

/* Blue Grey */
const QColor Material::blue_grey("#607d8b");
const QColor Material::blue_grey_50("#eceff1");
const QColor Material::blue_grey_100("#cfd8dc");
const QColor Material::blue_grey_200("#b0bec5");
const QColor Material::blue_grey_300("#90a4ae");
const QColor Material::blue_grey_400("#78909c");
const QColor Material::blue_grey_500("#607d8b");
const QColor Material::blue_grey_600("#546e7a");
const QColor Material::blue_grey_700("#455a64");
const QColor Material::blue_grey_800("#37474f");
const QColor Material::blue_grey_900("#263238");

/* White / Black */
const QColor Material::white("#ffffff");
const QColor Material::black("#000000");
