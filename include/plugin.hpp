#pragma once

#include "eleos.hpp"
#include "valve.hpp"

constexpr uintptr_t VISUALIZE_COLLISION_BOUNDS_OFFSET = 0x55e17b;

#define LOG_PRE "(visualize_bbox): "

class visualize_bbox_plugin
  : public eleos::interface
  , valve::plugin_callbacks::v3 {
public:
  visualize_bbox_plugin( ) noexcept
    : eleos::interface( "ISERVERPLUGINCALLBACKS003" ) { };

  auto description( ) -> const char * override {
    return "visualize_bbox";
  };

  auto load(
    valve::create_interface factory, valve::create_interface /*unused*/
  ) -> bool override;

  auto unload( ) -> void override;
};

// instantiate visualize_bbox_plugin class
visualize_bbox_plugin plugin;
