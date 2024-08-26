#include "plugin.hpp"

#include <dlfcn.h>
#include <link.h>
#include <sys/mman.h>

void ( *console_msg )( char const *, ... );

static void *tier0_handle;
static void *client_handle;

static long page_size;

auto visualize_bbox_plugin::load(
  valve::create_interface factory,
  valve::create_interface server_factory
) -> bool {
  page_size = sysconf( _SC_PAGESIZE );

  tier0_handle = dlopen( "libtier0.so", RTLD_NOW );
  if ( !tier0_handle )
    return false;

  console_msg =
    ( void ( * )( char const *, ... ) ) dlsym( tier0_handle, "Msg" );
  if ( !console_msg )
    return false;

  console_msg( LOG_PRE "loaded!\n" );

  client_handle = dlopen( "cstrike/bin/client.so", RTLD_NOW );
  if ( !client_handle )
    return false;

  const auto base =
    reinterpret_cast< struct link_map * >( client_handle )->l_addr;
  const auto jz = base + VISUALIZE_COLLISION_BOUNDS_OFFSET;

  const auto aligned =
    reinterpret_cast< void * >( jz & ~( page_size - 1 ) );
  mprotect( aligned, page_size, PROT_READ | PROT_WRITE | PROT_EXEC );

  // Convert jz to jnz
  *reinterpret_cast< char * >( jz + 1 ) = 0x85;

  mprotect( aligned, page_size, PROT_READ | PROT_EXEC );

  console_msg( LOG_PRE "patched!\n" );

  return true;
};

auto visualize_bbox_plugin::unload( ) -> void {
  const auto base =
    reinterpret_cast< struct link_map * >( client_handle )->l_addr;
  const auto jmp = base + VISUALIZE_COLLISION_BOUNDS_OFFSET;

  const auto aligned =
    reinterpret_cast< void * >( jmp & ~( page_size - 1 ) );
  mprotect( aligned, page_size, PROT_READ | PROT_WRITE | PROT_EXEC );

  // Convert jnz to jz
  *reinterpret_cast< char * >( jmp + 1 ) = 0x84;

  mprotect( aligned, page_size, PROT_READ | PROT_EXEC );

  console_msg( LOG_PRE "reverted patch!\n" );

  dlclose( tier0_handle );
  dlclose( client_handle );
};
