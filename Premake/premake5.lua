include "common.lua"

workspace "FRAGILE"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }
  startproject "FeatureShowcase"

include (dirs.engine)
include (dirs.graphicsengine)
include (dirs.graphicsengineshaders)
include (dirs.assetmanager)
include (dirs.imgui)
include (dirs.logger)
include (dirs.math)
include (dirs.commonutilities)
include (dirs.projectcreator)
include (dirs.networkengine)

include (dirs.network .. "NetworkShared")

include (dirs.application .. "ModelViewer")
include (dirs.application .. "FeatureShowcase")
include(dirs.application .. "NetworkClient")
include(dirs.application .. "NetworkServer")
include(dirs.application .. "GrassRendering")