cmake_minimum_required(VERSION 3.21)
project(CS2-Game LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(QtTest
    WIN32 MACOSX_BUNDLE
    main.cpp
    player.cpp
    player.hpp
    map.hpp map.cpp
    AudioManager.hpp
    Enemy.cpp Enemy.hpp
    slime.cpp slime.hpp
)

qt_add_resources(QtTest "images"
    FILES "resources/player/attacking/swordsman_1_attack.png"
    FILES "resources/player/idling/swordsman_1_idle.png"
    FILES "resources/player/running/swordsman_1_run.png"
    FILES "resources/map-assets/grass.png"
    FILES "resources/map-assets/left.png"
    FILES "resources/map-assets/right.png"
    FILES "resources/map-assets/down.png"
    FILES "resources/map-assets/up.png"
    FILES "resources/map-assets/center.png"
    FILES "resources/map-assets/up-right.png"
    FILES "resources/map-assets/right-up.png"
    FILES "resources/map-assets/left-up.png"
    FILES "resources/map-assets/house1.png"
    FILES "resources/map-assets/house2.png"
    FILES "resources/map-assets/house3.png"
    FILES "resources/map-assets/house4.png"
    FILES "resources/map-assets/grass1.png"
    FILES "resources/map-assets/grass2.png"
    FILES "resources/map-assets/grass3.png"
    FILES "resources/map-assets/grass4.png"
    FILES "resources/map-assets/grass5.png"
    FILES "resources/map-assets/grass6.png"
    FILES "resources/map-assets/tent1.png"
    FILES "resources/map-assets/tent2.png"
    FILES "resources/map-assets/tent3.png"
    FILES "resources/map-assets/tent4.png"
    FILES "resources/map-assets/stone1.png"
    FILES "resources/map-assets/stone2.png"
    FILES "resources/map-assets/topgatel.png"
    FILES "resources/map-assets/topgater.png"
    FILES "resources/map-assets/bottomgatel.png"
    FILES "resources/map-assets/bottomgater.png"
    FILES "resources/map-assets/wallbl.png"
    FILES "resources/map-assets/wallbr.png"
    FILES "resources/map-assets/wallt.png"
    FILES "resources/map-assets/wallt2.png"
    FILES "resources/map-assets/wallt3.png"
    FILES "resources/map-assets/walltb1.png"
    FILES "resources/map-assets/walltb2.png"
    FILES "resources/map-assets/walltb3.png"
    FILES "resources/map-assets/walltl.png"
    FILES "resources/map-assets/walltlb.png"
    FILES "resources/map-assets/walltr.png"
    FILES "resources/map-assets/walltrb.png"
    FILES "resources/map-assets/wallblb.png"
    FILES "resources/map-assets/wallbrb.png"
    FILES "resources/map-assets/Tile2_17.png"
    FILES "resources/map-assets/Tile2_30.png"
    FILES "resources/map-assets/sidewalll2.png"
    FILES "resources/map-assets/sidewallr2.png"
    FILES "resources/map-assets/sidegate1.png"
    FILES "resources/map-assets/sidegate2.png"
    FILES "resources/map-assets/sidegate3.png"
    FILES "resources/map-assets/sidegate4.png"
    FILES "resources/sound-effects/GrassWalk.wav"
    FILES "resources/sound-effects/SwordSwing.wav"
    FILES "resources/map-data/level-1.csv"
)


target_link_libraries(QtTest
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
)

include(GNUInstallDirs)

install(TARGETS QtTest
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtTest
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
