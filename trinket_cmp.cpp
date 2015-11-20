/*
    Copyright (C) 2015 Aean(a.k.a. fhsvengetta)
    All rights reserved.

    IreCore is distributed under the terms of The MIT License.
    You should have received a copy of the MIT License along with this program.
    If not, see <http://opensource.org/licenses/mit-license.php>.
*/

#include "irecore.h"
#include "gic.h"

struct trinket_profile_t {
    std::string name;
    int itemid;
    int itemlvl;
    int str;
    int crit;
    int haste;
    int mastery;
    int mult;
    int vers;
    int demonic;
    int upgrade;
    float dps;
    int not_selected;
    bool operator<( const trinket_profile_t& rhs ) {
        return dps > rhs.dps;
    }
    void eval() {
        if(not_selected) return;
        trinket1 = trinket_from_id( itemid );
        trinket1_ilvl = itemlvl;
        trinket2 = 0;
        if(demonic>=0) enemy_is_demonic = demonic;
        stat_array.clear();
        stat_array.push_back( current_stat );
        stat_array[0].gear_str += str;
        stat_array[0].gear_crit += crit;
        stat_array[0].gear_haste += haste;
        stat_array[0].gear_mastery += mastery;
        stat_array[0].gear_mult += mult;
        stat_array[0].gear_vers += vers;
        stat_array[0].name = name;
        parameters_consistency();
        auto_apl();
        generate_predef();
        ocl().run( apl, predef );
        dps = stat_array[0].dps;
    }
    trinket_profile_t( std::string name, int itemid, int itemlvl, int str, int crit, int haste, int mastery, int mult, int vers, int upgrade = 0, int demonic = -1 ) :
        name( name ), itemid( itemid ), itemlvl( itemlvl ), str( str ), crit( crit ), haste( haste ), mastery( mastery ), mult( mult ), vers( vers ), upgrade(upgrade), demonic( demonic ), not_selected( 0 )
    {};
};

std::vector<trinket_profile_t> trinkets_to_cmp = {};

void pushback_trinkets() {
    trinkets_to_cmp.clear();
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Vial of Convulsive Shadows" ).toStdString(), 113969, 670, 334, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Vial of Convulsive Shadows" ).toStdString(), 113969, 676, 354, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Vial of Convulsive Shadows" ).toStdString(), 113969, 685, 385, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Vial of Convulsive Shadows" ).toStdString(), 113969, 691, 407, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Vial of Convulsive Shadows" ).toStdString(), 113969, 700, 443, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Vial of Convulsive Shadows" ).toStdString(), 113969, 706, 468, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Forgemaster's Insignia" ).toStdString(), 113983, 670, 335, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Forgemaster's Insignia" ).toStdString(), 113983, 676, 355, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Forgemaster's Insignia" ).toStdString(), 113983, 685, 386, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Forgemaster's Insignia" ).toStdString(), 113983, 691, 408, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Forgemaster's Insignia" ).toStdString(), 113983, 700, 444, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Forgemaster's Insignia" ).toStdString(), 113983, 706, 469, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Horn of Screaming Spirits" ).toStdString(), 119193, 670, 283, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Horn of Screaming Spirits" ).toStdString(), 119193, 676, 299, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Horn of Screaming Spirits" ).toStdString(), 119193, 685, 325, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Horn of Screaming Spirits" ).toStdString(), 119193, 691, 344, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Horn of Screaming Spirits" ).toStdString(), 119193, 700, 374, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Horn of Screaming Spirits" ).toStdString(), 119193, 706, 395, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Scabbard of Kyanos" ).toStdString(), 118882, 665, 0, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Wild Gladiator's Badge of Victory" ).toStdString(), 124867, 700, 355, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Wild Combatant's Badge of Victory" ).toStdString(), 125041, 670, 244, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Primal Combatant's Badge of Victory" ).toStdString(), 119936, 620, 153, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Wild Gladiator's Insignia of Victory" ).toStdString(), 124868, 700, 0, 0, 0, 0, 0, 355, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Wild Combatant's Insignia of Victory" ).toStdString(), 125042, 670, 0, 0, 0, 0, 0, 244, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Primal Combatant's Insignia of Victory" ).toStdString(), 119937, 620, 0, 0, 0, 0, 0, 153, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Tectus' Beating Heart" ).toStdString(), 113645, 655, 216, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Tectus' Beating Heart" ).toStdString(), 113645, 661, 228, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Tectus' Beating Heart" ).toStdString(), 113645, 670, 248, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Tectus' Beating Heart" ).toStdString(), 113645, 676, 262, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Tectus' Beating Heart" ).toStdString(), 113645, 685, 285, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Tectus' Beating Heart" ).toStdString(), 113645, 691, 302, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bottle of Infesting Spores" ).toStdString(), 113658, 655, 228, 134, 0, 0, 0, 134, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bottle of Infesting Spores" ).toStdString(), 113658, 661, 241, 141, 0, 0, 0, 141, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bottle of Infesting Spores" ).toStdString(), 113658, 670, 262, 154, 0, 0, 0, 154, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bottle of Infesting Spores" ).toStdString(), 113658, 676, 277, 163, 0, 0, 0, 163, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bottle of Infesting Spores" ).toStdString(), 113658, 685, 301, 177, 0, 0, 0, 177, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bottle of Infesting Spores" ).toStdString(), 113658, 691, 319, 187, 0, 0, 0, 187, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Formidable Fang" ).toStdString(), 114613, 655, 112, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Grandiose Carnage" ).toStdString(), 114552, 645, 183, 0, 0, 183, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Draenic Philosopher's Stone" ).toStdString(), 109262, 620, 0, 0, 0, 97, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Stone of Wind" ).toStdString(), 122601, 640, 0, 0, 0, 0, 0, 175, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Stone of the Earth" ).toStdString(), 122602, 655, 0, 0, 0, 0, 0, 201, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Stone of the Water" ).toStdString(), 122603, 670, 0, 0, 0, 0, 0, 231, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Stone of Fire" ).toStdString(), 122604, 685, 0, 0, 0, 0, 0, 265, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Stone of the Wilds" ).toStdString(), 128023, 700, 0, 0, 0, 0, 0, 305, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Stone of the Elements" ).toStdString(), 128024, 715, 0, 0, 0, 0, 0, 351, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Skull of War" ).toStdString(), 112318, 640, 171, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Skull of War" ).toStdString(), 112318, 655, 196, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Skull of War" ).toStdString(), 112318, 670, 225, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Skull of War" ).toStdString(), 112318, 685, 259, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Skull of War" ).toStdString(), 112318, 700, 298, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Skull of War" ).toStdString(), 112318, 715, 343, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mote of the Mountain" ).toStdString(), 116292, 640, 175, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mote of the Mountain" ).toStdString(), 116292, 646, 185, 0, 0, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Storage House Key" ).toStdString(), 116317, 655, 201, 0, 0, 0, 201, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Storage House Key" ).toStdString(), 116317, 661, 212, 0, 0, 0, 212, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Worldbreaker's Resolve" ).toStdString(), 124523, 705, 0, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Worldbreaker's Resolve" ).toStdString(), 124523, 711, 0, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Worldbreaker's Resolve" ).toStdString(), 124523, 720, 0, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Worldbreaker's Resolve" ).toStdString(), 124523, 726, 0, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Worldbreaker's Resolve" ).toStdString(), 124523, 735, 0, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Worldbreaker's Resolve" ).toStdString(), 124523, 741, 0, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Discordant Chorus" ).toStdString(), 124237, 700, 397, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Discordant Chorus" ).toStdString(), 124237, 706, 420, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Discordant Chorus" ).toStdString(), 124237, 715, 456, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Discordant Chorus" ).toStdString(), 124237, 721, 482, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Discordant Chorus" ).toStdString(), 124237, 730, 525, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Discordant Chorus" ).toStdString(), 124237, 736, 555, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Empty Drinking Horn" ).toStdString(), 124238, 700, 397, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Empty Drinking Horn" ).toStdString(), 124238, 706, 420, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Empty Drinking Horn" ).toStdString(), 124238, 715, 456, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Empty Drinking Horn" ).toStdString(), 124238, 721, 482, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Empty Drinking Horn" ).toStdString(), 124238, 730, 525, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Empty Drinking Horn" ).toStdString(), 124238, 736, 555, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unending Hunger" ).toStdString(), 124236, 695, 0, 758, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unending Hunger" ).toStdString(), 124236, 701, 0, 800, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unending Hunger" ).toStdString(), 124236, 710, 0, 871, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unending Hunger" ).toStdString(), 124236, 716, 0, 922, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unending Hunger" ).toStdString(), 124236, 725, 0, 1003, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unending Hunger" ).toStdString(), 124236, 731, 0, 1060, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Rumbling Pebble" ).toStdString(), 124235, 690, 271, 0, 0, 271, 271, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Rumbling Pebble" ).toStdString(), 124235, 696, 286, 0, 0, 286, 286, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Rumbling Pebble" ).toStdString(), 124235, 705, 312, 0, 0, 312, 312, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Rumbling Pebble" ).toStdString(), 124235, 711, 329, 0, 0, 329, 329, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Rumbling Pebble" ).toStdString(), 124235, 720, 359, 0, 0, 359, 359, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Rumbling Pebble" ).toStdString(), 124235, 726, 379, 0, 0, 379, 379, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Spores of Alacrity" ).toStdString(), 110014, 685, 147, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Spores of Alacrity" ).toStdString(), 110014, 705, 177, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Spores of Alacrity" ).toStdString(), 110014, 630, 88, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Spores of Alacrity" ).toStdString(), 110014, 636, 93, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bonemaw's Big Toe" ).toStdString(), 110012, 685, 177, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bonemaw's Big Toe" ).toStdString(), 110012, 705, 213, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bonemaw's Big Toe" ).toStdString(), 110012, 630, 106, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Bonemaw's Big Toe" ).toStdString(), 110012, 636, 112, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Emberscale Talisman" ).toStdString(), 110013, 685, 177, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Emberscale Talisman" ).toStdString(), 110013, 705, 213, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Emberscale Talisman" ).toStdString(), 110013, 630, 106, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Emberscale Talisman" ).toStdString(), 110013, 636, 112, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mote of Corruption" ).toStdString(), 110010, 685, 265, 0, 0, 265, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mote of Corruption" ).toStdString(), 110010, 705, 319, 0, 0, 319, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mote of Corruption" ).toStdString(), 110010, 630, 159, 0, 0, 159, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mote of Corruption" ).toStdString(), 110010, 636, 168, 0, 0, 168, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Fires of the Sun" ).toStdString(), 110011, 685, 265, 0, 0, 0, 265, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Fires of the Sun" ).toStdString(), 110011, 705, 319, 0, 0, 0, 319, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Fires of the Sun" ).toStdString(), 110011, 630, 159, 0, 0, 0, 159, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Fires of the Sun" ).toStdString(), 110011, 636, 168, 0, 0, 0, 168, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unquenchable Doomfire Censer" ).toStdString(), 128153, 685, 265, 133, 0, 133, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Unquenchable Doomfire Censer" ).toStdString(), 128153, 691, 281, 140, 0, 140, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Fetid Salivation" ).toStdString(), 128148, 685, 265, 0, 265, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Fetid Salivation" ).toStdString(), 128148, 691, 281, 0, 281, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Mark of Supreme Doom" ).toStdString(), 124546, 705, 187, 0, 0, 0, 0, 0, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Chipped Soul Prism" ).toStdString(), 124545, 705, 187, 187, 187, 187, 187, 187, 1 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Gronntooth War Horn" ).toStdString(), 133595, 715, 140, 140, 140, 0, 0, 0, 0 ) );
    trinkets_to_cmp.push_back( trinket_profile_t( QApplication::translate( "gicClass", "Gronntooth War Horn(Demon)" ).toStdString(), 133595, 715, 140, 140, 140, 0, 0, 0, 0, 1 ) );
    std::stable_sort(trinkets_to_cmp.begin(), trinkets_to_cmp.end(), [](const trinket_profile_t& t1, const trinket_profile_t& t2){return t1.itemlvl > t2.itemlvl;});
    int s = trinkets_to_cmp.size();
    for(int i = 0; i < s; i++){
        if(trinkets_to_cmp[i].upgrade){
            trinkets_to_cmp[i].upgrade = 0;
            trinket_profile_t g1(trinkets_to_cmp[i]);
            trinket_profile_t g2(trinkets_to_cmp[i]);
            g1.upgrade = 5;
            g2.upgrade = 10;
            g1.itemlvl += 5;
            g2.itemlvl += 10;
            g1.name = (QString::fromStdString(g1.name) + QString("(+5)")).toStdString();
            g2.name = (QString::fromStdString(g2.name) + QString("(+10)")).toStdString();
            g1.crit = (int)(((double) g1.crit) * pow(ilvlScaleCoeff, 5));
            g1.mastery = (int)(((double) g1.mastery) * pow(ilvlScaleCoeff, 5));
            g1.haste = (int)(((double) g1.haste) * pow(ilvlScaleCoeff, 5));
            g1.mult = (int)(((double) g1.mult) * pow(ilvlScaleCoeff, 5));
            g1.vers = (int)(((double) g1.vers) * pow(ilvlScaleCoeff, 5));
            g1.str = (int)(((double) g1.str) * pow(ilvlScaleCoeff, 5));
            g2.crit = (int)(((double) g2.crit) * pow(ilvlScaleCoeff, 10));
            g2.mastery = (int)(((double) g2.mastery) * pow(ilvlScaleCoeff, 10));
            g2.haste = (int)(((double) g2.haste) * pow(ilvlScaleCoeff, 10));
            g2.mult = (int)(((double) g2.mult) * pow(ilvlScaleCoeff, 10));
            g2.vers = (int)(((double) g2.vers) * pow(ilvlScaleCoeff, 10));
            g2.str = (int)(((double) g2.str) * pow(ilvlScaleCoeff, 10));
            trinkets_to_cmp.push_back(g2);
            trinkets_to_cmp.push_back(g1);
        }
    }
}

void gic::on_btnSelectTrinkets_clicked()
{
    if(trinkets_to_cmp.empty()) pushback_trinkets();
    if(!dlgTrinkets){
        dlgTrinkets = new QDialog();
        uiTrinkets.setupUi(dlgTrinkets);
        connect( uiTrinkets.btnSelectAllTrinkets, SIGNAL( clicked( void ) ), this, SLOT( on_btnSelectAllTrinkets_clicked() ) );
        connect( uiTrinkets.btnSelectNoneTrinkets, SIGNAL( clicked( void ) ), this, SLOT( on_btnSelectNoneTrinkets_clicked() ) );
        uiTrinkets.btnSelectTrinketsBtns->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
        uiTrinkets.btnSelectTrinketsBtns->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    }
    uiTrinkets.listTrinketList->clear();
    for ( auto i = trinkets_to_cmp.begin(); i != trinkets_to_cmp.end(); i++ ) {
        QListWidgetItem* aitem = new QListWidgetItem(QString("[") + QString::number(i->itemlvl) + QString("]") + QString::fromStdString(i->name));
        aitem->setFlags(aitem->flags() | Qt::ItemIsUserCheckable);
        aitem->setCheckState(i->not_selected ? Qt::Unchecked : Qt::Checked);
        uiTrinkets.listTrinketList->addItem(aitem);
    }
    dlgTrinkets->show();
}

void gic::on_btnSelectTrinketsBtns_accepted()
{
    if(trinkets_to_cmp.empty()) pushback_trinkets();
    for( int i = 0; i < trinkets_to_cmp.size(); i++ ) {
        trinkets_to_cmp[i].not_selected = uiTrinkets.listTrinketList->item(i)->checkState() != Qt::Checked;
    }
    dlgTrinkets->hide();
}

void gic::on_btnSelectTrinketsBtns_rejected()
{
    dlgTrinkets->hide();
}

void gic::on_btnSelectAllTrinkets_clicked()
{
    for( int i = 0; i < uiTrinkets.listTrinketList->count(); i++ ){
        uiTrinkets.listTrinketList->item(i)->setCheckState(Qt::Checked);
    }
}

void gic::on_btnSelectNoneTrinkets_clicked()
{
    for( int i = 0; i < uiTrinkets.listTrinketList->count(); i++ ){
        uiTrinkets.listTrinketList->item(i)->setCheckState(Qt::Unchecked);
    }
}

void trinket_benchmark() {
    *report_path << QApplication::translate( "gicClass", "Start Trinket Benchmark." ).toStdString() << std::endl;
    if(trinkets_to_cmp.empty()) pushback_trinkets();
    current_stat.gear_str = 0;
    current_stat.gear_crit = 0;
    current_stat.gear_haste = 0;
    current_stat.gear_mastery = 0;
    current_stat.gear_mult = 0;
    current_stat.gear_vers = 0;
    current_stat.name = "Trinket Baseline";
    trinket1 = 0;
    trinket2 = 0;
    for ( int i = 0; i < 16; i++ ) {
        if ( i == 14 || i == 15 ) continue;
        current_stat.gear_str += gear_list[i].str;
        current_stat.gear_crit += gear_list[i].crit;
        current_stat.gear_haste += gear_list[i].haste;
        current_stat.gear_mastery += gear_list[i].mastery;
        current_stat.gear_mult += gear_list[i].mult;
        current_stat.gear_vers += gear_list[i].vers;
    }
    stat_array.clear();
    calculate_scale_factors = 0;
    stat_array.push_back( current_stat );
    parameters_consistency();
    auto_apl();
    generate_predef();
    ocl().run( apl, predef );
    float baseline_dps = stat_array[0].dps;
    for ( auto i = trinkets_to_cmp.begin(); i != trinkets_to_cmp.end(); i++ )
        i->eval();
    std::sort( trinkets_to_cmp.begin(), trinkets_to_cmp.end() );
    FILE* f = fopen( "trinket_ladder.csv", "wb" );
    fprintf( f, "\xEF\xBB\xBF" );
    for ( auto i = trinkets_to_cmp.begin(); i != trinkets_to_cmp.end(); i++ ) {
        if(i->not_selected) continue;
        fprintf( f, "\"[%d]%s\",%d\r\n", i->itemlvl, i->name.c_str(), int( i->dps - baseline_dps + 0.5 ) );
        *report_path << "[" << i->itemlvl << "]" << i->name << ": " << i->dps - baseline_dps << std::endl;
    }
    fclose( f );
    *report_path << QApplication::translate( "gicClass", "Trinket ladder saved as \"trinket_ladder.csv\"." ).toStdString() << std::endl;
}
