cmake_minimum_required(VERSION 3.21)
project(CS2-Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia)

qt_standard_project_setup()

file(GLOB_RECURSE HEADERS "include/*.hpp")

qt_add_executable(QtTest
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${HEADERS}
    src/player.cpp
    src/map.cpp
    src/Enemy.cpp
    src/slime.cpp
    src/characterstats.cpp
    src/gameview.cpp
    src/pausewindow.cpp
    src/campfire.cpp
)

target_include_directories(QtTest PUBLIC include)


qt_add_resources(QtTest "images"
    FILES 
        "resources/player/attacking/swordsman_1_attack.png"
        "resources/player/idling/swordsman_1_idle.png"
        "resources/player/running/swordsman_1_run.png"
        "resources/map-assets/grass.png"
        "resources/map-assets/left.png"
        "resources/map-assets/right.png"
        "resources/map-assets/down.png"
        "resources/map-assets/up.png"
        "resources/map-assets/center.png"
        "resources/map-assets/up-right.png"
        "resources/map-assets/right-up.png"
        "resources/map-assets/left-up.png"
        "resources/map-assets/right-down.png"
        "resources/map-assets/house1.png"
        "resources/map-assets/house2.png"
        "resources/map-assets/house3.png"
        "resources/map-assets/house4.png"
        "resources/map-assets/grass1.png"
        "resources/map-assets/grass2.png"
        "resources/map-assets/grass3.png"
        "resources/map-assets/grass4.png"
        "resources/map-assets/grass5.png"
        "resources/map-assets/grass6.png"
        "resources/map-assets/tent1.png"
        "resources/map-assets/tent2.png"
        "resources/map-assets/tent3.png"
        "resources/map-assets/tent4.png"
        "resources/map-assets/stone1.png"
        "resources/map-assets/stone2.png"
        "resources/map-assets/topgatel.png"
        "resources/map-assets/topgater.png"
        "resources/map-assets/bottomgatel.png"
        "resources/map-assets/bottomgater.png"
        "resources/map-assets/wallbl.png"
        "resources/map-assets/wallbr.png"
        "resources/map-assets/wallt.png"
        "resources/map-assets/wallt2.png"
        "resources/map-assets/wallt3.png"
        "resources/map-assets/walltb1.png"
        "resources/map-assets/walltb2.png"
        "resources/map-assets/walltb3.png"
        "resources/map-assets/walltl.png"
        "resources/map-assets/walltlb.png"
        "resources/map-assets/walltr.png"
        "resources/map-assets/walltrb.png"
        "resources/map-assets/wallblb.png"
        "resources/map-assets/wallbrb.png"
        "resources/map-assets/Tile2_17.png"
        "resources/map-assets/Tile2_30.png"
        "resources/map-assets/sidewalll2.png"
        "resources/map-assets/sidewallr2.png"
        "resources/map-assets/sidegate1.png"
        "resources/map-assets/sidegate2.png"
        "resources/map-assets/sidegate3.png"
        "resources/map-assets/sidegate4.png"
        "resources/sound-effects/GrassWalk.wav"
        "resources/sound-effects/SwordSwing.wav"
        "resources/map-data/level-1.csv"
        "resources/enemy/idle/Slime1_Idle_with_shadow.png"
        "resources/enemy/walk/Slime1_Walk_with_shadow.png"
        "resources/enemy/attack/Slime1_Attack_with_shadow.png"
        "resources/ui-elements/character_panel.png"
        "resources/map-assets/Tree_Emerald_2.png"
        "resources/map-assets/TreeBasic1.png"
        "resources/map-assets/TreeBasic2.png"
        "resources/map-assets/TreeBasic3.png"
        "resources/map-assets/TreeBasic4.png"
        "resources/map-assets/LogBasic1.png"
        "resources/map-assets/LogBasic2.png"
        "resources/fonts/pixelfont.ttf"
        "resources/map-assets/brokenlb.png"
        "resources/map-assets/brokenlt.png"
        "resources/map-assets/brokenb.png"
        "resources/map-assets/brokent.png"
        "resources/map-assets/brokenlog1.png"
        "resources/map-assets/brokenlog2.png"
        "resources/map-assets/remains1.png"
        "resources/map-assets/remains2.png"
        "resources/map-assets/remains3.png"
        "resources/map-assets/cart.png"
        "resources/map-assets/cutdownlogs.png"
        "resources/map-assets/axe.png"
        "resources/map-assets/lamp.png"
        "resources/map-assets/barrel.png"
        "resources/map-assets/hut.png"
        "resources/map-assets/campfire.png"
        "resources/map-assets/camplog1.png"
        "resources/map-assets/camplog2.png"
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
