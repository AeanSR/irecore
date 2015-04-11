/**
    frm.c: back-end framework.
    2014.7.14
*/

#define SHOW_LOG
#define passive_power_regen 0

#if !defined(__OPENCL_VERSION__)
/* Front-end gives these defines. */
#define vary_combat_length 20.0f
#define max_length 450.0f
#define initial_health_percentage 100.0f
#define death_pct 0.0f
#define iterations 1
#define power_max 120.0f
#define PLATE_SPECIALIZATION 0
#define SINGLE_MINDED 1
#define BUFF_STR_AGI_INT 1
#define BUFF_AP 1
#define BUFF_CRIT 1
#define BUFF_HASTE 1
#define BUFF_MASTERY 1
#define BUFF_MULT 1
#define BUFF_VERS 1
#define BUFF_SP 1
#define BUFF_STA 1
#define RACE RACE_NONE
#define MH_LOW 814
#define MH_HIGH 1514
#define MH_SPEED 2.6f
#define MH_TYPE WEAPON_1H
#define OH_LOW 814
#define OH_HIGH 1514
#define OH_SPEED 2.6f
#define OH_TYPE WEAPON_1H
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
#endif /* !defined(__OPENCL_VERSION__) */

/* Seed struct which holds the current state. */
typedef struct {
    k32u mt[4]; /* State words. */
    k32u mti;   /* State counter: must be within [0,3]. */
} seed_t;

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
    k32u snapshot;
} _event_t;
typedef struct {
    k32u count;
    time_t power_suffice;
    _event_t event[EQ_SIZE];
} event_queue_t;

/* Declarations from class modules. */
typedef struct {
    time_t cd;
} bloodthirst_t;
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
} sudden_death_t;

typedef struct {
    time_t cd;
} ICD_t;
typedef struct {
    time_t lasttimeattemps;
    time_t lasttimeprocs;
} RPPM_t;
/* Snapshot saves past states. */
#define SNAPSHOT_SIZE (64)
typedef struct {
    float fp[1];
    k32u ip[1];
} snapshot_t;

typedef struct {
    snapshot_t* buffer;
    k64u bitmap;
} snapshot_manager_t;

typedef struct weapon_t {
    float speed;
    k32u low;
    k32u high;
    k32u type;
} weapon_t;

#define WEAPON_2H 0
#define WEAPON_1H 1
#define WEAPON_DAGGER 2

deviceonly(__constant) weapon_t weapon[] = {
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
typedef struct kdeclspec( packed ) {
    float power;
    float power_regen;
    stat_t stat;

    bloodthirst_t   bloodthirst;
    ragingblow_t    ragingblow;
    enrage_t        enrage;
    bloodsurge_t    bloodsurge;
    sudden_death_t  sudden_death;

    RPPM_t		sudden_death_proc;

    time_t gcd;
}
player_t;

/* Runtime info struct, each thread preserves its own. */
typedef struct kdeclspec( packed ) {
    seed_t seed;
    time_t timestamp;
    event_queue_t eq;
    snapshot_manager_t snapshot_manager;
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
void rng_init( rtinfo_t* rti, k32u seed ) {
    rti->seed.mti = 0; /* Reset counter */
    /* Use a LCG to fill state matrix. See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    rti->seed.mt[0] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 1;
    rti->seed.mt[1] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 2;
    rti->seed.mt[2] = seed & 0xffffffffUL;
    /* Due to multiple run for same kernel, set thread id into state words to avoid seed overlapping. */
    seed = (k32u)get_global_id(0);
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 3;
    rti->seed.mt[3] = seed & 0xffffffffUL;
}

/* Generate one IEEE-754 single precision float point uniformally distributed in the interval [.0f, 1.0f). */
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
_event_t* eq_enqueue( rtinfo_t* rti, time_t trigger, k32u routine, k32u snapshot ) {
    k32u i = ++( rti->eq.count );
    _event_t* p = &( rti->eq.event[-1] );

    assert( rti->eq.count <= EQ_SIZE ); /* Full check. */

    /*
        There are two circumstances which could cause the assert below fail:
        1. Devs got something wrong in the class module, enqueued an event happens before 'now'.
        2. Time register is about to overflow, the triggering delay + current timestamp have exceeded the max representable time.
        Since the later circumstance is not a fault, we would just throw the event away and continue quietly.
        When you are exceeding the max time limits, all new events will be thrown, and finally you will get an empty EQ,
        then the empty checks on EQ will fail.
    */
    if ( rti->timestamp <= trigger ) {
        for( ; i > 1 && p[i >> 1].time > trigger; i >>= 1 )
            p[i] = p[i >> 1];
        p[i] = ( _event_t ) {
            .time = trigger, .routine = routine, .snapshot = snapshot
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
    if ( passive_power_regen && rti->player.power_regen > 0 )
        eq_enqueue_ps( rti, TIME_OFFSET( FROM_SECONDS( ( cost - rti->player.power ) / rti->player.power_regen ) ) );
    return 0;
}

/* Power consume. */
void power_consume( rtinfo_t* rti, float cost ) {
    assert( power_check( rti, cost ) ); /* Power should suffice. */
    rti->player.power -= cost;
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
    if ( rti->timestamp < p[1].time &&
            ( !rti->eq.power_suffice || rti->timestamp < rti->eq.power_suffice ) ) {
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
        if ( passive_power_regen )
            power_gain( rti, TO_SECONDS( min.time - rti->timestamp ) * rti->player.power_regen );
        rti->timestamp = min.time;

        if ( min.routine == EVENT_END_SIMULATION ) /* Finish the simulation here. */
            return 0;

        /* TODO: Some preparations? */
        routine_entries( rti, min );
        /* TODO: Some finishing works? */

    } else {
        /* Invoke power suffice routine. */
        if ( passive_power_regen )
            power_gain( rti, TO_SECONDS( rti->eq.power_suffice - rti->timestamp ) * rti->player.power_regen );
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

k32u snapshot_alloc( rtinfo_t* rti, snapshot_t** snapshot ) {
    k32u no;
    assert( rti->snapshot_manager.bitmap ); /* Full check. */
    no = clz( rti->snapshot_manager.bitmap ); /* Get first available place. */
    rti->snapshot_manager.bitmap &= ~( K64U_MSB >> no ); /* Mark as occupied. */
    *snapshot = &rti->snapshot_manager.buffer[ no ];
    return no;
}

snapshot_t* snapshot_kill( rtinfo_t* rti, k32u no ) {
    assert( no < SNAPSHOT_SIZE ); /* Subscript check. */
    assert( ~rti->snapshot_manager.bitmap & ( K64U_MSB >> no ) ); /* Existance check. */
    rti->snapshot_manager.bitmap |= K64U_MSB >> no; /* Mark as available. */
    return &( rti->snapshot_manager.buffer[ no ] );
}

snapshot_t* snapshot_read( rtinfo_t* rti, k32u no ) {
    assert( no < SNAPSHOT_SIZE ); /* Subscript check. */
    assert( ~rti->snapshot_manager.bitmap & ( K64U_MSB >> no ) ); /* Existance check. */
    return &( rti->snapshot_manager.buffer[ no ] );
}

void snapshot_init( rtinfo_t* rti, snapshot_t* buffer ) {
    rti->snapshot_manager.bitmap = ~ K64U_C( 0 ); /* every bit is set to available. */
    rti->snapshot_manager.buffer = buffer;
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

void proc_ICD(rtinfo_t* rti, ICD_t* state, float chance, time_t cooldown, k32u routnum, k32u snapshot) {
    if ((!state->cd || state->cd <= rti->timestamp) && uni_rng(rti) < chance) {
        state->cd = TIME_OFFSET(cooldown);
        eq_enqueue(rti, rti->timestamp, routnum, snapshot);
    }
}
void proc_PPM(rtinfo_t* rti, float PPM, weapon_t* weapon, k32u routnum, k32u snapshot) {
    if (uni_rng(rti) < (PPM * weapon->speed / 60.0f)) {
        eq_enqueue(rti, rti->timestamp, routnum, snapshot);
    }
}
void proc_RPPM(rtinfo_t* rti, RPPM_t* state, float RPPM, k32u routnum, k32u snapshot) {
    float proc = RPPM * min(TO_SECONDS(rti->timestamp - state->lasttimeattemps), 10.0f) / 60.0f;
    state->lasttimeattemps = rti->timestamp;
    proc *= max(1.0f, 1.0f + (min(TO_SECONDS(rti->timestamp - state->lasttimeprocs), 1000.0f) / (60.0f / RPPM) - 1.5f) * 3.0f);
    if (uni_rng(rti) < proc) {
        eq_enqueue(rti, rti->timestamp, routnum, snapshot);
        state->lasttimeprocs = rti->timestamp;
    }
}


void sim_init( rtinfo_t* rti, k32u seed, snapshot_t* ssbuf ) {
    /* Analogize get_global_id for CPU. */
    hostonly(
        static int gid = 0;
        set_global_id( 0, gid++ );
    )

    /* RNG. */
    rng_init( rti, seed );
    /* Snapshot manager. */
    snapshot_init( rti, ssbuf );

    /* Combat length. */
    assert( vary_combat_length < max_length ); /* Vary can't be greater than max. */
    assert( vary_combat_length + max_length < 2147483.647f );
    rti->expected_combat_length = FROM_SECONDS( max_length + vary_combat_length * clamp( stdnor_rng( rti ) * ( 1.0f / 3.0f ), -1.0f, 1.0f ) );

    /* Class module initializer. */
    module_init( rti );

    eq_enqueue( rti, rti->expected_combat_length, EVENT_END_SIMULATION, 0 );

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
    snapshot_t snapshot_buffer[ SNAPSHOT_SIZE ];
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
        ( k32u )deterministic_seed + ( k32u )get_global_id( 0 ),
        snapshot_buffer
    );

    while( eq_execute( &_rti ) );

    dps_result[get_global_id( 0 )] = _rti.damage_collected / TO_SECONDS( _rti.expected_combat_length );
}

/* Class module. */
void refresh_str(rtinfo_t* rti) {
    float fstr = rti->player.stat.gear_str;
    k32u str;
    float coeff = 1.0f;
    if (PLATE_SPECIALIZATION) coeff *= 1.05f;
    if (BUFF_STR_AGI_INT) coeff *= 1.05f;
    str = convert_uint_rtz(fstr * coeff);
    fstr = 1455; /* Base str @lvl 100. */
    fstr += racial_base_str[RACE]; /* Racial str. */
    str += convert_uint_rtz(fstr * coeff);
    rti->player.stat.str = str;
}

void refresh_ap(rtinfo_t* rti) {
    k32u ap = rti->player.stat.str;
    if (BUFF_AP) ap = convert_uint_rtz(ap * 1.1f + 0.5f);
    rti->player.stat.ap = ap;
}

void refresh_mastery(rtinfo_t* rti) {
    float mastery = rti->player.stat.gear_mastery;
    if (BUFF_MASTERY) mastery += 550;
    mastery = 1.4f * (0.08f + mastery / 11000);
    rti->player.stat.mastery = mastery;
}

void refresh_crit(rtinfo_t* rti) {
    float crit = rti->player.stat.gear_crit;
    crit *= 1.05f;
    crit = 0.05f + crit / 11000;
    if (BUFF_CRIT) crit += 0.05f;
    if (RACE == RACE_NIGHTELF_DAY || RACE == RACE_BLOODELF || RACE == RACE_WORGEN)
        crit += 0.01f;
    rti->player.stat.crit = crit;
}

void refresh_haste(rtinfo_t* rti) {
    float haste = rti->player.stat.gear_haste;
    haste = 1.0f + haste / 9000;
    if (BUFF_HASTE) haste *= 1.05f;
    if (RACE == RACE_NIGHTELF_NIGHT || RACE == RACE_GOBLIN || RACE == RACE_GNOME)
        haste *= 1.01f;
    rti->player.stat.haste = haste - 1.0f;
}

void refresh_mult(rtinfo_t* rti) {
    float mult = rti->player.stat.gear_mult;
    mult = mult / 6600;
    if (BUFF_MULT) mult += 0.05f;
    rti->player.stat.mult = mult;
}

void refresh_vers(rtinfo_t* rti) {
    float vers = rti->player.stat.gear_vers;
    if (RACE == RACE_HUMAN) vers += 100;
    vers = vers / 13000;
    if (BUFF_VERS) vers += 0.03f;
    rti->player.stat.vers = vers;
}

void mhproc(rtinfo_t* rti);
void ohproc(rtinfo_t* rti);
void approc(rtinfo_t* rti);

float weapon_dmg(rtinfo_t* rti, float weapon_multiplier, kbool normalized, kbool offhand) {
    float dmg = weapon[offhand].low;
    dmg += uni_rng(rti) * (weapon[offhand].high - weapon[offhand].low);
    dmg += ( normalized ? normalized_weapon_speed[weapon[offhand].type] : weapon[offhand].speed ) * rti->player.stat.ap / 3.5f;
    dmg *= weapon_multiplier;
    /* Crazed Berserker */
    if (offhand) dmg *= 0.5f * ( SINGLE_MINDED ? 1.5f : 1.25f );
    if (SINGLE_MINDED) dmg *= 1.3f;
    if (UP(enrage.expire)) {
        dmg *= 1.1f;
        dmg *= 1.0f + rti->player.stat.mastery;
    }
    dmg *= 1.0f + rti->player.stat.vers;
    if (offhand) ohproc(rti);
    else mhproc(rti);
    return dmg;
}

float ap_dmg(rtinfo_t* rti, float ap_multiplier) {
    float dmg = ap_multiplier * rti->player.stat.ap;
    if (SINGLE_MINDED) dmg *= 1.3f;
    if (UP(enrage.expire)) {
        dmg *= 1.1f;
        dmg *= 1.0f + rti->player.stat.mastery;
    }
    dmg *= 1.0f + rti->player.stat.vers;
    approc(rti);
    return dmg;
}

enum {
    DMGTYPE_NONE,
    DMGTYPE_MELEE,
    DMGTYPE_SPECIAL,
};
kbool deal_damage( rtinfo_t* rti, float dmg, k32u dmgtype, float extra_crit_rate ) {
    switch( dmgtype ) {
    case DMGTYPE_NONE:
        lprintf(("damage %.0f", dmg));
        rti->damage_collected += dmg;
        return 0;
        break;
    default: {
        float c = uni_rng(rti);
        float cr = rti->player.stat.crit - 0.03f + extra_crit_rate;
        kbool ret;
        float fdmg;

        dmg *= 0.650684f;

        fdmg = dmg;
        if (c < cr) {
            ret = 1;
            fdmg *= (RACE == RACE_DWARF || RACE == RACE_TAUREN) ? 2.04f : 2.0f;
            lprintf(("damage *%.0f*", fdmg));
        } else {
            ret = 0;
            lprintf(("damage %.0f", fdmg));
        }
        rti->damage_collected += fdmg;

        float mr = 0.5f * rti->player.stat.mult;
        float m = uni_rng(rti);
        if (m < mr) {
            c = uni_rng(rti);
            fdmg = dmg * 0.3f;
            if (c < cr) {
                fdmg *= (RACE == RACE_DWARF || RACE == RACE_TAUREN) ? 2.04f : 2.0f;
                lprintf(("mult damage *%.0f*", fdmg));
            } else {
                lprintf(("mult damage %.0f", fdmg));
            }
            rti->damage_collected += fdmg;
        }
        m = uni_rng(rti);
        if (m < mr) {
            c = uni_rng(rti);
            fdmg = dmg * 0.3f;
            if (c < cr) {
                fdmg *= (RACE == RACE_DWARF || RACE == RACE_TAUREN) ? 2.04f : 2.0f;
                lprintf(("mult damage *%.0f*", fdmg));
            } else {
                lprintf(("mult damage %.0f", fdmg));
            }
            rti->damage_collected += fdmg;
        }
        return ret;
    }
    break;
    }

}

/* Event list. */
#define DECL_EVENT( name ) void event_##name ( rtinfo_t* rti, k32u snapshot )
#define HOOK_EVENT( name ) case routnum_##name: event_##name( rti, e.snapshot ); break;
#define DECL_SPELL( name ) void spell_##name ( rtinfo_t* rti )
#define SPELL( name ) spell_##name ( rti )
enum {
    routnum_gcd_expire,
    routnum_bloodthirst_execute,
    routnum_bloodthirst_cd,
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
    routnum_sudden_death_trigger,
    routnum_sudden_death_expire,
};

void gcd_start ( rtinfo_t* rti, time_t length ) {
    rti->player.gcd = TIME_OFFSET( length );
    eq_enqueue( rti, rti->player.gcd, routnum_gcd_expire, 0 );
}

DECL_EVENT( gcd_expire ) {
    /* Do nothing. */
}

DECL_EVENT( bloodthirst_execute ) {
    float d = weapon_dmg(rti, 0.5f, 1, 0);

    power_gain( rti, 10.0f );
    rti->player.bloodthirst.cd = TIME_OFFSET( FROM_SECONDS( 4.5 / (1.0f + rti->player.stat.haste) ) );
    eq_enqueue( rti, rti->player.bloodthirst.cd, routnum_bloodthirst_cd, 0 );

    if (uni_rng(rti) < 0.2f) {
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 0.1 ) ), routnum_bloodsurge_trigger, 0 );
    }

    if ( deal_damage(rti, d, DMGTYPE_SPECIAL, 0.4f) ) {
        /* Crit */
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 0.1 ) ), routnum_enrage_trigger, 0 );
        lprintf(("bloodthirst crit"));

    } else {
        /* Hit */
        lprintf(("bloodthirst hit"));
    }

}

DECL_EVENT( bloodthirst_cd ) {
    lprintf(("bloodthirst ready"));
}

DECL_EVENT( ragingblow_execute ) {
    /* Main hand. */
    float d = weapon_dmg(rti, 2.0f, 1, 0);

    rti->player.ragingblow.stack --;
    if (rti->player.ragingblow.stack == 0) {
        rti->player.ragingblow.expire = 0;
        eq_enqueue( rti, rti->timestamp, routnum_ragingblow_expire, 0 );
        lprintf(("ragingblow expire"));
    }

    if (deal_damage(rti, d, DMGTYPE_SPECIAL, 0) ) {
        /* Crit */
        lprintf(("ragingblow crit"));

    } else {
        /* Hit */
        lprintf(("ragingblow hit"));
    }

    /* Off hand. */
    d = weapon_dmg(rti, 2.0, 1, 1);
    if (deal_damage(rti, d, DMGTYPE_SPECIAL, 0) ) {
        /* Crit */
        lprintf(("ragingblow oh crit"));

    } else {
        /* Hit */
        lprintf(("ragingblow oh hit"));
    }

}

DECL_EVENT( ragingblow_trigger ) {
    rti->player.ragingblow.stack ++;
    rti->player.ragingblow.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
    if (rti->player.ragingblow.stack > 2) {
        rti->player.ragingblow.stack = 2;
    }
    eq_enqueue( rti, rti->player.ragingblow.expire, routnum_ragingblow_expire, 0 );
    lprintf(("ragingblow stack %d", rti->player.ragingblow.stack));
}

DECL_EVENT( ragingblow_expire ) {
    if (rti->player.ragingblow.expire == rti->timestamp) {
        rti->player.ragingblow.stack = 0;
        lprintf(("ragingblow expire"));
    }
}

DECL_EVENT( enrage_trigger ) {
    power_gain( rti, 10.0f );
    rti->player.enrage.expire = TIME_OFFSET( FROM_SECONDS( 8 ) );
    eq_enqueue( rti, rti->timestamp, routnum_ragingblow_trigger, 0 );
    eq_enqueue( rti, rti->player.enrage.expire, routnum_enrage_expire, 0 );
    lprintf(("enrage trig"));
}

DECL_EVENT( enrage_expire ) {
    if (rti->player.enrage.expire == rti->timestamp) {
        lprintf(("enrage expire"));
    }
}

DECL_EVENT( execute_execute ) {
    /* Main hand. */
    float d = weapon_dmg(rti, 3.5f * 1.2f, 1, 0);
    if (SINGLE_MINDED) d *= 1.15f;

    if (deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf(("execute crit"));

    } else {
        /* Hit */
        lprintf(("execute hit"));
    }

    /* Off hand. */

    d = weapon_dmg(rti, 3.5f * 1.2f, 1, 1);
    if (SINGLE_MINDED) d *= 1.15f;

    if (deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf(("execute oh crit"));

    } else {
        /* Hit */
        lprintf(("execute oh hit"));
    }
}
DECL_EVENT( wildstrike_execute ) {
    float d = weapon_dmg(rti, 3.75f, 1, 1);

    if (deal_damage( rti, d, DMGTYPE_SPECIAL, 0 ) ) {
        /* Crit */
        lprintf(("wildstrike crit"));

    } else {
        /* Hit */
        lprintf(("wildstrike hit"));
    }
}

DECL_EVENT( bloodsurge_trigger ) {
    rti->player.bloodsurge.stack = 2;
    rti->player.bloodsurge.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
    eq_enqueue( rti, rti->player.bloodsurge.expire, routnum_bloodsurge_expire, 0 );
    lprintf(("bloodsurge trig"));
}

DECL_EVENT( bloodsurge_expire ) {
    if (rti->player.bloodsurge.expire == rti->timestamp) {
        rti->player.bloodsurge.stack = 0;
        rti->player.bloodsurge.expire = 0;
        lprintf(("bloodsurge expire"));
    }
}

DECL_EVENT( auto_attack_mh ) {
    float d = weapon_dmg(rti, 1.0f, 0, 0);

    if (uni_rng(rti) < 0.19f ) {
        /* Miss */
        lprintf(("mh miss"));
    } else {
        power_gain( rti, 3.5f * weapon[0].speed );
        proc_RPPM(rti, &rti->player.sudden_death_proc, 2.5f * (1.0f + rti->player.stat.haste), routnum_sudden_death_trigger, 0);
        if(deal_damage( rti, d, DMGTYPE_MELEE, 0)) {
            /* Crit */
            lprintf(("mh crit"));
        } else {
            /* Hit */
            lprintf(("mh hit"));
        }
    }

    eq_enqueue(rti, TIME_OFFSET( FROM_SECONDS( weapon[0].speed / (1.0f + rti->player.stat.haste) ) ), routnum_auto_attack_mh, 0);
}

DECL_EVENT( auto_attack_oh ) {
    float d = weapon_dmg(rti, 1.0f, 0, 1);

    if (uni_rng(rti) < 0.19f ) {
        /* Miss */
        lprintf(("oh miss"));
    } else {
        power_gain( rti, 3.5f * weapon[1].speed * 0.5f );
        proc_RPPM(rti, &rti->player.sudden_death_proc, 2.5f * (1.0f + rti->player.stat.haste), routnum_sudden_death_trigger, 0);
        if(deal_damage( rti, d, DMGTYPE_MELEE, 0)) {
            /* Crit */
            lprintf(("oh crit"));
        } else {
            /* Hit */
            lprintf(("oh hit"));
        }
    }

    eq_enqueue(rti, TIME_OFFSET( FROM_SECONDS( weapon[1].speed / (1.0f + rti->player.stat.haste) ) ), routnum_auto_attack_oh, 0);
}

DECL_EVENT(sudden_death_trigger) {
    rti->player.sudden_death.expire = TIME_OFFSET( FROM_SECONDS(10) );
    eq_enqueue(rti, rti->player.sudden_death.expire, routnum_sudden_death_expire, 0);
    lprintf(("suddendeath trig"));
}

DECL_EVENT( sudden_death_expire ) {
    if (rti->player.sudden_death.expire == rti->timestamp) {
        lprintf(("suddendeath expire"));
    }
}

DECL_SPELL( bloodthirst ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.bloodthirst.cd > rti->timestamp ) return;
    gcd_start( rti, FROM_SECONDS( 1.5 / (1.0f + rti->player.stat.haste) ) );
    eq_enqueue( rti, rti->timestamp, routnum_bloodthirst_execute, 0 );
    lprintf(("cast bloodthirst"));
}

DECL_SPELL( ragingblow ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !UP(ragingblow.expire) ) return;
    if ( !power_check( rti, 10.0f ) ) return;
    gcd_start( rti, FROM_SECONDS( 1.5 / (1.0f + rti->player.stat.haste) ) );
    power_consume( rti, 10.0f );
    eq_enqueue( rti, rti->timestamp, routnum_ragingblow_execute, 0 );
    lprintf(("cast ragingblow"));
}

DECL_SPELL( execute ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !UP(sudden_death.expire) ) {
        if ( enemy_health_percent(rti) >= 20.0f || !power_check( rti, 30.0f ) ) return;
        power_consume( rti, 30.0f );
    } else {
        rti->player.sudden_death.expire = 0;
        eq_enqueue( rti, rti->timestamp, routnum_sudden_death_expire, 0 );
    }
    gcd_start( rti, FROM_SECONDS( 1.5 / (1.0f + rti->player.stat.haste) ) );
    eq_enqueue( rti, rti->timestamp, routnum_execute_execute, 0 );
    lprintf(("cast execute"));
}

DECL_SPELL( wildstrike ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !UP(bloodsurge.expire) ) {
        if ( !power_check( rti, 45.0f ) ) return;
        power_consume( rti, 45.0f );
    } else {
        rti->player.bloodsurge.stack --;
        if (rti->player.bloodsurge.stack == 0) {
            rti->player.bloodsurge.expire = 0;
            eq_enqueue( rti, rti->timestamp, routnum_bloodsurge_expire, 0 );
            lprintf(("bloodsurge expire"));
        }
    }
    gcd_start( rti, FROM_SECONDS( 0.75 ) );
    eq_enqueue( rti, rti->timestamp, routnum_wildstrike_execute, 0 );
    lprintf(("cast wildstrike"));
}


void routine_entries( rtinfo_t* rti, _event_t e ) {
    switch(e.routine) {
        HOOK_EVENT( gcd_expire );
        HOOK_EVENT( bloodthirst_execute );
        HOOK_EVENT( bloodthirst_cd );
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
        HOOK_EVENT( sudden_death_trigger );
        HOOK_EVENT( sudden_death_expire );
    default:
        assert( 0 );
    }
}

void mhproc(rtinfo_t* rti) {

}
void ohproc(rtinfo_t* rti) {

}
void approc(rtinfo_t* rti) {

}

void module_init( rtinfo_t* rti ) {
    rti->player.power_regen = 0.0f;
    rti->player.power = 0.0f;
    eq_enqueue(rti, rti->timestamp, routnum_auto_attack_mh, 0);
    eq_enqueue(rti, TIME_OFFSET( FROM_SECONDS( 0.5 ) ), routnum_auto_attack_oh, 0);

    refresh_str(rti);
    refresh_ap(rti);
    refresh_crit(rti);
    refresh_haste(rti);
    refresh_mastery(rti);
    refresh_mult(rti);
    refresh_vers(rti);

    lprintf(("Raid buffed str %d", rti->player.stat.str));
    lprintf(("Raid buffed ap %d", rti->player.stat.ap));
    lprintf(("Raid buffed crit %f", rti->player.stat.crit));
    lprintf(("Raid buffed haste %f", rti->player.stat.haste));
    lprintf(("Raid buffed mastery %f", rti->player.stat.mastery));
    lprintf(("Raid buffed mult %f", rti->player.stat.mult));
    lprintf(("Raid buffed vers %f", rti->player.stat.vers));

    rti->player.sudden_death_proc.lasttimeattemps = FROM_SECONDS(10);
    rti->player.sudden_death_proc.lasttimeprocs = FROM_SECONDS(180);

}


/* Debug build. */
#if !defined(__OPENCL_VERSION__)
void scan_apl( rtinfo_t* rti ) {
    SPELL( bloodthirst );
    SPELL( execute );
    SPELL( ragingblow );
    SPELL( wildstrike );
}


void host_kernel_entry() {
    float result;
    sim_iterate( &result, 5171, 3945, 1714, 917, 1282, 478, 0 );

    printf( "result: %.3f\n", result );
}
#endif /* !defined(__OPENCL_VERSION__) */



