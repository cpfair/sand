// Rather than compute the fallback during code generation
// This will allow us to continue to use GColorWhatever on Aplite without worrying about the B&W fallback
// The logic is: lightness > 0.5 ? GColorWhite : GColorBlack
#ifdef PBL_BW
#define GColorOxfordBlue GColorBlack
#define GColorDukeBlue GColorBlack
#define GColorBlue GColorBlack
#define GColorDarkGreen GColorBlack
#define GColorMidnightGreen GColorBlack
#define GColorCobaltBlue GColorBlack
#define GColorBlueMoon GColorBlack
#define GColorIslamicGreen GColorBlack
#define GColorJaegerGreen GColorBlack
#define GColorTiffanyBlue GColorBlack
#define GColorVividCerulean GColorBlack
#define GColorGreen GColorBlack
#define GColorMalachite GColorBlack
#define GColorMediumSpringGreen GColorBlack
#define GColorCyan GColorBlack
#define GColorBulgarianRose GColorBlack
#define GColorImperialPurple GColorBlack
#define GColorIndigo GColorBlack
#define GColorElectricUltramarine GColorBlack
#define GColorArmyGreen GColorBlack
#define GColorLiberty GColorBlack
#define GColorVeryLightBlue GColorWhite
#define GColorKellyGreen GColorBlack
#define GColorMayGreen GColorBlack
#define GColorCadetBlue GColorBlack
#define GColorPictonBlue GColorWhite
#define GColorBrightGreen GColorBlack
#define GColorScreaminGreen GColorWhite
#define GColorMediumAquamarine GColorWhite
#define GColorElectricBlue GColorWhite
#define GColorDarkCandyAppleRed GColorBlack
#define GColorJazzberryJam GColorBlack
#define GColorPurple GColorBlack
#define GColorVividViolet GColorBlack
#define GColorWindsorTan GColorBlack
#define GColorRoseVale GColorBlack
#define GColorPurpureus GColorBlack
#define GColorLavenderIndigo GColorWhite
#define GColorLimerick GColorBlack
#define GColorBrass GColorBlack
#define GColorBabyBlueEyes GColorWhite
#define GColorSpringBud GColorBlack
#define GColorInchworm GColorWhite
#define GColorMintGreen GColorWhite
#define GColorCeleste GColorWhite
#define GColorRed GColorBlack
#define GColorFolly GColorBlack
#define GColorFashionMagenta GColorBlack
#define GColorMagenta GColorBlack
#define GColorOrange GColorBlack
#define GColorSunsetOrange GColorWhite
#define GColorBrilliantRose GColorWhite
#define GColorShockingPink GColorWhite
#define GColorChromeYellow GColorBlack
#define GColorRajah GColorWhite
#define GColorMelon GColorWhite
#define GColorRichBrilliantLavender GColorWhite
#define GColorYellow GColorBlack
#define GColorIcterine GColorWhite
#define GColorPastelYellow GColorWhite
#define GColorLightGray GColorWhite
#define GColorDarkGray GColorBlack
#endif
