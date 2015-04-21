/**
	IreCore kernel, Aean <v#aean.net>
	based on LuaCL kernel https://github.com/llxibo/LuaCL

	*** EDIT AT YOUR OWN RISK! ***
	All kernel source goes here exposed to the air, for your conveinence to edit.
	But it is NOT recommended if you are not definitely sure how to edit.

	IreCore is distributed under the terms of The MIT License.
	Free to use for any purpose. No warranty given.
*/

#define SHOW_LOG
#define passive_power_regen 0

#if !defined(__OPENCL_VERSION__)
/* Front-end gives these defines. */
#define RNG_MWC64X
#define vary_combat_length 20.0f
#define max_length 450.0f
#define initial_health_percentage 100.0f
#define death_pct 0.0f
#define iterations 1
#define power_max 120.0f
#define PLATE_SPECIALIZATION 1
#define SINGLE_MINDED 0
#define BUFF_STR_AGI_INT 1
#define BUFF_AP 1
#define BUFF_CRIT 1
#define BUFF_HASTE 1
#define BUFF_MASTERY 1
#define BUFF_MULT 1
#define BUFF_VERS 1
#define BUFF_SP 1
#define BUFF_STA 1
#define BUFF_POTION 1
#define BUFF_BLOODLUST 1
#define RACE RACE_TAUREN
#define MH_LOW 1659
#define MH_HIGH 2490
#define MH_SPEED 3.6f
#define MH_TYPE WEAPON_2H
#define OH_LOW 1659
#define OH_HIGH 2490
#define OH_SPEED 3.6f
#define OH_TYPE WEAPON_2H
#define TALENT_TIER3 2
#define TALENT_TIER4 1
#define TALENT_TIER6 2
#define TALENT_TIER7 1
#define archmages_incandescence 0
#define archmages_greater_incandescence 1
#define t17_2pc 1
#define t17_4pc 1
#define thunderlord_mh 1
#define thunderlord_oh 1
#define bleedinghollow_mh 0
#define bleedinghollow_oh 0
#define shatteredhand_mh 0
#define shatteredhand_oh 0
//#define trinket_vial_of_convulsive_shadows 2033
//#define trinket_forgemasters_insignia 181
#define trinket_horn_of_screaming_spirits 2652
#define trinket_scabbard_of_kyanos 2200
#endif /* !defined(__OPENCL_VERSION__) */

/* Debug on Host! */
#if !defined(__OPENCL_VERSION__)
#ifndef _DEBUG
#define _DEBUG
#endif /* _DEBUG */
#endif /* !defined(__OPENCL_VERSION__) */

/* Codes enclosed in 'hostonly' will be discarded at OpenCL devices, and vice versa. */
#if defined(__OPENCL_VERSION__)
#define hostonly(code)
#define deviceonly(code) code
#else
#define hostonly(code) code
#define deviceonly(code)
#endif /* defined(__OPENCL_VERSION__) */
#if defined(_MSC_VER)
#define msvconly(code) code
#else
#define msvconly(code)
#endif /* defined(_MSC_VER) */

/* Macro guard word. */
#define safemacro(v) do{v;}while(0)

/* Std C Library. */
#if !defined(__OPENCL_VERSION__)
int printf( const char* format, ... );
void abort( void );
void* malloc( unsigned long long );
void free( void* );
#define KRNL_STR2(v) #v
#define KRNL_STR(v) KRNL_STR2(v)
#endif /* !defined(__OPENCL_VERSION__) */

/* Diagnostic. */
#if defined(_DEBUG) && !defined(__OPENCL_VERSION__)
#if defined(_MSC_VER)
#define hfuncname __FUNCTION__
#else
#define hfuncname __func__
#endif /* defined(_MSC_VER) */
#define assert(expression) if(!(expression)){ \
        printf("Assertion failed: %s, function %s, file %s, line %d.\n", \
                KRNL_STR(expression),  hfuncname ,__FILE__, __LINE__); \
                abort(); }else
#else
#define assert(expression)
#endif /* defined(_DEBUG) && !defined(__OPENCL_VERSION__) */

/* Unified typename. */
#if defined(__OPENCL_VERSION__)
#define kbool bool
#define k8s char
#define k8u uchar
#define k16s short
#define k16u ushort
#define k32s int
#define k32u uint
#define k64s long
#define k64u ulong
#define K64S_C(num) (num##L)
#define K64U_C(num) (num##UL)
#else
#define kbool int
#define k8s signed char
#define k8u unsigned char
#define k16s short int
#define k16u unsigned short int
#define k32s long int
#define k32u unsigned long int
#define k64s long long int
#define k64u unsigned long long int
#define K64S_C(num) (num##LL)
#define K64U_C(num) (num##ULL)
#define convert_ushort_sat(num) ((num) < 0 ? (k16u)0 : (num) > 0xffff ? (k16u)0xffff : (k16u)(num))
#define convert_ushort_rtp(num) ((k16u)(num) + !!((float)(num) - (float)(k16u)(num)))
#define convert_uint_rtp(num) ((k32u)(num) + !!((float)(num) - (float)(k32u)(num)))
#define convert_uint_rtz(num) ((k32u)(num))
float convert_float_rtp( k64u x ) {
    union {
        k32u u;
        float f;
    } u;
    u.f = ( float )x;
    if( ( k64u )u.f < x && x < K64U_C( 0xffffff8000000000 ) )
        u.u++;
    return u.f;
}
#endif /* defined(__OPENCL_VERSION__) */

/*
    Assumed char is exactly 8 bit.
    Most std headers will pollute the namespace, thus we do not use CHAR_BIT,
    which is defined in <limits.h>.
*/
#define K64U_MSB ( K64U_C( 1 ) << (sizeof(k64u) * 8 - 1) )

/* Unified compile hint. */
#if defined(__OPENCL_VERSION__)
#define kdeclspec(attr) __attribute__((attr))
#define hdeclspec(attr)
#else
#define kdeclspec(attr)
#if defined(_MSC_VER)
#define hdeclspec __declspec
#else
#define hdeclspec(attr) __attribute__((attr))
#endif /* defined(_MSC_VER) */
#endif /* defined(__OPENCL_VERSION__) */

/* get_global_id() on Host. */
#if !defined(__OPENCL_VERSION__)
#if defined(_MSC_VER)
#define htlsvar __declspec(thread)
#else
#define htlsvar __thread
#endif /* defined(_MSC_VER) */
htlsvar int global_idx[3] = {0};
int get_global_id( int dim ) {
    return global_idx[dim];
}
void set_global_id( int dim, int idx ) {
    global_idx[dim] = idx;
}
#endif /* !defined(__OPENCL_VERSION__) */

/* clz() "Count leading zero" on Host */
#if !defined(__OPENCL_VERSION__)
#if defined(_MSC_VER)
#if defined(_M_IA64) || defined(_M_X64)
unsigned char _BitScanReverse64( unsigned long* _Index, unsigned __int64 _Mask ); /* MSVC Intrinsic. */
/*
    _BitScanReverse64 set IDX to the position(from LSB) of the first bit set in mask.
    What we need is counting leading zero, thus return 64 - (IDX + 1).
*/
k8u clz( k64u mask ) {
    unsigned long IDX = 0;
    _BitScanReverse64( &IDX, mask );
    return 63 - IDX;
}
#elif defined(_M_IX86)
unsigned char _BitScanReverse( unsigned long* _Index, unsigned long _Mask ); /* MSVC Intrinsic. */
/*
    On 32-bit machine, _BitScanReverse only accept 32 bit number.
    So we need do some cascade.
*/
k32u clz( k64u mask ) {
    unsigned long IDX = 0;
    if ( mask >> 32 ) {
        _BitScanReverse( &IDX, mask >> 32 );
        return ( k32u )( 31 - IDX );
    }
    _BitScanReverse( &IDX, mask & 0xFFFFFFFFULL );
    return ( k32u )( 63 - IDX );
}
#else
/*
    This machine is not x86/amd64/Itanium Processor Family(IPF).
    the processor don't have a 'bsr' instruction so _BitScanReverse is not available.
    Need some bit manipulation...
*/
#define NEED_CLZ_BIT_TWIDDLING
#endif /* defined(_M_IA64) || defined(_M_X64) */
#elif defined(__GNUC__)
#define clz __builtin_clzll /* GCC have already done this. */
#else
/*
    Unkown compilers. We know nothing about what intrinsics they have,
    nor what their inline assembly format is.
*/
#define NEED_CLZ_BIT_TWIDDLING
#endif /* defined(_MSC_VER) */
#endif /* !defined(__OPENCL_VERSION__) */
#if defined(NEED_CLZ_BIT_TWIDDLING)
#undef NEED_CLZ_BIT_TWIDDLING
/*
    Tons of magic numbers... For how could this works, see:
        http://supertech.csail.mit.edu/papers/debruijn.pdf
    result for zero input is NOT same as MSVC intrinsic version, but still undefined.
*/
k32u clz( k64u mask ) {
    static k32u DeBruijn[64] = {
        /*        0   1   2   3   4   5   6   7 */
        /*  0 */  0, 63, 62, 11, 61, 57, 10, 37,
        /*  8 */ 60, 26, 23, 56, 30,  9, 16, 36,
        /* 16 */  2, 59, 25, 18, 20, 22, 42, 55,
        /* 24 */ 40, 29,  5,  8, 15, 46, 35, 53,
        /* 32 */  1, 12, 58, 38, 27, 24, 31, 17,
        /* 40 */  3, 19, 21, 43, 41,  6, 47, 54,
        /* 48 */ 13, 39, 28, 32,  4, 44,  7, 48,
        /* 56 */ 14, 33, 45, 49, 34, 50, 51, 52,
    };
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;
    return DeBruijn[( v * 0x022fdd63cc95386dULL ) >> 58];
}
#endif /* defined(NEED_CLZ_BIT_TWIDDLING) */

/* Const Pi. */
#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif /* !defined(M_PI) */

/* Math utilities on Host. */
#if !defined(__OPENCL_VERSION__)
double cos( double x );
#define cospi(x) cos( (double)(x) * M_PI )
double sqrt( double x );
double log( double x );
double clamp( double val, double min, double max ) {
    return val < min ? min : val > max ? max : val;
}
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define mix(x, y, a) ((x) + (( (y) - (x) ) * (a)))
#define mad_hi(x,y,c) ((k32u)(((k64u)(x) * (k64u)(y)) >> 32) + (c))
#endif /* !defined(__OPENCL_VERSION__) */

/* Seed struct which holds the current state. */
#if defined(RNG_MT127)
typedef struct {
    k32u mt[4]; /* State words. */
    k32u mti;   /* State counter: must be within [0,3]. */
} seed_t;
#elif defined(RNG_MWC64X)
typedef struct {
    k32u x;
    k32u c;
} seed_t;
#else
typedef struct {
    k32u holdrand;
} seed_t;
#endif

typedef k32u time_t;
#define FROM_SECONDS( sec ) ((time_t)convert_uint_rtp((float)(sec) * 1000.0f))
#define FROM_MILLISECONDS( msec ) ((time_t)(msec))
#define TO_SECONDS( timestamp ) (convert_float_rtp((k32u)timestamp) * 0.001f)
#define TIME_OFFSET( time ) ((time_t)((k32s)rti->timestamp + (k32s)time))
#define TIME_DISTANT( time ) ((time_t)max((k32s)(time) - (k32s)(rti->timestamp), 0))
#define UP( time_to_check ) ( rti->player.time_to_check && rti->player.time_to_check > rti->timestamp )
#define REMAIN( time_to_check ) ((time_t)max(((k32s)rti->player.time_to_check - (k32s)rti->timestamp), 0))

/* Event queue. */
#define EQ_SIZE_EXP (6)
#define EQ_SIZE ((1 << EQ_SIZE_EXP) - 1)
typedef struct {
    time_t time;
    k32u routine;
} _event_t;
typedef struct {
    k32u count;
    time_t power_suffice;
    _event_t event[EQ_SIZE];
} event_queue_t;
hostonly(
    static k32u maxqueuelength = 0;
)

#define RACE_NONE 0
#define RACE_HUMAN 1
#define RACE_DWARF 2
#define RACE_GNOME 3
#define RACE_NIGHTELF_DAY 4
#define RACE_NIGHTELF_NIGHT 5
#define RACE_DRAENEI 6
#define RACE_WORGEN 7
#define RACE_ORC 8
#define RACE_TROLL 9
#define RACE_TAUREN 10
#define RACE_UNDEAD 11
#define RACE_BLOODELF 12
#define RACE_GOBLIN 13
#define RACE_PANDAREN 14

deviceonly( __constant ) k32s racial_base_str[] = {
    0, 0, 5, -5, -4, -4, 66, 3, 3, 1, 5, -1, -3, -3, 0,
};

/* Declarations from class modules. */
typedef struct {
    time_t cd;
} ICD_t;
typedef struct {
    time_t lasttimeattemps;
    time_t lasttimeprocs;
} RPPM_t;
#if (TALENT_TIER3 != 3)
typedef struct {
    time_t cd;
} bloodthirst_t;
#endif
typedef struct {
    k16u stack;
    time_t expire;
} ragingblow_t;
typedef struct {
    time_t expire;
} enrage_t;
typedef struct {
    k16u stack;
    time_t expire;
} bloodsurge_t;
typedef struct {
    time_t expire;
	RPPM_t proc;
} suddendeath_t;
typedef struct {
    time_t cd;
} berserkerrage_t;
typedef struct {
    time_t cd;
    time_t expire;
} recklessness_t;

#if (TALENT_TIER4 == 1)
typedef struct {
    time_t cd;
} stormbolt_t;
#endif
#if (TALENT_TIER4 == 2)
typedef struct {
    time_t cd;
} shockwave_t;
#endif
#if (TALENT_TIER4 == 3)
typedef struct {
    time_t cd;
} dragonroar_t;
#endif
#if (TALENT_TIER6 == 1)
typedef struct {
    time_t cd;
    time_t expire;
} avatar_t;
#endif
#if (TALENT_TIER6 == 2)
typedef struct {
    time_t cd;
    time_t expire;
    float pool;
    float ticks;
    time_t dot_start;
} bloodbath_t;
#endif
#if (TALENT_TIER6 == 3)
typedef struct {
    time_t cd;
    time_t expire;
} bladestorm_t;
#endif

#if (TALENT_TIER7 == 2)
typedef struct {
    time_t cd;
    time_t expire;
} ravager_t;
#endif
#if (TALENT_TIER7 == 3)
typedef struct {
    time_t cd;
} siegebreaker_t;
#endif
#if (BUFF_POTION == 1)
typedef struct {
    time_t expire;
    time_t cd;
} potion_t;
#endif
#if (t17_4pc)
typedef struct {
	time_t expire;
	k32u stack;
} rampage_t;
#endif
#if (archmages_incandescence || archmages_greater_incandescence)
typedef struct {
	time_t expire;
	RPPM_t proc;
} incandescence_t;
#endif
#if (thunderlord_mh || thunderlord_oh)
typedef struct{
	time_t expire;
	k32u extend;
	RPPM_t proc;
} thunderlord_t;
#endif
#if (bleedinghollow_mh || bleedinghollow_oh)
typedef struct{
	time_t expire;
	RPPM_t proc;
} bleedinghollow_t;
#endif
#if (shatteredhand_mh || shatteredhand_oh)
typedef struct{
	time_t expire;
	float ticks;
	RPPM_t proc;
} shatteredhand_t;
#endif
#if (RACE == RACE_BLOODELF)
typedef struct{
	time_t cd;
} arcanetorrent_t;
#endif
#if (RACE == RACE_TROLL)
typedef struct{
	time_t cd;
	time_t expire;
} berserking_t;
#endif
#if (RACE == RACE_ORC)
typedef struct{
	time_t cd;
	time_t expire;
} bloodfury_t;
#endif

typedef struct weapon_t {
    float speed;
    k32u low;
    k32u high;
    k32u type;
} weapon_t;

#define WEAPON_2H 0
#define WEAPON_1H 1
#define WEAPON_DAGGER 2

deviceonly( __constant ) weapon_t weapon[] = {
    {
        MH_SPEED,
        MH_LOW,
        MH_HIGH,
        MH_TYPE,
    },
    {
        OH_SPEED,
        OH_LOW,
        OH_HIGH,
        OH_TYPE,
    },
};

deviceonly( __constant ) float normalized_weapon_speed[] = {
    3.3f,
    2.4f,
    1.7f,
};

typedef struct stat_t {
    k32u gear_str;
    k32u gear_crit;
    k32u gear_haste;
    k32u gear_mastery;
    k32u gear_mult;
    k32u gear_vers;
    float crit;
    float haste;
    float mastery;
    float mult;
    float vers;
    k32u str;
    k32u ap;
} stat_t;

/* Player struct, filled by the class module. */
typedef struct {
    float power;
#if (passive_power_regen)
    float power_regen;
#endif
	stat_t stat;
#if (TALENT_TIER3 != 3)
    bloodthirst_t   bloodthirst;
#endif
    ragingblow_t    ragingblow;
    enrage_t        enrage;
    bloodsurge_t    bloodsurge;
    berserkerrage_t	berserkerrage;
    recklessness_t	recklessness;
#if (TALENT_TIER3 == 2)
    suddendeath_t	suddendeath;
#endif
#if (TALENT_TIER4 == 1)
    stormbolt_t		stormbolt;
#endif
#if (TALENT_TIER4 == 2)
    shockwave_t		shockwave;
#endif
#if (TALENT_TIER4 == 3)
    dragonroar_t	dragonroar;
#endif
#if (TALENT_TIER6 == 1)
    avatar_t		avatar;
#endif
#if (TALENT_TIER6 == 2)
    bloodbath_t		bloodbath;
#endif
#if (TALENT_TIER6 == 3)
    bladestorm_t	bladestorm;
#endif
#if (TALENT_TIER7 == 2)
    ravager_t		ravager;
#endif
#if (TALENT_TIER7 == 3)
    siegebreaker_t	siegebreaker;
#endif
#if (BUFF_POTION == 1)
    potion_t potion;
#endif
#if (t17_4pc)
	rampage_t		rampage;
#endif
#if (archmages_incandescence || archmages_greater_incandescence)
	incandescence_t incandescence;
#endif
#if (thunderlord_mh)
	thunderlord_t	enchant_mh;
#endif
#if (thunderlord_oh)
	thunderlord_t	enchant_oh;
#endif
#if (bleedinghollow_mh)
	bleedinghollow_t enchant_mh;
#endif
#if (bleedinghollow_oh)
	bleedinghollow_t enchant_oh;
#endif
#if (shatteredhand_mh)
	shatteredhand_t enchant_mh;
#endif
#if (shatteredhand_oh)
	shatteredhand_t enchant_oh;
#endif
#if (RACE == RACE_BLOODELF)
	arcanetorrent_t arcanetorrent;
#endif
#if (RACE == RACE_TROLL)
	berserking_t	berserking;
#endif
#if (RACE == RACE_ORC)
	bloodfury_t		bloodfury;
#endif
#if defined(trinket_vial_of_convulsive_shadows)
	struct{
		time_t cd;
		time_t expire;
	} vial_of_convulsive_shadows;
#endif
#if defined(trinket_forgemasters_insignia)
	struct{
		time_t expire;
		k32u stack;
		RPPM_t proc;
	} forgemasters_insignia;
#endif
#if defined(trinket_horn_of_screaming_spirits)
	struct{
		time_t expire;
		RPPM_t proc;
	} horn_of_screaming_spirits;
#endif
#if defined(trinket_scabbard_of_kyanos)
	struct{
		time_t cd;
		time_t expire;
	} scabbard_of_kyanos;
#endif
	time_t gcd;
}
player_t;

/* Runtime info struct, each thread preserves its own. */
typedef struct {
    seed_t seed;
    time_t timestamp;
    event_queue_t eq;
    float damage_collected;
    player_t player;
    time_t expected_combat_length;

}
rtinfo_t;

/* Formated time print. */
hostonly(
void tprint( rtinfo_t* rti ) {
    printf( "%02d:%02d.%03d ", rti->timestamp / 60000, ( rti->timestamp % 60000 ) / 1000, ( ( rti->timestamp % 60000 ) % 1000 ) );
}
)
#if defined(SHOW_LOG)
#define lprintf(v) hostonly(safemacro({tprint(rti);printf v;printf("\n");}))
#else
#define lprintf(v)
#endif /* defined(SHOW_LOG) */

/* Declaration Action Priority List (APL) */
void scan_apl( rtinfo_t* rti ); /* Implement is generated by front-end. */

/*
    Event routine entries. Each class module implement its own.
    Each type of event should be assigned to a routine number.
    Given routine number 'routine_entries' select the corresponding function to call.
*/
void routine_entries( rtinfo_t* rti, _event_t e );
/** Routine number 0xFF indicates the end of simulation. */
#define EVENT_END_SIMULATION ((k32u)-1)

/*
    Class modules may need an initializer, link it here.
*/
void module_init( rtinfo_t* rti );

/* Initialize RNG */
#if defined(RNG_MT127)
void rng_init( rtinfo_t* rti, k32u seed ) {
    rti->seed.mti = 0; /* Reset counter */
    /* Use a LCG to fill state matrix. See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    rti->seed.mt[0] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 1;
    rti->seed.mt[1] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 2;
    rti->seed.mt[2] = seed & 0xffffffffUL;
    /* Due to multiple run for same kernel, set thread id into state words to avoid seed overlapping. */
    seed = ( k32u )get_global_id( 0 );
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 3;
    rti->seed.mt[3] = seed & 0xffffffffUL;
}
#elif defined(RNG_MWC64X)
void rng_init( rtinfo_t* rti, k32u seed ) {
    rti->seed.x = seed;
    /* Due to multiple run for same kernel, set thread id into state words to avoid seed overlapping. */
    rti->seed.c = ( k32u )get_global_id( 0 );
}
#else
void rng_init( rtinfo_t* rti, k32u seed ) {
	seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 1;
    /* Due to multiple run for same kernel, set thread id into state words to avoid seed overlapping. */
    rti->seed.holdrand = seed + ( k32u )get_global_id( 0 );
}
#endif
/* Generate one IEEE-754 single precision float point uniformally distributed in the interval [.0f, 1.0f). */
#if defined(RNG_MT127)
/*
	Minimalist Mersenne Twister for OpenCL
	MT by Makoto Matsumoto, See http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
*/
float uni_rng( rtinfo_t* rti ) {
    k32u y; /* Should be a register. */
    assert( rti->seed.mti < 4 ); /* If not, RNG is uninitialized. */

    /* Concat lower-right and upper-left state bits. */
    y = ( rti->seed.mt[rti->seed.mti] & 0xfffffffeU ) | ( rti->seed.mt[( rti->seed.mti + 1 ) & 3] & 0x00000001U );
    /* Compute next state with the recurring equation. */
    y = rti->seed.mt[rti->seed.mti] = rti->seed.mt[( rti->seed.mti + 2 ) & 3] ^ ( y >> 1 ) ^ ( 0xfa375374U & -( k32s )( y & 0x1U ) );
    /* Increase the counter */
    rti->seed.mti = ( rti->seed.mti + 1 ) & 3;
    /* Tempering */
    y ^= ( y >> 12 );
    y ^= ( y << 7 ) & 0x33555f00U;
    y ^= ( y << 15 ) & 0xf5f78000U;
    y ^= ( y >> 18 );
    /* Mask to IEEE-754 format [1.0f, 2.0f). */
    y = ( ( y & 0x3fffffffU ) | 0x3f800000U );
    return ( *( float* )&y ) - 1.0f; /* Decrease to [.0f, 1.0f). */
}
#elif defined(RNG_MWC64X)
/*
	Part of MWC64X by David Thomas, dt10@imperial.ac.uk
	This is provided under BSD, full license is with the main package.
	See http://www.doc.ic.ac.uk/~dt10/research
*/
float uni_rng( rtinfo_t* rti ) {
    k32u Xn = 4294883355U * rti->seed.x + rti->seed.c;
	k32u Cn = mad_hi(4294883355U, rti->seed.x, (k32u)(Xn<rti->seed.c));
	rti->seed.x = Xn;
	rti->seed.c = Cn;
	k32u y = Xn ^ Cn;
    /* Mask to IEEE-754 format [1.0f, 2.0f). */
    y = ( ( y & 0x3fffffffU ) | 0x3f800000U );
    return ( *( float* )&y ) - 1.0f; /* Decrease to [.0f, 1.0f). */
}
#else
/*
	Classic 32-bit LCG known as "rand".
*/
float uni_rng( rtinfo_t* rti ) {
	rti->seed.holdrand = rti->seed.holdrand * 214013 + 2531011;
    k32u y = rti->seed.holdrand;
    /* Mask to IEEE-754 format [1.0f, 2.0f). */
    y = ( ( y & 0x3fffffffU ) | 0x3f800000U );
    return ( *( float* )&y ) - 1.0f; /* Decrease to [.0f, 1.0f). */
}
#endif

/* Generate one IEEE-754 single precision float point with standard normal distribution. */
float stdnor_rng( rtinfo_t* rti ) {
    /*
        The max number generated by uni_rng() should equal to:
            as_float( 0x3fffffff ) - 1.0f => as_float( 0x3f7ffffe )
        Thus, we want transform the interval to (.0f, 1.0f], we should add:
            1.0f - as_float( 0x3f7ffffe ) => as_float( 0x34000000 )
        Which is representable by decimal 1.1920929E-7.
        With a minimal value 1.1920929E-7, the max vaule stdnor_rng could give is approximately 5.64666.
    */
    return ( float )( sqrt( -2.0f * log( uni_rng( rti ) + 1.1920929E-7 ) ) * cospi( 2.0f * uni_rng( rti ) ) );
    /*
        To get another individual normally distributed number in pair, replace 'cospi' to 'sinpi'.
        It's simply thrown away here, because of diverge penalty.
        With only one thread in a warp need to recalculate, the whole warp must diverge.
    */
}

/* Enqueue an event into EQ. */
_event_t* eq_enqueue( rtinfo_t* rti, time_t trigger, k32u routine ) {
    k32u i = ++( rti->eq.count );
    _event_t* p = &( rti->eq.event[-1] );

    assert( rti->eq.count <= EQ_SIZE ); /* Full check. */
    /* Count max queue length on debug. */
    hostonly(
        maxqueuelength = max( maxqueuelength, rti->eq.count );
    )

    /*
    	There are two circumstances which could cause the assert below fail:
    	1. Devs got something wrong in the class module, enqueued an event happens before 'now'.
    	2. Time register is about to overflow, the triggering delay + current timestamp have exceeded the max representable time.
    	Since the later circumstance is not a fault, we would just throw the event away and continue quietly.
    	When you are exceeding the max time limits, all new events will be thrown, and finally you will get an empty EQ,
    	then the empty checks on EQ will fail.
    	*/
    if ( rti->timestamp <= trigger ) {
        for ( ; i > 1 && p[i >> 1].time > trigger; i >>= 1 )
            p[i] = p[i >> 1];
        p[i] = ( _event_t ) {
            .time = trigger, .routine = routine
        };
        return &p[i];
    }

    return 0;
}

/* Enqueue a power suffice event into EQ. */
void eq_enqueue_ps( rtinfo_t* rti, time_t trigger ) {
    if ( trigger > rti->timestamp )
        if ( !rti->eq.power_suffice || rti->eq.power_suffice > trigger )
            rti->eq.power_suffice = trigger;
}

/* Power gain. */
void power_gain( rtinfo_t* rti, float power ) {
    rti->player.power = min( power_max, rti->player.power + power );
}

/* Power check. */
kbool power_check( rtinfo_t* rti, float cost ) {
    if ( cost <= rti->player.power ) return 1;
#if (passive_power_regen)
    if ( rti->player.power_regen > 0 )
        eq_enqueue_ps( rti, TIME_OFFSET( FROM_SECONDS( ( cost - rti->player.power ) / rti->player.power_regen ) ) );
#endif
	return 0;
}

#if (TALENT_TIER7 == 1)
void anger_management_count( rtinfo_t* rti, float rage );
#endif

/* Power consume. */
void power_consume( rtinfo_t* rti, float cost ) {
    assert( power_check( rti, cost ) ); /* Power should suffice. */
    rti->player.power -= cost;
#if (TALENT_TIER7 == 1)
    anger_management_count( rti, cost );
#endif
}

/* Execute the top priority. */
int eq_execute( rtinfo_t* rti ) {
    k32u i, child;
    _event_t min, last;
    _event_t* p = &rti->eq.event[-1];

    assert( rti->eq.count ); /* Empty check. */
    assert( rti->eq.count <= EQ_SIZE ); /* Not zero but negative? */
    assert( rti->timestamp <= p[1].time ); /* Time won't go back. */
    assert( !rti->eq.power_suffice || rti->timestamp <= rti->eq.power_suffice ); /* Time won't go back. */

    ///* If time jumps over 1 second, insert a check point (as a power suffice event). */
    //if ( FROM_SECONDS( 1 ) < TIME_DISTANT( p[1].time ) &&
    //        ( !rti->eq.power_suffice || FROM_SECONDS( 1 ) < TIME_DISTANT( rti->eq.power_suffice ) ) )
    //    rti->eq.power_suffice = TIME_OFFSET( FROM_SECONDS( 1 ) );

    /* When time elapse, trigger a full scanning at APL. */
    if ( rti->timestamp < p[1].time 
		 && ( !rti->eq.power_suffice || rti->timestamp < rti->eq.power_suffice ) 
		) {
        scan_apl( rti ); /* This may change p[1]. */

        /* Check again. */
        assert( rti->eq.count );
        assert( rti->eq.count <= EQ_SIZE );
        assert( rti->timestamp <= p[1].time );
        assert( !rti->eq.power_suffice || rti->timestamp <= rti->eq.power_suffice );
    }

    min = p[1];

    if ( !rti->eq.power_suffice || rti->eq.power_suffice >= min.time ) {

        /* Delete from heap. */
        last = p[rti->eq.count--];
        for( i = 1; i << 1 <= rti->eq.count; i = child ) {
            child = i << 1;
            if ( child != rti->eq.count && rti->eq.event[child].time < p[child].time )
                child++;
            if ( last.time > p[child].time )
                p[i] = p[child];
            else
                break;
        }
        p[i] = last;

        /* Now 'min' contains the top priority. Execute it. */
#if ( passive_power_regen )
        power_gain( rti, TO_SECONDS( min.time - rti->timestamp ) * rti->player.power_regen );
#endif
        rti->timestamp = min.time;

        if ( min.routine == EVENT_END_SIMULATION ) /* Finish the simulation here. */
            return 0;

        /* TODO: Some preparations? */
        routine_entries( rti, min );
        /* TODO: Some finishing works? */

    } else {
        /* Invoke power suffice routine. */
#if ( passive_power_regen )
        power_gain( rti, TO_SECONDS( rti->eq.power_suffice - rti->timestamp ) * rti->player.power_regen );
#endif
        rti->timestamp = rti->eq.power_suffice;
        rti->eq.power_suffice = 0;
        /*
            Power suffices would not make any impact, just a reserved APL scanning.
            The scanning will occur when timestamp elapses next time, not immediately.
         */
    }

    return 1;
}

/* Delete an event from EQ. Costly. */
void eq_delete( rtinfo_t* rti, time_t time, k32u routnum ) {
    _event_t* p = &rti->eq.event[-1];
    k32u i = 1, child;
    _event_t last;

    /* Find the event in O(n). */
    while( i <= rti->eq.count ) {
        if ( p[i].time == time && p[i].routine == routnum )
            break;
        i++;
    }
    if ( i > rti->eq.count ) /* Not found? */
        return;

    /* Delete the event in O(logn). */
    last = p[rti->eq.count--];
    for( ; i << 1 <= rti->eq.count; i = child ) {
        child = i << 1;
        if ( child != rti->eq.count && rti->eq.event[child].time < p[child].time )
            child++;
        if ( last.time > p[child].time )
            p[i] = p[child];
        else
            break;
    }
    p[i] = last;

}

float enemy_health_percent( rtinfo_t* rti ) {
    /*
        What differs from SimulationCraft, OpenCL iterations are totally parallelized.
        It's impossible to determine mob initial health by the results from previous iterations.
        The best solution here is to use a linear mix of time to approximate the health precent,
        which is used in SimC for the very first iteration.
    */
    time_t remainder = max( ( k32u )FROM_SECONDS( 0 ), ( k32u )( rti->expected_combat_length - rti->timestamp ) );
    return mix( death_pct, initial_health_percentage, ( float )remainder / ( float )rti->expected_combat_length );
}

void proc_ICD( rtinfo_t* rti, ICD_t* state, float chance, time_t cooldown, k32u routnum ) {
    if ( ( !state->cd || state->cd <= rti->timestamp ) && uni_rng( rti ) < chance ) {
        state->cd = TIME_OFFSET( cooldown );
        eq_enqueue( rti, rti->timestamp, routnum );
    }
}
void proc_PPM( rtinfo_t* rti, float PPM, weapon_t* weapon, k32u routnum ) {
    if ( uni_rng( rti ) < ( PPM * weapon->speed / 60.0f ) ) {
        eq_enqueue( rti, rti->timestamp, routnum );
    }
}
void proc_RPPM( rtinfo_t* rti, RPPM_t* state, float RPPM, k32u routnum ) {
	if (state->lasttimeattemps == rti->timestamp) return;
    float proc = RPPM * min( TO_SECONDS( rti->timestamp - state->lasttimeattemps ), 10.0f ) / 60.0f;
    state->lasttimeattemps = rti->timestamp;
    proc *= max( 1.0f, 1.0f + ( min( TO_SECONDS( rti->timestamp - state->lasttimeprocs ), 1000.0f ) / ( 60.0f / RPPM ) - 1.5f ) * 3.0f );
    if ( uni_rng( rti ) < proc ) {
        eq_enqueue( rti, rti->timestamp, routnum );
        state->lasttimeprocs = rti->timestamp;
    }
}


void sim_init( rtinfo_t* rti, k32u seed ) {
    /* Analogize get_global_id for CPU. */
    hostonly(
        static int gid = 0;
        set_global_id( 0, gid++ );
    )

    /* RNG. */
    rng_init( rti, seed );

    /* Combat length. */
    assert( vary_combat_length < max_length ); /* Vary can't be greater than max. */
    assert( vary_combat_length + max_length < 2147483.647f );
    rti->expected_combat_length = FROM_SECONDS( max_length + vary_combat_length * clamp( stdnor_rng( rti ) * ( 1.0f / 3.0f ), -1.0f, 1.0f ) );

    /* Class module initializer. */
    module_init( rti );

    eq_enqueue( rti, rti->expected_combat_length, EVENT_END_SIMULATION );

}

/* Single iteration logic. */
deviceonly( __kernel ) void sim_iterate(
    deviceonly( __global ) float* dps_result,
    k32u deterministic_seed,
    k32u gear_str,
    k32u gear_crit,
    k32u gear_haste,
    k32u gear_mastery,
    k32u gear_mult,
    k32u gear_vers
) {
    deviceonly( __private ) rtinfo_t _rti;
    /* Write zero to RTI. */
    _rti = ( rtinfo_t ) {
        msvconly( 0 )
    };

    _rti.player.stat.gear_str = gear_str;
    _rti.player.stat.gear_crit = gear_crit;
    _rti.player.stat.gear_haste = gear_haste;
    _rti.player.stat.gear_mastery = gear_mastery;
    _rti.player.stat.gear_mult = gear_mult;
    _rti.player.stat.gear_vers = gear_vers;

    sim_init(
        &_rti,
        ( k32u )deterministic_seed + ( k32u )get_global_id( 0 )
    );

    while( eq_execute( &_rti ) );

    dps_result[get_global_id( 0 )] = _rti.damage_collected / TO_SECONDS( _rti.expected_combat_length );
}

/* Class module. */
void refresh_str( rtinfo_t* rti ) {
    float fstr = (float)rti->player.stat.gear_str;
    k32u str;
    float coeff = 1.0f;
    if ( PLATE_SPECIALIZATION ) coeff *= 1.05f;
    if ( BUFF_STR_AGI_INT ) coeff *= 1.05f;
#if (archmages_incandescence || archmages_greater_incandescence)
	if (UP(incandescence.expire)) coeff *= archmages_greater_incandescence ? 1.15f : 1.1f;
#endif
    str = convert_uint_rtz( fstr * coeff );
    fstr = 1455; /* Base str @lvl 100. */
    fstr += racial_base_str[RACE]; /* Racial str. */
    str += convert_uint_rtz( fstr * coeff );
    rti->player.stat.str = str;
}

void refresh_ap( rtinfo_t* rti ) {
    k32u ap = rti->player.stat.str;
#if (RACE == RACE_ORC)
	if (UP(bloodfury.expire)) ap += 345;
#endif
    if ( BUFF_AP ) ap = convert_uint_rtz( ap * 1.1f + 0.5f );
    rti->player.stat.ap = ap;
}

void refresh_mastery( rtinfo_t* rti ) {
    float mastery = (float)rti->player.stat.gear_mastery;
#if (bleedinghollow_mh)
	if (UP(enchant_mh.expire)) mastery += 500.0f;
#endif
#if (bleedinghollow_oh)
	if (UP(enchant_oh.expire)) mastery += 500.0f;
#endif
    if ( BUFF_MASTERY ) mastery += 550;
    mastery = 1.4f * ( 0.08f + mastery / 11000 );
    rti->player.stat.mastery = mastery;
}

void refresh_crit( rtinfo_t* rti ) {
    float crit = (float)rti->player.stat.gear_crit;
#if (thunderlord_mh)
	if (UP(enchant_mh.expire)) crit += 500.0f;
#endif
#if (thunderlord_oh)
	if (UP(enchant_oh.expire)) crit += 500.0f;
#endif
    crit *= 1.05f;
    crit = 0.05f + crit / 11000;
    if ( BUFF_CRIT ) crit += 0.05f;
    if ( ( RACE == RACE_NIGHTELF_DAY ) || ( RACE == RACE_BLOODELF ) || ( RACE == RACE_WORGEN ) )
        crit += 0.01f;
    rti->player.stat.crit = crit;
}

void refresh_haste( rtinfo_t* rti ) {
    float haste = (float)rti->player.stat.gear_haste;
    haste = 1.0f + haste / 9000;
    if ( BUFF_HASTE ) haste *= 1.05f;
    if ( ( RACE == RACE_NIGHTELF_NIGHT ) || ( RACE == RACE_GOBLIN ) || ( RACE == RACE_GNOME ) )
        haste *= 1.01f;
#if (RACE == RACE_TROLL)
	if (UP(berserking.expire)) haste *= 1.15f;
#endif
    if ( BUFF_BLOODLUST && rti->timestamp ) if ( ( rti->timestamp % FROM_SECONDS( 600 ) ) < FROM_SECONDS( 30 ) ) haste *= 1.3f;
    rti->player.stat.haste = haste - 1.0f;
}

void refresh_mult( rtinfo_t* rti ) {
    float mult = (float)rti->player.stat.gear_mult;
    mult = mult / 6600;
    if ( BUFF_MULT ) mult += 0.05f;
    rti->player.stat.mult = mult;
}

void refresh_vers( rtinfo_t* rti ) {
    float vers = (float)rti->player.stat.gear_vers;
    if ( RACE == RACE_HUMAN ) vers += 100;
    vers = vers / 13000;
    if ( BUFF_VERS ) vers += 0.03f;
    rti->player.stat.vers = vers;
}

float weapon_dmg( rtinfo_t* rti, float weapon_multiplier, kbool normalized, kbool offhand ) {
    float dmg = (float)weapon[offhand].low;
    dmg += uni_rng( rti ) * ( weapon[offhand].high - weapon[offhand].low );
    dmg += ( normalized ? normalized_weapon_speed[weapon[offhand].type] : weapon[offhand].speed ) * rti->player.stat.ap / 3.5f;
    dmg *= weapon_multiplier;
    /* Crazed Berserker */
    if ( offhand ) dmg *= 0.5f * ( SINGLE_MINDED ? 1.5f : 1.25f );
    if ( SINGLE_MINDED ) dmg *= 1.3f;
    if ( UP( enrage.expire ) ) {
        dmg *= 1.1f;
        dmg *= 1.0f + rti->player.stat.mastery;
    }
    dmg *= 1.0f + rti->player.stat.vers;
    return dmg;
}

float ap_dmg( rtinfo_t* rti, float ap_multiplier ) {
    float dmg = ap_multiplier * rti->player.stat.ap;
    if ( SINGLE_MINDED ) dmg *= 1.3f;
    if ( UP( enrage.expire ) ) {
        dmg *= 1.1f;
        dmg *= 1.0f + rti->player.stat.mastery;
    }
    dmg *= 1.0f + rti->player.stat.vers;
    return dmg;
}

/* Event list. */
#define DECL_EVENT( name ) void event_##name ( rtinfo_t* rti )
#define HOOK_EVENT( name ) case routnum_##name: event_##name( rti ); break;
#define DECL_SPELL( name ) void spell_##name ( rtinfo_t* rti )
#define SPELL( name ) spell_##name ( rti )
enum {
    routnum_gcd_expire,
    routnum_bloodthirst_execute,
#if (TALENT_TIER3 != 3)
    routnum_bloodthirst_cd,
#endif
    routnum_ragingblow_execute,
    routnum_ragingblow_trigger,
    routnum_ragingblow_expire,
    routnum_enrage_trigger,
    routnum_enrage_expire,
    routnum_execute_execute,
    routnum_wildstrike_execute,
    routnum_bloodsurge_trigger,
    routnum_bloodsurge_expire,
    routnum_auto_attack_mh,
    routnum_auto_attack_oh,
    routnum_berserkerrage_cd,
    routnum_recklessness_cd,
    routnum_recklessness_execute,
    routnum_recklessness_expire,

#if (TALENT_TIER3 == 2)
    routnum_suddendeath_trigger,
    routnum_suddendeath_expire,
#endif
#if (TALENT_TIER4 == 1)
    routnum_stormbolt_execute,
    routnum_stormbolt_cd,
#endif
#if (TALENT_TIER4 == 2)
    routnum_shockwave_execute,
    routnum_shockwave_cd,
#endif
#if (TALENT_TIER4 == 3)
    routnum_dragonroar_execute,
    routnum_dragonroar_cd,
#endif
#if (TALENT_TIER6 == 1)
    routnum_avatar_start,
    routnum_avatar_expire,
    routnum_avatar_cd,
#endif
#if (TALENT_TIER6 == 2)
    routnum_bloodbath_start,
    routnum_bloodbath_cd,
    routnum_bloodbath_expire,
    routnum_bloodbath_tick,
#endif
#if (TALENT_TIER6 == 3)
    routnum_bladestorm_tick,
    routnum_bladestorm_cd,
#endif
#if (TALENT_TIER7 == 2)
    routnum_ravager_tick,
    routnum_ravager_cd,
#endif
#if (TALENT_TIER7 == 3)
    routnum_siegebreaker_execute,
    routnum_siegebreaker_cd,
#endif

#if (BUFF_BLOODLUST == 1)
    routnum_bloodlust_start,
    routnum_bloodlust_end,
#endif
#if (BUFF_POTION == 1)
    routnum_potion_start,
    routnum_potion_cd,
    routnum_potion_expire,
#endif

#if (t17_4pc)
	routnum_rampage_refresh,
	routnum_rampage_expire,
#endif
#if (archmages_incandescence || archmages_greater_incandescence)
	routnum_incandescence_trigger,
	routnum_incandescence_expire,
#endif
#if (thunderlord_mh || bleedinghollow_mh || shatteredhand_mh)
	routnum_enchant_mh_trigger,
	routnum_enchant_mh_expire,
#endif
#if (shatteredhand_mh)
	routnum_enchant_mh_tick,
#endif
#if (thunderlord_oh || bleedinghollow_oh || shatteredhand_oh)
	routnum_enchant_oh_trigger,
	routnum_enchant_oh_expire,
#endif
#if (shatteredhand_oh)
	routnum_enchant_oh_tick,
#endif
#if (RACE == RACE_BLOODELF)
	routnum_arcanetorrent_cd,
#endif
#if (RACE == RACE_TROLL)
	routnum_berserking_start,
	routnum_berserking_expire,
	routnum_berserking_cd,
#endif
#if (RACE == RACE_ORC)
	routnum_bloodfury_start,
	routnum_bloodfury_expire,
	routnum_bloodfury_cd,
#endif
#if defined(trinket_vial_of_convulsive_shadows)
	routnum_vial_of_convulsive_shadows_expire,
	routnum_vial_of_convulsive_shadows_start,
	routnum_vial_of_convulsive_shadows_cd,
#endif
#if defined(trinket_forgemasters_insignia)
	routnum_forgemasters_insignia_tick,
#endif
#if defined(trinket_horn_of_screaming_spirits)
	routnum_horn_of_screaming_spirits_trigger,
	routnum_horn_of_screaming_spirits_expire,
#endif
#if defined(trinket_scabbard_of_kyanos)
	routnum_scabbard_of_kyanos_expire,
	routnum_scabbard_of_kyanos_start,
	routnum_scabbard_of_kyanos_cd,
#endif
};

enum {
    DMGTYPE_NONE,
    DMGTYPE_MELEE,
    DMGTYPE_SPECIAL,
    DMGTYPE_DRAGONROAR,
};

void special_procs(rtinfo_t* rti);

kbool deal_damage( rtinfo_t* rti, float dmg, k32u dmgtype, float extra_crit_rate ) {
    switch( dmgtype ) {
    case DMGTYPE_NONE:
        lprintf( ( "damage %.0f", dmg ) );
        rti->damage_collected += dmg;
        return 0;
        break;
    default: {
        float c = uni_rng( rti );
        float cr = rti->player.stat.crit - 0.03f + extra_crit_rate;
        float cdb = ( RACE == RACE_DWARF || RACE == RACE_TAUREN ) ? 2.04f : 2.0f;
        kbool ret;
        float fdmg;
#if (TALENT_TIER6 == 2)
        float bbcounter = rti->damage_collected;
#endif

        if ( UP( recklessness.expire ) ) {
            if( dmgtype == DMGTYPE_SPECIAL ) cr += 0.3f;
            cdb *= 1.1f;
        }
#if (t17_4pc)
		if (UP(rampage.expire)){
			cr += 0.06f * rti->player.rampage.stack;
		}
#endif
		if ( dmgtype == DMGTYPE_DRAGONROAR ) cr = 1.0f;
        else dmg *= 0.650684f;
#if (TALENT_TIER6 == 1)
        if ( UP( avatar.expire ) )
            dmg *= 1.2f;
#endif

        fdmg = dmg;
        if ( c < cr ) {
            ret = 1;
            fdmg *= ( RACE == RACE_DWARF || RACE == RACE_TAUREN ) ? 2.04f : 2.0f;
            lprintf( ( "damage *%.0f*", fdmg ) );
#if (thunderlord_mh)
			if (UP(enchant_mh.expire) && rti->player.enchant_mh.extend){
				rti->player.enchant_mh.extend --;
				rti->player.enchant_mh.expire += FROM_SECONDS(2);
				eq_enqueue(rti, rti->player.enchant_mh.expire, routnum_enchant_mh_expire);
			}
#endif
#if (thunderlord_oh)
			if (UP(enchant_oh.expire) && rti->player.enchant_oh.extend){
				rti->player.enchant_oh.extend --;
				rti->player.enchant_oh.expire += FROM_SECONDS(2);
				eq_enqueue(rti, rti->player.enchant_oh.expire, routnum_enchant_oh_expire);
			}
#endif
        } else {
            ret = 0;
            lprintf( ( "damage %.0f", fdmg ) );
        }
        rti->damage_collected += fdmg;

        float mr = rti->player.stat.mult;
        float m = uni_rng( rti );
        if ( m < mr ) {
            c = uni_rng( rti );
            fdmg = dmg * 0.3f;
            if ( c < cr ) {
                fdmg *= cdb;
                lprintf( ( "mult damage *%.0f*", fdmg ) );
            } else {
                lprintf( ( "mult damage %.0f", fdmg ) );
            }
            rti->damage_collected += fdmg;
        }
        m = uni_rng( rti );
        if ( m < mr ) {
            c = uni_rng( rti );
            fdmg = dmg * 0.3f;
            if ( c < cr ) {
                fdmg *= cdb;
                lprintf( ( "mult damage *%.0f*", fdmg ) );
            } else {
                lprintf( ( "mult damage %.0f", fdmg ) );
            }
            rti->damage_collected += fdmg;
        }

#if (TALENT_TIER6 == 2)
        if ( dmgtype != DMGTYPE_MELEE && UP( bloodbath.expire ) ) {
            bbcounter = rti->damage_collected - bbcounter;
            bbcounter *= 0.3f;
            rti->player.bloodbath.pool += bbcounter;
            rti->player.bloodbath.ticks = 6.0f;
            if ( rti->player.bloodbath.dot_start < rti->timestamp ) {
                rti->player.bloodbath.dot_start = rti->timestamp;
                eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 1 ) ), routnum_bloodbath_tick );
            }
        }
#endif
		special_procs(rti);

        return ret;
    }
    break;
    }

}

// === General Cooldown =======================================================
void gcd_start ( rtinfo_t* rti, time_t length ) {
    rti->player.gcd = TIME_OFFSET( length );
    eq_enqueue( rti, rti->player.gcd, routnum_gcd_expire );
}

DECL_EVENT( gcd_expire ) {
    /* Do nothing. */
}

// === Auto-attack ============================================================
DECL_EVENT( auto_attack_mh ) {
    float d = weapon_dmg( rti, 1.0f, 0, 0 );

    if ( uni_rng( rti ) < 0.19f ) {
        /* Miss */
        lprintf( ( "mh miss" ) );
    } else {
        power_gain( rti, 3.5f * weapon[0].speed );
#if (TALENT_TIER3 == 2)
        proc_RPPM( rti, &rti->player.suddendeath.proc, 2.5f * ( 1.0f + rti->player.stat.haste ), routnum_suddendeath_trigger );
#endif
        if( deal_damage( rti, d, DMGTYPE_MELEE, 0 ) ) {
            /* Crit */
            lprintf( ( "mh crit" ) );
        } else {
            /* Hit */
            lprintf( ( "mh hit" ) );
        }
    }
#if (t17_4pc)
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( weapon[0].speed / ( 1.0f + rti->player.stat.haste
		+ 0.06f * rti->player.rampage.stack
		) ) ), routnum_auto_attack_mh );
#else
	eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( weapon[0].speed / ( 1.0f + rti->player.stat.haste ) ) ), routnum_auto_attack_mh );
#endif
}

DECL_EVENT( auto_attack_oh ) {
    float d = weapon_dmg( rti, 1.0f, 0, 1 );

    if ( uni_rng( rti ) < 0.19f ) {
        /* Miss */
        lprintf( ( "oh miss" ) );
    } else {
        power_gain( rti, 3.5f * weapon[1].speed * 0.5f );
#if (TALENT_TIER3 == 2)
        proc_RPPM( rti, &rti->player.suddendeath.proc, 2.5f * ( 1.0f + rti->player.stat.haste ), routnum_suddendeath_trigger );
#endif
        if( deal_damage( rti, d, DMGTYPE_MELEE, 0 ) ) {
            /* Crit */
            lprintf( ( "oh crit" ) );
        } else {
            /* Hit */
            lprintf( ( "oh hit" ) );
        }
    }

#if (t17_4pc)
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( weapon[1].speed / ( 1.0f + rti->player.stat.haste
		+ 0.06f * rti->player.rampage.stack
		) ) ), routnum_auto_attack_oh );
#else
	eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( weapon[1].speed / ( 1.0f + rti->player.stat.haste ) ) ), routnum_auto_attack_oh );
#endif
}

// === enrage =================================================================
DECL_EVENT( enrage_trigger ) {
    power_gain( rti, 10.0f );
    rti->player.enrage.expire = TIME_OFFSET( FROM_SECONDS( 8 ) );
    eq_enqueue( rti, rti->timestamp, routnum_ragingblow_trigger );
    eq_enqueue( rti, rti->player.enrage.expire, routnum_enrage_expire );
    lprintf( ( "enrage trig" ) );
}

DECL_EVENT( enrage_expire ) {
    if ( rti->player.enrage.expire == rti->timestamp ) {
        lprintf( ( "enrage expire" ) );
    }
}

// === bloodthirst ============================================================
DECL_EVENT( bloodthirst_execute ) {
    float d = weapon_dmg( rti, 0.5f, 1, 0 );

    power_gain( rti, 10.0f );
#if (TALENT_TIER3 != 3)
    rti->player.bloodthirst.cd = TIME_OFFSET( FROM_SECONDS( 4.5 / ( 1.0f + rti->player.stat.haste ) ) );
    eq_enqueue( rti, rti->player.bloodthirst.cd, routnum_bloodthirst_cd );
#endif
    if ( uni_rng( rti ) < 0.2f ) {
        eq_enqueue( rti, rti->timestamp, routnum_bloodsurge_trigger );
    }

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0.4f ) ) {
        /* Crit */
        eq_enqueue( rti, rti->timestamp, routnum_enrage_trigger );
        lprintf( ( "bloodthirst crit" ) );

    } else {
        /* Hit */
        lprintf( ( "bloodthirst hit" ) );
    }

}

#if (TALENT_TIER3 != 3)
DECL_EVENT( bloodthirst_cd ) {
    lprintf( ( "bloodthirst ready" ) );
}
#endif

DECL_SPELL( bloodthirst ) {
    if ( rti->player.gcd > rti->timestamp ) return;
#if (TALENT_TIER3 != 3)
    if ( rti->player.bloodthirst.cd > rti->timestamp ) return;
#endif
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
    eq_enqueue( rti, rti->timestamp, routnum_bloodthirst_execute );
    lprintf( ( "cast bloodthirst" ) );
}

// === ragingblow =============================================================
DECL_EVENT( ragingblow_execute ) {
    /* Main hand. */
    float d = weapon_dmg( rti, 2.0f, 1, 0 );

    rti->player.ragingblow.stack --;
    if ( rti->player.ragingblow.stack == 0 ) {
        rti->player.ragingblow.expire = 0;
        eq_enqueue( rti, rti->timestamp, routnum_ragingblow_expire );
        lprintf( ( "ragingblow expire" ) );
    }

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "ragingblow crit" ) );
#if (t17_2pc)
		if (uni_rng(rti) < 0.2){
			eq_enqueue(rti, rti->timestamp, routnum_enrage_trigger);
		}
#endif
    } else {
        /* Hit */
        lprintf( ( "ragingblow hit" ) );
    }

    /* Off hand. */
    d = weapon_dmg( rti, 2.0, 1, 1 );
    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "ragingblow oh crit" ) );
#if (t17_2pc)
		if (uni_rng(rti) < 0.2){
			eq_enqueue(rti, rti->timestamp, routnum_enrage_trigger);
		}
#endif
    } else {
        /* Hit */
        lprintf( ( "ragingblow oh hit" ) );
    }

}

DECL_EVENT( ragingblow_trigger ) {
    rti->player.ragingblow.stack ++;
    rti->player.ragingblow.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
    if ( rti->player.ragingblow.stack > 2 ) {
        rti->player.ragingblow.stack = 2;
    }
    eq_enqueue( rti, rti->player.ragingblow.expire, routnum_ragingblow_expire );
    lprintf( ( "ragingblow stack %d", rti->player.ragingblow.stack ) );
}

DECL_EVENT( ragingblow_expire ) {
    if ( rti->player.ragingblow.expire == rti->timestamp ) {
        rti->player.ragingblow.stack = 0;
        lprintf( ( "ragingblow expire" ) );
    }
}

DECL_SPELL( ragingblow ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !UP( ragingblow.expire ) ) return;
    if ( !power_check( rti, 10.0f ) ) return;
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
    power_consume( rti, 10.0f );
    eq_enqueue( rti, rti->timestamp, routnum_ragingblow_execute );
    lprintf( ( "cast ragingblow" ) );
}

// === execute ================================================================
DECL_EVENT( execute_execute ) {
    /* Main hand. */
    float d = weapon_dmg( rti, 3.5f * 1.2f, 1, 0 );
    if ( SINGLE_MINDED ) d *= 1.15f;

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "execute crit" ) );

    } else {
        /* Hit */
        lprintf( ( "execute hit" ) );
    }

    /* Off hand. */
    d = weapon_dmg( rti, 3.5f * 1.2f, 1, 1 );
    if ( SINGLE_MINDED ) d *= 1.15f;

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "execute oh crit" ) );

    } else {
        /* Hit */
        lprintf( ( "execute oh hit" ) );
    }
}

#if (TALENT_TIER3 == 2)
DECL_EVENT( suddendeath_trigger ) {
    rti->player.suddendeath.expire = TIME_OFFSET( FROM_SECONDS( 10 ) );
    eq_enqueue( rti, rti->player.suddendeath.expire, routnum_suddendeath_expire );
    lprintf( ( "suddendeath trig" ) );
}

DECL_EVENT( suddendeath_expire ) {
    if ( rti->player.suddendeath.expire == rti->timestamp ) {
        lprintf( ( "suddendeath expire" ) );
    }
}
#endif

DECL_SPELL( execute ) {
    if ( rti->player.gcd > rti->timestamp ) return;
#if (TALENT_TIER3 == 2)
    if ( !UP( suddendeath.expire ) ) {
        if ( enemy_health_percent( rti ) >= 20.0f || !power_check( rti, 30.0f ) ) return;
        power_consume( rti, 30.0f );
    } else {
        rti->player.suddendeath.expire = 0;
        eq_enqueue( rti, rti->timestamp, routnum_suddendeath_expire );
#if (TALENT_TIER7 == 1)
        anger_management_count( rti, 30.0f );
#endif
    }
#else
    if ( enemy_health_percent( rti ) >= 20.0f || !power_check( rti, 30.0f ) ) return;
    power_consume( rti, 30.0f );
#endif
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
    eq_enqueue( rti, rti->timestamp, routnum_execute_execute );
    lprintf( ( "cast execute" ) );
}

// === wildstrike =============================================================
DECL_EVENT( wildstrike_execute ) {
    float d = weapon_dmg( rti, 3.75f, 1, 1 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "wildstrike crit" ) );

    } else {
        /* Hit */
        lprintf( ( "wildstrike hit" ) );
    }
}

DECL_EVENT( bloodsurge_trigger ) {
    rti->player.bloodsurge.stack = 2;
    rti->player.bloodsurge.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
    eq_enqueue( rti, rti->player.bloodsurge.expire, routnum_bloodsurge_expire );
    lprintf( ( "bloodsurge trig" ) );
}

DECL_EVENT( bloodsurge_expire ) {
    if ( rti->player.bloodsurge.expire == rti->timestamp ) {
        rti->player.bloodsurge.stack = 0;
        rti->player.bloodsurge.expire = 0;
        lprintf( ( "bloodsurge expire" ) );
    }
}

DECL_SPELL( wildstrike ) {
#if (TALENT_TIER3 == 1)
#define WILDSTRIKE_RAGE_COST 20.0f
#else
#define WILDSTRIKE_RAGE_COST 45.0f
#endif
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !UP( bloodsurge.expire ) ) {
        if ( !power_check( rti, WILDSTRIKE_RAGE_COST ) ) return;
        power_consume( rti, WILDSTRIKE_RAGE_COST );
    } else {
        rti->player.bloodsurge.stack --;
        if ( rti->player.bloodsurge.stack == 0 ) {
            rti->player.bloodsurge.expire = 0;
            eq_enqueue( rti, rti->timestamp, routnum_bloodsurge_expire );
            lprintf( ( "bloodsurge expire" ) );
        }
    }
    gcd_start( rti, FROM_SECONDS( 0.75 ) );
    eq_enqueue( rti, rti->timestamp, routnum_wildstrike_execute );
    lprintf( ( "cast wildstrike" ) );
}

// === bloodlust ==============================================================
#if (BUFF_BLOODLUST == 1)
DECL_EVENT( bloodlust_start ) {
    lprintf( ( "bloodlust start" ) );
    refresh_haste( rti );
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 30 ) ), routnum_bloodlust_end );
}

DECL_EVENT( bloodlust_end ) {
    lprintf( ( "bloodlust end" ) );
    refresh_haste( rti );
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 570 ) ), routnum_bloodlust_start );
}
#endif

// === potion =================================================================
#if (BUFF_POTION == 1)
DECL_EVENT( potion_expire ) {
    lprintf( ( "potion end" ) );
    rti->player.stat.gear_str -= 1000;
    refresh_str( rti );
    refresh_ap( rti );
}

DECL_EVENT( potion_cd ) {
    lprintf( ( "potion cd" ) );
}

DECL_EVENT( potion_start ) {
    lprintf( ( "potion start" ) );
    rti->player.stat.gear_str += 1000;
    refresh_str( rti );
    refresh_ap( rti );
    rti->player.potion.expire = TIME_OFFSET( FROM_SECONDS( 25 ) );
    eq_enqueue( rti, rti->player.potion.expire, routnum_potion_expire );
    
}

DECL_SPELL( potion ) {
    if ( rti->player.potion.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
    eq_enqueue( rti, rti->timestamp, routnum_potion_start );
	if ( rti->timestamp == FROM_SECONDS( 0 ) ) {
        rti->player.potion.cd = TIME_OFFSET( FROM_SECONDS( 60 ) );
        eq_enqueue( rti, rti->player.potion.cd, routnum_potion_cd );
    } else {
        rti->player.potion.cd = rti->expected_combat_length + 1;
    }
}
#endif

// === berserkerrage ==========================================================
DECL_EVENT( berserkerrage_cd ) {
    lprintf( ( "berserkerrage ready" ) );
}

DECL_SPELL( berserkerrage ) {
    if ( rti->player.berserkerrage.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
    rti->player.berserkerrage.cd = TIME_OFFSET( FROM_SECONDS( 30 ) );
    eq_enqueue( rti, rti->player.berserkerrage.cd, routnum_berserkerrage_cd );
    eq_enqueue( rti, rti->timestamp, routnum_enrage_trigger );
    lprintf( ( "cast berserkerrage" ) );
}

// === recklessness ===========================================================
DECL_EVENT( recklessness_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.recklessness.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.recklessness.cd == rti->timestamp ) {
            lprintf( ( "recklessness ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.recklessness.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.recklessness.cd + rti->timestamp ) / 2, routnum_recklessness_cd );
        } else {
            eq_enqueue( rti, rti->player.recklessness.cd, routnum_recklessness_cd );
        }
#endif
}

DECL_EVENT( recklessness_expire ) {
    lprintf( ( "recklessness expire" ) );
}

DECL_EVENT( recklessness_execute ) {
    lprintf( ( "recklessness start" ) );
#if (t17_4pc)
	eq_enqueue(rti, TIME_OFFSET(FROM_SECONDS(1)), routnum_rampage_refresh);
#endif
}

#if (t17_4pc)
DECL_EVENT(rampage_expire) {
	lprintf(("rampage expire"));
	rti->player.rampage.stack = 0;
}

DECL_EVENT(rampage_refresh) {
	if (rti->player.rampage.stack == 0){
		rti->player.rampage.expire = TIME_OFFSET(FROM_SECONDS(14));
		eq_enqueue( rti, rti->player.rampage.expire, routnum_rampage_expire);
	}
	rti->player.rampage.stack++;
	if (rti->player.rampage.stack < 10)
		eq_enqueue(rti, TIME_OFFSET(FROM_SECONDS(1)), routnum_rampage_refresh);
}
#endif

DECL_SPELL( recklessness ) {
    if ( rti->player.recklessness.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
    rti->player.recklessness.cd = TIME_OFFSET( FROM_SECONDS( 180 ) );
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->player.recklessness.cd + rti->timestamp ) / 2, routnum_recklessness_cd );
#else
    eq_enqueue( rti, rti->player.recklessness.cd, routnum_recklessness_cd );
#endif
    rti->player.recklessness.expire = TIME_OFFSET( FROM_SECONDS( 10 ) );
    eq_enqueue( rti, rti->player.recklessness.expire, routnum_recklessness_expire );
    eq_enqueue( rti, rti->timestamp, routnum_recklessness_execute );
    lprintf( ( "cast recklessness" ) );
}

// === stormbolt ==============================================================
#if (TALENT_TIER4 == 1)
DECL_EVENT( stormbolt_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.stormbolt.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.stormbolt.cd == rti->timestamp ) {
            lprintf( ( "stormbolt ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.stormbolt.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.stormbolt.cd + rti->timestamp ) / 2, routnum_stormbolt_cd );
        } else {
            eq_enqueue( rti, rti->player.stormbolt.cd, routnum_stormbolt_cd );
        }
#endif
}

DECL_EVENT( stormbolt_execute ) {
    float d = weapon_dmg( rti, 0.6f * 4.0f, 1, 0 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "stormbolt crit" ) );

    } else {
        /* Hit */
        lprintf( ( "stormbolt hit" ) );
    }
    d = weapon_dmg( rti, 0.6f * 4.0f, 1, 1 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "stormbolt_oh crit" ) );

    } else {
        /* Hit */
        lprintf( ( "stormbolt_oh hit" ) );
    }
}

DECL_SPELL( stormbolt ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.stormbolt.cd > rti->timestamp ) return;
    rti->player.stormbolt.cd = TIME_OFFSET( FROM_SECONDS( 30 ) );
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->timestamp + rti->player.stormbolt.cd ) / 2, routnum_stormbolt_cd );
#else
    eq_enqueue( rti, rti->player.stormbolt.cd, routnum_stormbolt_cd );
#endif
    eq_enqueue( rti, rti->timestamp, routnum_stormbolt_execute );
    lprintf( ( "cast stormbolt" ) );
}
#endif

// === shockwave ==============================================================
#if (TALENT_TIER4 == 2)
DECL_EVENT( shockwave_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.shockwave.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.shockwave.cd == rti->timestamp ) {
            lprintf( ( "shockwave ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.shockwave.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.shockwave.cd + rti->timestamp ) / 2, routnum_shockwave_cd );
        } else {
            eq_enqueue( rti, rti->player.shockwave.cd, routnum_shockwave_cd );
        }
#endif
}

DECL_EVENT( shockwave_execute ) {
    float d = ap_dmg( rti, 1.25f );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "shockwave crit" ) );

    } else {
        /* Hit */
        lprintf( ( "shockwave hit" ) );
    }
}

DECL_SPELL( shockwave ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.shockwave.cd > rti->timestamp ) return;
    rti->player.shockwave.cd = TIME_OFFSET( FROM_SECONDS( 40 ) );
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->timestamp + rti->player.shockwave.cd ) / 2, routnum_shockwave_cd );
#else
    eq_enqueue( rti, rti->player.shockwave.cd, routnum_shockwave_cd );
#endif
    eq_enqueue( rti, rti->timestamp, routnum_shockwave_execute );
    lprintf( ( "cast shockwave" ) );
}
#endif

// === dragonroar =============================================================
#if (TALENT_TIER4 == 3)
DECL_EVENT( dragonroar_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.dragonroar.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.dragonroar.cd == rti->timestamp ) {
            lprintf( ( "dragonroar ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.dragonroar.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.dragonroar.cd + rti->timestamp ) / 2, routnum_dragonroar_cd );
        } else {
            eq_enqueue( rti, rti->player.dragonroar.cd, routnum_dragonroar_cd );
        }
#endif
}

DECL_EVENT( dragonroar_execute ) {
    float d = ap_dmg( rti, 1.65f );

    if ( deal_damage( rti, d, DMGTYPE_DRAGONROAR, 0 ) ) {
        /* Crit */
        lprintf( ( "dragonroar crit" ) );

    } else {
        /* Hit */
        lprintf( ( "dragonroar hit" ) );
    }
}

DECL_SPELL( dragonroar ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.dragonroar.cd > rti->timestamp ) return;
    rti->player.dragonroar.cd = TIME_OFFSET( FROM_SECONDS( 60 ) );
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->timestamp + rti->player.dragonroar.cd ) / 2, routnum_dragonroar_cd );
#else
    eq_enqueue( rti, rti->player.dragonroar.cd, routnum_dragonroar_cd );
#endif
    eq_enqueue( rti, rti->timestamp, routnum_dragonroar_execute );
    lprintf( ( "cast dragonroar" ) );
}
#endif

// === ravager ================================================================
#if (TALENT_TIER7 == 2)
DECL_EVENT( ravager_cd ) {
    if ( rti->player.ravager.cd == rti->timestamp ) {
        lprintf( ( "ravager ready" ) );
    }
}

DECL_EVENT( ravager_tick ) {
    float d = ap_dmg( rti, 0.615f );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "ravager crit" ) );

    } else {
        /* Hit */
        lprintf( ( "ravager hit" ) );
    }
    if ( rti->player.ravager.expire > rti->timestamp )
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 1 ) ), routnum_ravager_tick );
}

DECL_SPELL( ravager ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.ravager.cd > rti->timestamp ) return;
    rti->player.ravager.cd = TIME_OFFSET( FROM_SECONDS( 60 ) );
    rti->player.ravager.expire = TIME_OFFSET( FROM_SECONDS( 10 ) );
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
    eq_enqueue( rti, rti->player.ravager.cd, routnum_ravager_cd );
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 1 ) ), routnum_ravager_tick );
    lprintf( ( "cast ravager" ) );
}
#endif

// === siegebreaker ===========================================================
#if (TALENT_TIER7 == 3)
DECL_EVENT( siegebreaker_cd ) {
    if ( rti->player.siegebreaker.cd == rti->timestamp ) {
        lprintf( ( "siegebreaker ready" ) );
    }
}

DECL_EVENT( siegebreaker_execute ) {
    float d = weapon_dmg( rti, 4.5f, 1, 0 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "siegebreaker crit" ) );

    } else {
        /* Hit */
        lprintf( ( "siegebreaker hit" ) );
    }
    d = weapon_dmg( rti, 4.5f, 1, 1 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "siegebreaker_oh crit" ) );

    } else {
        /* Hit */
        lprintf( ( "siegebreaker_oh hit" ) );
    }
}

DECL_SPELL( siegebreaker ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.siegebreaker.cd > rti->timestamp ) return;
    rti->player.siegebreaker.cd = TIME_OFFSET( FROM_SECONDS( 45 ) );
    gcd_start( rti, FROM_SECONDS( 1.5f / ( 1.0f + rti->player.stat.haste ) ) );
    eq_enqueue( rti, rti->player.siegebreaker.cd, routnum_siegebreaker_cd );
    eq_enqueue( rti, rti->timestamp, routnum_siegebreaker_execute );
    lprintf( ( "cast siegebreaker" ) );
}
#endif

// === bladestorm =============================================================
#if (TALENT_TIER6 == 3)
DECL_EVENT( bladestorm_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.bladestorm.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.bladestorm.cd == rti->timestamp ) {
            lprintf( ( "bladestorm ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.bladestorm.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.bladestorm.cd + rti->timestamp ) / 2, routnum_bladestorm_cd );
        } else {
            eq_enqueue( rti, rti->player.bladestorm.cd, routnum_bladestorm_cd );
        }
#endif
}

DECL_EVENT( bladestorm_tick ) {
    float d = weapon_dmg( rti, 1.6f, 1, 0 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "bladestorm crit" ) );

    } else {
        /* Hit */
        lprintf( ( "bladestorm hit" ) );
    }
    d = weapon_dmg( rti, 1.6f, 1, 1 );

    if ( deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf( ( "bladestorm_oh crit" ) );

    } else {
        /* Hit */
        lprintf( ( "bladestorm_oh hit" ) );
    }
    if ( rti->player.bladestorm.expire > rti->timestamp )
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 1 ) ), routnum_bladestorm_tick );
}

DECL_SPELL( bladestorm ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.bladestorm.cd > rti->timestamp ) return;
    rti->player.bladestorm.cd = TIME_OFFSET( FROM_SECONDS( 60 ) );
    rti->player.bladestorm.expire = TIME_OFFSET( FROM_SECONDS( 6 ) );
    gcd_start( rti, FROM_SECONDS( 6 ) ); /* stuck gcd during bladestorm to avoid additional checks. */
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->timestamp + rti->player.bladestorm.cd ) / 2, routnum_bladestorm_cd );
#else
    eq_enqueue( rti, rti->player.bladestorm.cd, routnum_bladestorm_cd );
#endif
    eq_enqueue( rti, rti->timestamp, routnum_bladestorm_tick );
    lprintf( ( "cast bladestorm" ) );
}
#endif

// === avatar =================================================================
#if (TALENT_TIER6 == 1)
DECL_EVENT( avatar_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.avatar.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.avatar.cd == rti->timestamp ) {
            lprintf( ( "avatar ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.avatar.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.avatar.cd + rti->timestamp ) / 2, routnum_avatar_cd );
        } else {
            eq_enqueue( rti, rti->player.avatar.cd, routnum_avatar_cd );
        }
#endif
}

DECL_EVENT( avatar_start ) {
    rti->player.avatar.expire = TIME_OFFSET( FROM_SECONDS( 20 ) );
    eq_enqueue( rti, rti->player.avatar.expire, routnum_avatar_expire );
}

DECL_EVENT( avatar_expire ) {
    lprintf( ( "avatar expire" ) );
}

DECL_SPELL( avatar ) {
    if ( rti->player.avatar.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
    rti->player.avatar.cd = TIME_OFFSET( FROM_SECONDS( 90 ) );
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->player.avatar.cd + rti->timestamp ) / 2, routnum_avatar_cd );
#else
    eq_enqueue( rti, rti->player.avatar.cd, routnum_avatar_cd );
#endif
    eq_enqueue( rti, rti->timestamp, routnum_avatar_start );
    lprintf( ( "cast avatar" ) );
}
#endif

// === bloodbath ==============================================================
#if (TALENT_TIER6 == 2)
DECL_EVENT( bloodbath_cd ) {
#if (TALENT_TIER7 == 1)
    if ( rti->player.bloodbath.cd < rti->timestamp ) {
        return;
    } else
#endif
        if ( rti->player.bloodbath.cd == rti->timestamp ) {
            lprintf( ( "bloodbath ready" ) );
        }
#if (TALENT_TIER7 == 1)
        else if ( rti->player.bloodbath.cd - rti->timestamp > FROM_MILLISECONDS( 333 ) ) {
            eq_enqueue( rti, ( rti->player.bloodbath.cd + rti->timestamp ) / 2, routnum_bloodbath_cd );
        } else {
            eq_enqueue( rti, rti->player.bloodbath.cd, routnum_bloodbath_cd );
        }
#endif
}

DECL_EVENT( bloodbath_start ) {
    rti->player.bloodbath.expire = TIME_OFFSET( FROM_SECONDS( 12 ) );
    eq_enqueue( rti, rti->player.bloodbath.expire, routnum_bloodbath_expire );
}

DECL_EVENT( bloodbath_expire ) {
    lprintf( ( "bloodbath expire" ) );
}

DECL_EVENT( bloodbath_tick ) {
    if ( rti->player.bloodbath.ticks < 1.0f ) return;
    if ( rti->player.bloodbath.dot_start + FROM_SECONDS( 7.0f - rti->player.bloodbath.ticks ) != rti->timestamp ) return;
    float dmg = rti->player.bloodbath.pool / rti->player.bloodbath.ticks;
    rti->player.bloodbath.pool -= dmg;
    deal_damage( rti, dmg, DMGTYPE_NONE, 0 );
    lprintf( ( "bloodbath ticks" ) );
    rti->player.bloodbath.ticks -= 1.0f;
    if ( rti->player.bloodbath.ticks >= 1.0f )
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 1 ) ), routnum_bloodbath_tick );
}

DECL_SPELL( bloodbath ) {
    if ( rti->player.bloodbath.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
    rti->player.bloodbath.cd = TIME_OFFSET( FROM_SECONDS( 60 ) );
#if (TALENT_TIER7 == 1)
    eq_enqueue( rti, ( rti->player.bloodbath.cd + rti->timestamp ) / 2, routnum_bloodbath_cd );
#else
    eq_enqueue( rti, rti->player.bloodbath.cd, routnum_bloodbath_cd );
#endif
    eq_enqueue( rti, rti->timestamp, routnum_bloodbath_start );
    lprintf( ( "cast bloodbath" ) );
}
#endif

// === legendary ring =========================================================
#if (archmages_incandescence || archmages_greater_incandescence)
DECL_EVENT(incandescence_trigger){
	rti->player.incandescence.expire = TIME_OFFSET(FROM_SECONDS(10));
	eq_enqueue(rti, rti->player.incandescence.expire, routnum_incandescence_expire);
	refresh_str(rti);
	refresh_ap(rti);
	lprintf(("incandescence trigger"));
}
DECL_EVENT(incandescence_expire){
	if (rti->player.incandescence.expire == rti->timestamp){
		lprintf(("incandescence expire"));
		refresh_str(rti);
		refresh_ap(rti);
	}
}
#endif

// === enchants ===============================================================
#if (thunderlord_mh || bleedinghollow_mh || shatteredhand_mh)
DECL_EVENT(enchant_mh_expire){
	if (rti->player.enchant_mh.expire == rti->timestamp){
		lprintf(("mh enchant expire"));
		if (thunderlord_mh) refresh_crit(rti);
		if (bleedinghollow_mh) refresh_mastery(rti);
	}
}

DECL_EVENT(enchant_mh_trigger){
#if (thunderlord_mh)
	rti->player.enchant_mh.expire = TIME_OFFSET(FROM_SECONDS(6));
	rti->player.enchant_mh.extend = 3;
	refresh_crit(rti);
#endif
#if (bleedinghollow_mh)
	rti->player.enchant_mh.expire = TIME_OFFSET(FROM_SECONDS(12));
	refresh_mastery(rti);
#endif
#if (shatteredhand_mh)
	deal_damage(rti, 1500.0f, DMGTYPE_NONE, .0f);	
	rti->player.enchant_mh.expire = TIME_OFFSET(FROM_SECONDS(6));
	rti->player.enchant_mh.ticks = 6.0f;
	eq_enqueue(rti, TIME_OFFSET(FROM_SECONDS(1)), routnum_enchant_mh_tick);
#endif
	eq_enqueue(rti, rti->player.enchant_mh.expire, routnum_enchant_mh_expire);
}
#endif

#if (thunderlord_oh || bleedinghollow_oh || shatteredhand_oh)
DECL_EVENT(enchant_oh_expire){
	if (rti->player.enchant_oh.expire == rti->timestamp){
		lprintf(("oh enchant expire"));
		if (thunderlord_oh) refresh_crit(rti);
		if (bleedinghollow_oh) refresh_mastery(rti);
	}
}

DECL_EVENT(enchant_oh_trigger){
#if (thunderlord_oh)
	rti->player.enchant_oh.expire = TIME_OFFSET(FROM_SECONDS(6));
	rti->player.enchant_oh.extend = 3;
	refresh_crit(rti);
#endif
#if (bleedinghollow_oh)
	rti->player.enchant_oh.expire = TIME_OFFSET(FROM_SECONDS(12));
	refresh_mastery(rti);
#endif
#if (shatteredhand_oh)
	deal_damage(rti, 1500.0f, DMGTYPE_NONE, .0f);
	rti->player.enchant_oh.expire = TIME_OFFSET(FROM_SECONDS(6));
	rti->player.enchant_oh.ticks = 6.0f;
	eq_enqueue(rti, TIME_OFFSET(FROM_SECONDS(1)), routnum_enchant_oh_tick);
#endif
	eq_enqueue(rti, rti->player.enchant_oh.expire, routnum_enchant_oh_expire);
}
#endif

#if (shatteredhand_mh)
DECL_EVENT(enchant_mh_tick){
	if ( TIME_OFFSET(FROM_SECONDS(rti->player.enchant_mh.ticks - 1.0f)) == rti->player.enchant_mh.expire ){
		rti->player.enchant_mh.ticks -= 1.0f;
		deal_damage(rti, 750.0f, DMGTYPE_NONE, .0f);
		if (rti->player.enchant_mh.ticks >= 1.0f)
			eq_enqueue(rti, TIME_OFFSET(FROM_SECONDS(1)), routnum_enchant_mh_tick);
	}
}
#endif
#if (shatteredhand_oh)
DECL_EVENT(enchant_oh_tick){
	if ( TIME_OFFSET(FROM_SECONDS(rti->player.enchant_oh.ticks - 1.0f)) == rti->player.enchant_oh.expire ){
		rti->player.enchant_oh.ticks -= 1.0f;
		deal_damage(rti, 750.0f, DMGTYPE_NONE, .0f);
		if (rti->player.enchant_oh.ticks >= 1.0f)
			eq_enqueue(rti, TIME_OFFSET(FROM_SECONDS(1)), routnum_enchant_oh_tick);
	}
}
#endif

// === racial traits ==========================================================
#if (RACE == RACE_BLOODELF)
DECL_EVENT(arcanetorrent_cd){
	lprintf(("arcanetorrent ready"));
}

DECL_SPELL(arcanetorrent){
    if ( rti->player.arcanetorrent.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
    rti->player.arcanetorrent.cd = TIME_OFFSET( FROM_SECONDS( 120 ) );
    eq_enqueue( rti, rti->player.arcanetorrent.cd, routnum_arcanetorrent_cd );
	power_gain(rti, 15.0f);
    lprintf( ( "cast arcanetorrent" ) );
}
#endif

#if (RACE == RACE_TROLL)
DECL_EVENT(berserking_cd){
	lprintf(("berserking ready"));
}
DECL_EVENT(berserking_start){
	lprintf(("berserking start"));
	refresh_haste(rti);
}
DECL_EVENT(berserking_expire){
	lprintf(("berserking expire"));
	refresh_haste(rti);
}

DECL_SPELL(berserking){
    if ( rti->player.berserking.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
	eq_enqueue( rti, rti->timestamp, routnum_berserking_start );
	rti->player.berserking.expire = TIME_OFFSET( FROM_SECONDS( 10 ) );
	eq_enqueue( rti, rti->player.berserking.expire, routnum_berserking_expire );
    rti->player.berserking.cd = TIME_OFFSET( FROM_SECONDS( 180 ) );
    eq_enqueue( rti, rti->player.berserking.cd, routnum_berserking_cd );
    lprintf( ( "cast berserking" ) );
}
#endif

#if (RACE == RACE_ORC)
DECL_EVENT(bloodfury_cd){
	lprintf(("bloodfury ready"));
}
DECL_EVENT(bloodfury_start){
	lprintf(("bloodfury start"));
	refresh_ap(rti);
}
DECL_EVENT(bloodfury_expire){
	lprintf(("bloodfury expire"));
	refresh_ap(rti);
}

DECL_SPELL(bloodfury){
    if ( rti->player.bloodfury.cd > rti->timestamp ) return;
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
	eq_enqueue( rti, rti->timestamp, routnum_bloodfury_start );
	rti->player.bloodfury.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
	eq_enqueue( rti, rti->player.bloodfury.expire, routnum_bloodfury_expire );
    rti->player.bloodfury.cd = TIME_OFFSET( FROM_SECONDS( 120 ) );
    eq_enqueue( rti, rti->player.bloodfury.cd, routnum_bloodfury_cd );
    lprintf( ( "cast bloodfury" ) );
}
#endif

// === trinkets ===============================================================
#if defined(trinket_vial_of_convulsive_shadows)
DECL_EVENT(vial_of_convulsive_shadows_cd){
	lprintf(("convulsiveshadows ready"));
}
DECL_EVENT(vial_of_convulsive_shadows_start){
	lprintf(("convulsiveshadows start"));
	rti->player.stat.gear_mult += trinket_vial_of_convulsive_shadows;
	refresh_mult(rti);
}
DECL_EVENT(vial_of_convulsive_shadows_expire){
	lprintf(("convulsiveshadows expire"));
	rti->player.stat.gear_mult -= trinket_vial_of_convulsive_shadows;
	refresh_mult(rti);
}

DECL_SPELL(vial_of_convulsive_shadows){
    if ( rti->player.vial_of_convulsive_shadows.cd > rti->timestamp ) return;
#if defined(trinket_scabbard_of_kyanos)
	if ( UP(scabbard_of_kyanos.expire) ) return;
#endif
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
	eq_enqueue( rti, rti->timestamp, routnum_vial_of_convulsive_shadows_start );
	rti->player.vial_of_convulsive_shadows.expire = TIME_OFFSET( FROM_SECONDS( 20 ) );
	eq_enqueue( rti, rti->player.vial_of_convulsive_shadows.expire, routnum_vial_of_convulsive_shadows_expire );
    rti->player.vial_of_convulsive_shadows.cd = TIME_OFFSET( FROM_SECONDS( 120 ) );
    eq_enqueue( rti, rti->player.vial_of_convulsive_shadows.cd, routnum_vial_of_convulsive_shadows_cd );
    lprintf( ( "cast vial_of_convulsive_shadows" ) );
}
#endif

#if defined(trinket_forgemasters_insignia)
DECL_EVENT(forgemasters_insignia_tick){
	if (rti->player.forgemasters_insignia.stack < 20){
		if (rti->player.forgemasters_insignia.stack == 0){
			rti->player.forgemasters_insignia.expire = TIME_OFFSET(FROM_SECONDS(10));
			lprintf(("forgemasters_insignia start"));
		}
		rti->player.forgemasters_insignia.stack++;
		rti->player.stat.gear_mult += trinket_forgemasters_insignia;
		refresh_mult(rti);
		eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS(0.5) ), routnum_forgemasters_insignia_tick);
	}
	else {
		rti->player.forgemasters_insignia.stack = 0;
		rti->player.stat.gear_mult -= 20 * trinket_forgemasters_insignia;
		refresh_mult(rti);
		lprintf(("forgemasters_insignia expire"));
	}
}

#endif

#if defined(trinket_horn_of_screaming_spirits)
DECL_EVENT(horn_of_screaming_spirits_trigger){
	lprintf(("horn_of_screaming_spirits start"));
	rti->player.stat.gear_mastery += trinket_horn_of_screaming_spirits;
	refresh_mastery(rti);
	rti->player.horn_of_screaming_spirits.expire = TIME_OFFSET(FROM_SECONDS(10));
	eq_enqueue(rti, rti->player.horn_of_screaming_spirits.expire, routnum_horn_of_screaming_spirits_expire);
}
DECL_EVENT(horn_of_screaming_spirits_expire){
	lprintf(("horn_of_screaming_spirits expire"));
	rti->player.stat.gear_mastery -= trinket_horn_of_screaming_spirits;
	refresh_mastery(rti);
}
#endif

#if defined(trinket_scabbard_of_kyanos)
DECL_EVENT(scabbard_of_kyanos_cd){
	lprintf(("scabbard_of_kyanos ready"));
}
DECL_EVENT(scabbard_of_kyanos_start){
	lprintf(("scabbard_of_kyanos start"));
	rti->player.stat.gear_str += trinket_scabbard_of_kyanos;
	refresh_str(rti);
	refresh_ap(rti);
}
DECL_EVENT(scabbard_of_kyanos_expire){
	lprintf(("scabbard_of_kyanos expire"));
	rti->player.stat.gear_str -= trinket_scabbard_of_kyanos;
	refresh_str(rti);
	refresh_ap(rti);
}

DECL_SPELL(scabbard_of_kyanos){
    if ( rti->player.scabbard_of_kyanos.cd > rti->timestamp ) return;
#if defined(trinket_vial_of_convulsive_shadows)
	if ( UP(vial_of_convulsive_shadows.expire) ) return;
#endif
#if (TALENT_TIER6 == 3)
    if ( UP( bladestorm.expire ) ) return;
#endif
	eq_enqueue( rti, rti->timestamp, routnum_scabbard_of_kyanos_start );
	rti->player.scabbard_of_kyanos.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
	eq_enqueue( rti, rti->player.scabbard_of_kyanos.expire, routnum_scabbard_of_kyanos_expire );
    rti->player.scabbard_of_kyanos.cd = TIME_OFFSET( FROM_SECONDS( 90 ) );
    eq_enqueue( rti, rti->player.scabbard_of_kyanos.cd, routnum_scabbard_of_kyanos_cd );
    lprintf( ( "cast scabbard_of_kyanos" ) );
}
#endif

// === anger_management =======================================================
void anger_management_count( rtinfo_t* rti, float rage ) {
    time_t t = FROM_SECONDS( rage / 30.0f );
    if ( rti->player.recklessness.cd > t )
        rti->player.recklessness.cd = max( rti->timestamp, rti->player.recklessness.cd - t );
    else
        rti->player.recklessness.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.recklessness.cd == rti->timestamp )
        eq_enqueue( rti, rti->timestamp, routnum_recklessness_cd );
#if (TALENT_TIER4 == 1)
    if ( rti->player.stormbolt.cd > t )
        rti->player.stormbolt.cd = max( rti->timestamp, rti->player.stormbolt.cd - t );
    else
        rti->player.stormbolt.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.stormbolt.cd == rti->timestamp )
        eq_enqueue( rti, rti->player.stormbolt.cd, routnum_stormbolt_cd );
#endif
#if (TALENT_TIER4 == 2)
    if ( rti->player.shockwave.cd > t )
        rti->player.shockwave.cd = max( rti->timestamp, rti->player.shockwave.cd - t );
    else
        rti->player.shockwave.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.shockwave.cd == rti->timestamp )
        eq_enqueue( rti, rti->player.shockwave.cd, routnum_shockwave_cd );
#endif
#if (TALENT_TIER4 == 3)
    if ( rti->player.dragonroar.cd > t )
        rti->player.dragonroar.cd = max( rti->timestamp, rti->player.dragonroar.cd - t );
    else
        rti->player.dragonroar.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.dragonroar.cd == rti->timestamp )
        eq_enqueue( rti, rti->player.dragonroar.cd, routnum_dragonroar_cd );
#endif
#if (TALENT_TIER6 == 1)
    if ( rti->player.avatar.cd > t )
        rti->player.avatar.cd = max( rti->timestamp, rti->player.avatar.cd - t );
    else
        rti->player.avatar.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.avatar.cd == rti->timestamp )
        eq_enqueue( rti, rti->player.avatar.cd, routnum_avatar_cd );
#endif
#if (TALENT_TIER6 == 2)
    if ( rti->player.bloodbath.cd > t )
        rti->player.bloodbath.cd = max( rti->timestamp, rti->player.bloodbath.cd - t );
    else
        rti->player.bloodbath.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.bloodbath.cd == rti->timestamp )
        eq_enqueue( rti, rti->player.bloodbath.cd, routnum_bloodbath_cd );
#endif
#if (TALENT_TIER6 == 3)
    if ( rti->player.bladestorm.cd > t )
        rti->player.bladestorm.cd = max( rti->timestamp, rti->player.bladestorm.cd - t );
    else
        rti->player.bladestorm.cd = max( rti->timestamp, FROM_SECONDS( 0 ) );
    if ( rti->player.bladestorm.cd == rti->timestamp )
        eq_enqueue( rti, rti->player.bladestorm.cd, routnum_bladestorm_cd );
#endif
}

void routine_entries( rtinfo_t* rti, _event_t e ) {
    switch( e.routine ) {
        HOOK_EVENT( gcd_expire );
        HOOK_EVENT( bloodthirst_execute );
        HOOK_EVENT( ragingblow_execute );
        HOOK_EVENT( ragingblow_trigger );
        HOOK_EVENT( ragingblow_expire );
        HOOK_EVENT( enrage_trigger );
        HOOK_EVENT( enrage_expire );
        HOOK_EVENT( execute_execute );
        HOOK_EVENT( wildstrike_execute );
        HOOK_EVENT( bloodsurge_trigger );
        HOOK_EVENT( bloodsurge_expire );
        HOOK_EVENT( auto_attack_mh );
        HOOK_EVENT( auto_attack_oh );
        HOOK_EVENT( recklessness_cd );
        HOOK_EVENT( recklessness_execute );
        HOOK_EVENT( recklessness_expire );
        HOOK_EVENT( berserkerrage_cd );

#if (TALENT_TIER3 == 2)
        HOOK_EVENT( suddendeath_trigger );
        HOOK_EVENT( suddendeath_expire );
#endif
#if (TALENT_TIER3 != 3)
        HOOK_EVENT( bloodthirst_cd );
#endif
#if (TALENT_TIER4 == 1)
        HOOK_EVENT( stormbolt_execute );
        HOOK_EVENT( stormbolt_cd );
#endif
#if (TALENT_TIER4 == 2)
        HOOK_EVENT( shockwave_execute );
        HOOK_EVENT( shockwave_cd );
#endif
#if (TALENT_TIER4 == 3)
        HOOK_EVENT( dragonroar_execute );
        HOOK_EVENT( dragonroar_cd );
#endif
#if (TALENT_TIER6 == 1)
        HOOK_EVENT( avatar_start );
        HOOK_EVENT( avatar_expire );
        HOOK_EVENT( avatar_cd );
#endif
#if (TALENT_TIER6 == 2)
        HOOK_EVENT( bloodbath_start );
        HOOK_EVENT( bloodbath_expire );
        HOOK_EVENT( bloodbath_cd );
        HOOK_EVENT( bloodbath_tick );
#endif
#if (TALENT_TIER6 == 3)
        HOOK_EVENT( bladestorm_tick );
        HOOK_EVENT( bladestorm_cd );
#endif
#if (TALENT_TIER7 == 2)
        HOOK_EVENT( ravager_tick );
        HOOK_EVENT( ravager_cd );
#endif
#if (TALENT_TIER7 == 3)
        HOOK_EVENT( siegebreaker_execute );
        HOOK_EVENT( siegebreaker_cd );
#endif
#if (BUFF_BLOODLUST == 1)
        HOOK_EVENT( bloodlust_start );
        HOOK_EVENT( bloodlust_end );
#endif
#if (BUFF_POTION == 1)
        HOOK_EVENT( potion_start );
        HOOK_EVENT( potion_cd );
        HOOK_EVENT( potion_expire );
#endif
#if (t17_4pc)
		HOOK_EVENT( rampage_refresh );
		HOOK_EVENT( rampage_expire );
#endif
#if (archmages_incandescence || archmages_greater_incandescence)
		HOOK_EVENT( incandescence_trigger );
		HOOK_EVENT( incandescence_expire );
#endif
#if (thunderlord_mh || bleedinghollow_mh || shatteredhand_mh)
		HOOK_EVENT( enchant_mh_trigger );
		HOOK_EVENT( enchant_mh_expire );
#endif
#if (shatteredhand_mh)
		HOOK_EVENT( enchant_mh_tick );
#endif
#if (thunderlord_oh || bleedinghollow_oh || shatteredhand_oh)
		HOOK_EVENT( enchant_oh_trigger );
		HOOK_EVENT( enchant_oh_expire );
#endif
#if (shatteredhand_oh)
		HOOK_EVENT( enchant_oh_tick );
#endif
#if (RACE == RACE_BLOODELF)
		HOOK_EVENT( arcanetorrent_cd );
#endif
#if (RACE == RACE_TROLL)
		HOOK_EVENT( berserking_start );
		HOOK_EVENT( berserking_expire );
		HOOK_EVENT( berserking_cd );
#endif
#if (RACE == RACE_ORC)
		HOOK_EVENT( bloodfury_start );
		HOOK_EVENT( bloodfury_expire );
		HOOK_EVENT( bloodfury_cd );
#endif
#if defined(trinket_vial_of_convulsive_shadows)
		HOOK_EVENT( vial_of_convulsive_shadows_expire );
		HOOK_EVENT( vial_of_convulsive_shadows_start );
		HOOK_EVENT( vial_of_convulsive_shadows_cd );
#endif
#if defined(trinket_forgemasters_insignia)
		HOOK_EVENT( forgemasters_insignia_tick );
#endif
#if defined(trinket_horn_of_screaming_spirits)
		HOOK_EVENT( horn_of_screaming_spirits_trigger );
		HOOK_EVENT( horn_of_screaming_spirits_expire );
#endif
#if defined(trinket_scabbard_of_kyanos)
		HOOK_EVENT( scabbard_of_kyanos_expire );
		HOOK_EVENT( scabbard_of_kyanos_start );
		HOOK_EVENT( scabbard_of_kyanos_cd );
#endif
    default:
        assert( 0 );
    }
}

void module_init( rtinfo_t* rti ) {
#if (passive_power_regen)
    rti->player.power_regen = 0.0f;
#endif
	rti->player.power = 0.0f;
    eq_enqueue( rti, rti->timestamp, routnum_auto_attack_mh );
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 0.5 ) ), routnum_auto_attack_oh );

    refresh_str( rti );
    refresh_ap( rti );
    refresh_crit( rti );
    refresh_haste( rti );
    refresh_mastery( rti );
    refresh_mult( rti );
    refresh_vers( rti );

    lprintf( ( "Raid buffed str %d", rti->player.stat.str ) );
    lprintf( ( "Raid buffed ap %d", rti->player.stat.ap ) );
    lprintf( ( "Raid buffed crit %f", rti->player.stat.crit ) );
    lprintf( ( "Raid buffed haste %f", rti->player.stat.haste ) );
    lprintf( ( "Raid buffed mastery %f", rti->player.stat.mastery ) );
    lprintf( ( "Raid buffed mult %f", rti->player.stat.mult ) );
    lprintf( ( "Raid buffed vers %f", rti->player.stat.vers ) );

#if (TALENT_TIER3 == 2)
    rti->player.suddendeath.proc.lasttimeattemps = (time_t)-(k32s)FROM_SECONDS( 10 );
    rti->player.suddendeath.proc.lasttimeprocs = (time_t)-(k32s)FROM_SECONDS( 180 );
#endif
#if (BUFF_BLOODLUST == 1)
    eq_enqueue( rti, FROM_MILLISECONDS(1), routnum_bloodlust_start );
#endif
#if (BUFF_POTION == 1)
	SPELL(potion);
#endif
#if (archmages_incandescence || archmages_greater_incandescence)
	rti->player.incandescence.proc.lasttimeattemps = (time_t)-(k32s)FROM_SECONDS(10);
	rti->player.incandescence.proc.lasttimeprocs = (time_t)-(k32s)FROM_SECONDS(180);
#endif
#if (thunderlord_mh || bleedinghollow_mh || shatteredhand_mh)
	rti->player.enchant_mh.proc.lasttimeattemps = (time_t)-(k32s)FROM_SECONDS(10);
	rti->player.enchant_mh.proc.lasttimeprocs = (time_t)-(k32s)FROM_SECONDS(180);
#endif
#if (thunderlord_oh || bleedinghollow_oh || shatteredhand_oh)
	rti->player.enchant_oh.proc.lasttimeattemps = (time_t)-(k32s)FROM_SECONDS(10);
	rti->player.enchant_oh.proc.lasttimeprocs = (time_t)-(k32s)FROM_SECONDS(180);
#endif
#if defined(trinket_forgemasters_insignia)
	rti->player.forgemasters_insignia.proc.lasttimeattemps = (time_t)-(k32s)FROM_SECONDS(10);
	rti->player.forgemasters_insignia.proc.lasttimeprocs = (time_t)-(k32s)FROM_SECONDS(180);
#endif
#if defined(trinket_horn_of_screaming_spirits)
	rti->player.horn_of_screaming_spirits.proc.lasttimeattemps = (time_t)-(k32s)FROM_SECONDS(10);
	rti->player.horn_of_screaming_spirits.proc.lasttimeprocs = (time_t)-(k32s)FROM_SECONDS(180);
#endif
}

void special_procs(rtinfo_t* rti){
#if (archmages_incandescence || archmages_greater_incandescence)
	if (!UP(incandescence.expire)){
		proc_RPPM(rti, &rti->player.incandescence.proc, 0.92f, routnum_incandescence_trigger);
	}
#endif
#if (thunderlord_mh)
	proc_RPPM(rti, &rti->player.enchant_mh.proc, 2.5f, routnum_enchant_mh_trigger);
#elif (bleedinghollow_mh)
	proc_RPPM(rti, &rti->player.enchant_mh.proc, 2.3f, routnum_enchant_mh_trigger);
#elif (shatteredhand_mh)
	proc_RPPM(rti, &rti->player.enchant_mh.proc, 3.5f * ( 1.0f + rti->player.stat.haste ), routnum_enchant_mh_trigger);
#endif
#if (thunderlord_oh)
	proc_RPPM(rti, &rti->player.enchant_oh.proc, 2.5f, routnum_enchant_oh_trigger);
#elif (bleedinghollow_oh)
	proc_RPPM(rti, &rti->player.enchant_oh.proc, 2.3f, routnum_enchant_oh_trigger);
#elif (shatteredhand_oh)
	proc_RPPM(rti, &rti->player.enchant_oh.proc, 3.5f * ( 1.0f + rti->player.stat.haste ), routnum_enchant_oh_trigger);
#endif
#if defined(trinket_forgemasters_insignia)
	if (!UP(forgemasters_insignia.expire)){
		proc_RPPM(rti, &rti->player.forgemasters_insignia.proc, 0.92f, routnum_forgemasters_insignia_tick);
	}
#endif
#if defined(trinket_horn_of_screaming_spirits)
	if (!UP(horn_of_screaming_spirits.expire)){
		proc_RPPM(rti, &rti->player.horn_of_screaming_spirits.proc, 0.92f, routnum_horn_of_screaming_spirits_trigger);
	}
#endif
}

/* Debug build. */
#if !defined(__OPENCL_VERSION__)
void scan_apl( rtinfo_t* rti ) {
    SPELL( recklessness );
    //SPELL( bloodbath );
    //SPELL( bloodthirst );
    //SPELL( execute );
    //SPELL( ragingblow );
    //SPELL( wildstrike );
}

void host_kernel_entry() {
    float result;
    sim_iterate( &result, 5171, 4313 + 250, 2148 + 125, 751, 1504, 478, 0 );

    printf( "result: %.3f\nmax queue length: %d\nruntime state size: %d\n", result, maxqueuelength, sizeof( rtinfo_t ) );
}
#endif /* !defined(__OPENCL_VERSION__) */



