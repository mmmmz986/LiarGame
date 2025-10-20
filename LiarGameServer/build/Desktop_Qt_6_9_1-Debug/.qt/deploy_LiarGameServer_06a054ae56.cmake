include("/home/aiot122/QT_6/LiarGameServer/build/Desktop_Qt_6_9_1-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/LiarGameServer-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE /home/aiot122/QT_6/LiarGameServer/build/Desktop_Qt_6_9_1-Debug/LiarGameServer
    GENERATE_QT_CONF
)
