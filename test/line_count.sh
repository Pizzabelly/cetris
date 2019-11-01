#!/bin/bash
wc frontends/curses/curses_ui.c frontends/curses/meson.build \
  frontends/sdl/cetris_sdl.c frontends/sdl/cetris_sdl.h frontends/gl/drawable.c frontends/gl/drawable.h \
  frontends/gl/skin.c frontends/gl/skin.h frontends/gl/main.c frontends/gl/shader.c frontends/gl/shader.h \
  frontends/gl/ui.c frontends/gl/ui.h frontends/gl/meson.build lib/*
