#ifndef COMPILETIMECOUNTER_
#define COMPILETIMECOUNTER_
#include <cstddef>

#define COUNTER_READ_CRUMB( TAG, RANK, ACC ) AutoIDInternal::counter_crumb( TAG(), AutoIDInternal::constant_index< RANK >(), AutoIDInternal::constant_index< ACC >() )
#define COUNTER_READ( TAG ) COUNTER_READ_CRUMB( TAG, 1, COUNTER_READ_CRUMB( TAG, 2, COUNTER_READ_CRUMB( TAG, 4, COUNTER_READ_CRUMB( TAG, 8, \
    COUNTER_READ_CRUMB( TAG, 16, COUNTER_READ_CRUMB( TAG, 32, COUNTER_READ_CRUMB( TAG, 64, COUNTER_READ_CRUMB( TAG, 128, 0 ) ) ) ) ) ) ) )

#define COUNTER_INC( TAG ) \
namespace AutoIDInternal \
{ \
	constexpr \
	constant_index< COUNTER_READ( TAG ) + 1 > \
	counter_crumb( TAG, constant_index< ( COUNTER_READ( TAG ) + 1 ) & ~ COUNTER_READ( TAG ) >, \
                                                constant_index< ( COUNTER_READ( TAG ) + 1 ) & COUNTER_READ( TAG ) > ) { return {}; } \
}
#define COUNTER_LINK_NAMESPACE( NS ) using NS::counter_crumb;

namespace AutoIDInternal
{
	template< std::size_t n >
	struct constant_index : std::integral_constant< std::size_t, n > {};

	template< typename id, std::size_t rank, std::size_t acc >
	constexpr constant_index< acc > counter_crumb(id, constant_index< rank >, constant_index< acc >) { return {}; } // found by ADL via constant_index
}
#endif // COMPILETIMECOUNTER_
